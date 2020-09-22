#include "ability.hpp"

#include "../util/range.hpp"
#include "audio.hpp"
#include "calc.hpp"
#include "character.hpp"
#include "character_status.hpp"
#include "data/types/type_ability.hpp"
#include "fov.hpp"
#include "game.hpp"
#include "i18n.hpp"
#include "input.hpp"
#include "message.hpp"
#include "random.hpp"
#include "text.hpp"
#include "variables.hpp"



namespace elona
{

namespace
{

int increase_potential(int potential, int level_delta)
{
    for (int i = 0; i < level_delta; ++i)
    {
        potential = std::min(calc_potential_on_gain(potential), 400);
    }
    return potential;
}



int decrease_potential(int potential, int level_delta)
{
    for (int i = 0; i < level_delta; ++i)
    {
        potential = std::max(calc_potential_on_loss(potential), 1);
    }
    return potential;
}



void set_ability(
    Character& chara,
    int id,
    int base_level,
    int experience,
    int potential)
{
    chara.get_skill(id).base_level = clamp(base_level, 0, 2000);
    chara.get_skill(id).experience = experience;
    chara.get_skill(id).potential = potential;
}

} // namespace



SkillData::SkillData()
    : _storage(600)
{
}



void chara_init_skill(Character& chara, int skill_id, int initial_level)
{
    int base_level = chara.get_skill(skill_id).base_level;
    int potential =
        calc_initial_skill_base_potential(skill_id, base_level, initial_level);
    int level;
    if (skill_id == 18)
    {
        level = calc_initial_skill_level_speed(initial_level, chara.level);
    }
    else
    {
        level = calc_initial_skill_level(initial_level, chara.level, potential);
    }
    potential = calc_initial_skill_decayed_potential(chara.level, potential);
    if (potential < 1)
    {
        potential = 1;
    }
    // life, luck, mana
    if (skill_id == 2 || skill_id == 19 || skill_id == 3)
    {
        level = initial_level;
        potential = 100;
    }
    if (base_level + level > 2000)
    {
        level = 2000 - base_level;
    }
    chara.get_skill(skill_id).base_level += clamp(level, 0, 2000);
    chara.get_skill(skill_id).potential += potential;
}



void chara_init_common_skills(Character& chara)
{
    for (int element = 50; element < 61; ++element)
    {
        auto level = calc_initial_resistance_level(
            chara, chara.get_skill(element).level, element);
        chara.get_skill(element).base_level = clamp(level, 1, 2000);
        chara.get_skill(element).experience = 0;
        chara.get_skill(element).potential = 0;
    }

    chara_init_skill(chara, 100, 4);
    chara_init_skill(chara, 101, 4);
    chara_init_skill(chara, 103, 4);
    chara_init_skill(chara, 102, 4);
    chara_init_skill(chara, 104, 4);
    chara_init_skill(chara, 105, 4);
    chara_init_skill(chara, 107, 4);
    chara_init_skill(chara, 108, 4);
    chara_init_skill(chara, 111, 4);
    chara_init_skill(chara, 109, 4);
    chara_init_skill(chara, 173, 4);
    chara_init_skill(chara, 154, 4);
    chara_init_skill(chara, 155, 4);
    chara_init_skill(chara, 106, 4);
    chara_init_skill(chara, 157, 4);
    chara_init_skill(chara, 181, 4);
    chara_init_skill(chara, 171, 4);
    chara_init_skill(chara, 170, 4);
    chara_init_skill(chara, 169, 4);
    chara_init_skill(chara, 168, 3);
    chara_init_skill(chara, 19, 50);
}



void chara_gain_skill(Character& chara, int id, int initial_level, int stock)
{
    if (id >= 400)
    {
        if (chara.is_player())
        {
            chara.spell_stocks().gain(
                *the_ability_db.get_id_from_integer(id), stock);
            modify_potential(chara, id, 1);
        }
    }
    if (chara.get_skill(id).base_level != 0)
    {
        if (id < 400)
        {
            modify_potential(chara, id, 20);
        }
        return;
    }
    if (id >= 400)
    {
        modify_potential(chara, id, 200);
    }
    else
    {
        modify_potential(chara, id, 50);
    }
    chara.get_skill(id).base_level =
        clamp(chara.get_skill(id).base_level + initial_level, 1, 2000);

    chara_refresh(chara);
}



void gain_special_action()
{
    if (cdata.player().get_skill(174).base_level > 15)
    {
        if (spact(29) == 0)
        {
            spact(29) = 1;
            txt(i18n::s.get(
                    "core.skill.gained",
                    the_ability_db.get_text("core.draw_charge", "name")),
                Message::color{ColorIndex::orange});
        }
        if (spact(30) == 0)
        {
            spact(30) = 1;
            txt(i18n::s.get(
                    "core.skill.gained",
                    the_ability_db.get_text("core.fill_charge", "name")),
                Message::color{ColorIndex::orange});
        }
    }
    if (cdata.player().get_skill(152).base_level > 15)
    {
        if (spact(31) == 0)
        {
            spact(31) = 1;
            txt(i18n::s.get(
                    "core.skill.gained",
                    the_ability_db.get_text("core.swarm", "name")),
                Message::color{ColorIndex::orange});
        }
    }
}



void chara_gain_fixed_skill_exp(Character& chara, int id, int experience)
{
    auto lv = chara.get_skill(id).base_level;
    auto exp = chara.get_skill(id).experience + experience;
    auto potential = chara.get_skill(id).potential;

    if (potential == 0)
        return;

    if (exp >= 1000)
    {
        const auto lv_delta = exp / 1000;
        lv += lv_delta;
        exp = exp % 1000;
        potential = decrease_potential(potential, lv_delta);
        set_ability(chara, id, lv, exp, potential);
        if (is_in_fov(chara))
        {
            if (chara.is_player() || chara.is_player_or_ally())
            {
                snd("core.ding3");
                Message::instance().txtef(ColorIndex::green);
            }
            txt(txtskillchange(chara, id, true));
        }
        chara_refresh(chara);
        return;
    }
    if (exp < 0)
    {
        auto lv_delta = -exp / 1000 + 1;
        exp = 1000 + exp % 1000;
        if (lv - lv_delta < 1)
        {
            lv_delta = lv - 1;
            if (lv == 1)
            {
                if (lv_delta == 0)
                {
                    exp = 0;
                }
            }
        }
        lv -= lv_delta;
        potential = increase_potential(potential, lv_delta);
        set_ability(chara, id, lv, exp, potential);
        if (chara.is_player() || chara.is_player_or_ally())
        {
            if (is_in_fov(chara))
            {
                if (lv_delta != 0)
                {
                    txt(txtskillchange(chara, id, false),
                        Message::color{ColorIndex::red});
                }
            }
        }
        chara_refresh(chara);
        return;
    }
    set_ability(chara, id, lv, exp, potential);
}



void chara_gain_skill_exp(
    Character& chara,
    int id,
    int experience,
    int experience_divisor_of_related_basic_attribute,
    int experience_divisor_of_character_level)
{
    if (chara.get_skill(id).base_level == 0)
        return;
    if (experience == 0)
        return;

    if (the_ability_db[id]->related_basic_attribute != 0)
    {
        // Gain experience in the basic attribute the skill is related to.
        chara_gain_skill_exp(
            chara,
            the_ability_db[id]->related_basic_attribute,
            calc_skill_related_attribute_exp(
                experience, experience_divisor_of_related_basic_attribute));
    }

    auto lv = chara.get_skill(id).base_level;
    auto potential = chara.get_skill(id).potential;
    if (potential == 0)
        return;

    int exp;
    if (experience > 0)
    {
        exp = calc_base_skill_exp_gained(experience, potential, lv);
        if (id >= 10 && id <= 19)
        {
            exp =
                calc_boosted_skill_exp_gained(exp, chara.growth_buffs[id - 10]);
        }
        if (exp == 0)
        {
            if (rnd(lv / 10 + 1) == 0)
            {
                exp = 1;
            }
            else
            {
                return;
            }
        }
    }
    else
    {
        exp = experience;
    }
    if (game()->current_map == mdata_t::MapId::show_house)
    {
        exp /= 5;
    }
    if (exp > 0)
    {
        if (id >= 100)
        {
            if (experience_divisor_of_character_level != 1000)
            {
                const auto lvl_exp = calc_chara_exp_from_skill_exp(
                    chara, exp, experience_divisor_of_character_level);
                chara.experience += lvl_exp;
                if (chara.is_player())
                {
                    game()->sleep_experience += lvl_exp;
                }
            }
        }
    }

    int new_exp_level = exp + chara.get_skill(id).experience;
    if (new_exp_level >= 1000)
    {
        const auto lv_delta = new_exp_level / 1000;
        new_exp_level = new_exp_level % 1000;
        lv += lv_delta;
        potential = decrease_potential(potential, lv_delta);
        set_ability(chara, id, lv, new_exp_level, potential);
        if (is_in_fov(chara))
        {
            if (chara.is_player() || chara.is_player_or_ally())
            {
                snd("core.ding3");
                Message::instance().txtef(ColorIndex::green);
                input_halt_input(HaltInput::alert);
            }
            txt(txtskillchange(chara, id, true));
        }
        chara_refresh(chara);
        return;
    }
    if (new_exp_level < 0)
    {
        auto lv_delta = -new_exp_level / 1000 + 1;
        new_exp_level = 1000 + new_exp_level % 1000;
        if (lv - lv_delta < 1)
        {
            lv_delta = lv - 1;
            if (lv == 1)
            {
                if (lv_delta == 0)
                {
                    new_exp_level = 0;
                }
            }
        }
        lv -= lv_delta;
        potential = increase_potential(potential, lv_delta);
        set_ability(chara, id, lv, new_exp_level, potential);
        if (is_in_fov(chara))
        {
            if (chara.is_player() || chara.is_player_or_ally())
            {
                if (lv_delta != 0)
                {
                    input_halt_input(HaltInput::alert);
                    txt(txtskillchange(chara, id, false),
                        Message::color{ColorIndex::red});
                }
            }
        }
        chara_refresh(chara);
        return;
    }

    set_ability(chara, id, lv, new_exp_level, potential);
}



void chara_gain_exp_digging(Character& chara)
{
    chara_gain_skill_exp(chara, 163, 100);
}



void chara_gain_exp_literacy(Character& chara)
{
    chara_gain_skill_exp(chara, 150, 15, 10, 100);
}



void chara_gain_exp_negotiation(Character& chara, int gold)
{
    const auto current_level = chara.get_skill(156).level;
    if (gold >= calc_exp_gain_negotiation_gold_threshold(current_level))
    {
        chara_gain_skill_exp(
            chara, 156, calc_exp_gain_negotiation(gold, current_level), 10);
    }
}



void chara_gain_exp_lock_picking(Character& chara)
{
    chara_gain_skill_exp(chara, 158, 100);
}



void chara_gain_exp_detection(Character& chara)
{
    chara_gain_skill_exp(
        chara, 159, calc_exp_gain_detection(game()->current_dungeon_level));
}



void chara_gain_exp_casting(Character& chara, int spell_id)
{
    if (chara.is_player())
    {
        chara_gain_skill_exp(
            chara, spell_id, calc_spell_exp_gain(spell_id), 4, 5);
        chara_gain_skill_exp(chara, 172, calc_exp_gain_casting(spell_id), 5);
    }
    else
    {
        chara_gain_skill_exp(chara, 172, calc_exp_gain_casting(spell_id), 5);
    }
}



void chara_gain_exp_mana_capacity(Character& chara)
{
    chara_gain_skill_exp(chara, 164, calc_exp_gain_mana_capacity(chara));
}



void chara_gain_exp_healing_and_meditation(Character& chara)
{
    chara_gain_skill_exp(chara, 154, calc_exp_gain_healing(chara), 1000);
    chara_gain_skill_exp(chara, 155, calc_exp_gain_meditation(chara), 1000);
}



void chara_gain_exp_stealth(Character& chara)
{
    chara_gain_skill_exp(chara, 157, calc_exp_gain_stealth(), 0, 1000);
}



void chara_gain_exp_investing(Character& chara)
{
    chara_gain_skill_exp(chara, 160, 600);
}



void chara_gain_exp_weight_lifting(Character& chara)
{
    chara_gain_skill_exp(
        chara, 153, calc_exp_gain_weight_lifting(chara), 0, 1000);
}



void chara_gain_exp_magic_device(Character& chara)
{
    if (chara.is_player())
    {
        chara_gain_skill_exp(chara, 174, 40);
    }
}



void chara_gain_exp_fishing(Character& chara)
{
    chara_gain_skill_exp(chara, 185, 100);
}



void chara_gain_exp_memorization(Character& chara, int spell_id)
{
    chara_gain_skill_exp(chara, 165, calc_exp_gain_memorization(spell_id));
}



void chara_gain_exp_crafting(Character& chara, int skill, int material_amount)
{
    chara_gain_skill_exp(chara, skill, calc_exp_gain_crafting(material_amount));
}



void chara_gain_exp_disarm_trap(Character& chara)
{
    chara_gain_skill_exp(chara, 175, 50);
}



int randskill()
{
    return rnd(40) + 150;
}



int randattb()
{
    return rnd(8) + 10;
}

} // namespace elona
