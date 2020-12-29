--- Functions to be used inside translations, like "${itemname(_1, _2, _3)}".
local FOV = ELONA.require("core.FOV")
local I18N = ELONA.require("core.I18N")
local Item = ELONA.require("core.Item")
local string = string

local i18n = {}

-- local print = print
-- local tostring = tostring
-- local type = type

--
-- English
--

i18n.en = {}

function i18n.en.ordinal(n)
   if n % 10 == 1 and n ~= 11 then
      return tostring(n) .. "st"
   elseif n % 10 == 2 and n ~= 12 then
      return tostring(n) .. "nd"
   elseif n % 10 == 3 and n ~= 13 then
      return tostring(n) .. "rd"
   else
      return tostring(n) .. "th"
   end
end

function i18n.en.trim_job(name_with_job)
   return string.gsub(name_with_job, " .*", " ")
end

function i18n.en.capitalize(str)
   if str == "" then
      return str
   end

   return str:gsub("^%l", string.upper)
end

function i18n.en.he(chara, ignore_sight)
   if not chara then
      return "it"
   end

   if ignore_sight then
      if chara.sex == "male" then
         return "he"
      else
         return "she"
      end
   end

   if chara.index < 0 or chara.index >= 245 then
      return "it"
   elseif not FOV.you_see(chara) then
      return "it"
   elseif chara.index == 0 then
      return "you"
   elseif chara.sex == "male" then
      return "he"
   else
      return "she"
   end
end

function i18n.en.his(chara, ignore_sight)
   if not chara then
      return "its"
   end

   if ignore_sight then
      if chara.index == 0 then
         return "your"
      elseif chara.sex == "male" then
         return "his"
      else
         return "her"
      end
   end

   if chara.index < 0 or chara.index >= 245 then
      return "its"
   elseif not FOV.you_see(chara) then
      return "its"
   elseif chara.index == 0 then
      return "your"
   elseif chara.sex == "male" then
      return "his"
   else
      return "her"
   end
end

function i18n.en.him(chara, ignore_sight)
   if not chara then
      return "it"
   end

   if ignore_sight then
      if chara.sex == "male" then
         return "him"
      else
         return "her"
      end
   end

   if chara.index < 0 or chara.index >= 245 then
      return "it"
   elseif not FOV.you_see(chara) then
      return "it"
   elseif chara.index == 0 then
      return "yourself"
   elseif chara.sex == "male" then
      return "him"
   else
      return "her"
   end
end

function i18n.en.is(object)
   if not object then
      return "is"
   end

   if object == true then
      return "are"
   elseif object == false then
      return "is"
   end

   if object.lua_type == "LuaItem" then
      if object.number > 1 then
         return "are"
      else
         return "is"
      end
   end

   if object.index == 0 then
      return "are"
   else
      return "is"
   end
end

function i18n.en.s(object, need_e)
   if not object then
      return need_e and "es" or "s"
   end

   if object == true then
      if need_e then
         return "es"
      else
         return "s"
      end
   elseif object == false then
      return ""
   end

   if type(object) == "number" then
      if object == 1 then
         return ""
      else
         return "s"
      end
   end

   if object.lua_type == "LuaItem" then
      if object.number == 1 then
         return ""
      else
         return "s"
      end
   end

   if object.index == 0 then
      return ""
   elseif need_e then
      return "es"
   else
      return "s"
   end
end

function i18n.en.does(object)
   if not object then
      return "does"
   end

   if object.lua_type == "LuaItem" then
      if object.number == 1 then
         return "do"
      else
         return "does"
      end
   end

   if object.index == 0 then
      return "do"
   else
      return "does"
   end
end

function i18n.en.have(chara)
   if not chara then
      return "has"
   end

   if chara.index == 0 then
      return "have"
   else
      return "has"
   end
end

function i18n.en.himself(chara)
   if not chara then
      return "itself"
   end

   if chara.index < 0 or chara.index >= 245 then
      return "itself"
   elseif not FOV.you_see(chara) then
      return "itself"
   elseif chara.index == 0 then
      return "yourself"
   elseif chara.sex == "male" then
      return "himself"
   else
      return "herself"
   end
end

function i18n.en.his_owned(chara)
   if not chara then
      return "'s"
   end

   if chara.index == 0 then
      return "r"
   else
      return "'s"
   end
end

function i18n.en.name_nojob(chara)
   return i18n.en.trim_job(chara.basename)
end


--
-- Japanese
--

i18n.ja = {}

function i18n.ja.ordinal(n)
   return tostring(n)
end

function i18n.ja.he(chara)
   if not chara then
      return "彼"
   end

   if chara.sex == "male" then
      return "彼"
   else
      return "彼女"
   end
end

function i18n.ja.his(chara)
   if not chara then
      return "彼の"
   end

   if chara.index == 0 then
      return "あなたの"
   elseif chara.sex == "male" then
      return "彼の"
   else
      return "彼女の"
   end
end

function i18n.ja.him(chara)
   if not chara then
      return "彼"
   end

   if chara.sex == "male" then
      return "彼"
   else
      return "彼女"
   end
end

function i18n.ja.kare_wa(chara)
   if not chara then
      return "それは"
   end

   if chara.index == 0 then
      return ""
   elseif not FOV.you_see(chara) then
      return "それは"
   else
      return chara.basename .. "は"
   end
end


--
-- Used in both English & Japanese
--

i18n.all = {}

function i18n.all.you()
   return I18N.get("core.chara.you")
end

function i18n.all.name(chara)
   if not chara or not chara.name then
      return I18N.get("core.chara.something")
   end
   return chara.name
end

function i18n.all.basename(chara)
   if not chara or not chara.name then
      return I18N.get("core.chara.something")
   end
   return chara.basename
end

function i18n.all.itemname(item, number, needs_article)
   if not item then
      return I18N.get("core.chara.something")
   end
   if item.lua_type ~= "LuaItem" then
      return I18N.get("core.chara.something")
   end
   if number == nil then
      number = item.number
   end
   if needs_article == nil then
      needs_article = true
   end

   return Item.itemname(item, number, needs_article)
end

function i18n.all.itembasename(item)
   if not item then
      return I18N.get("core.chara.something")
   end
   if item.lua_type ~= "LuaItem" then
      return I18N.get("core.chara.something")
   end
   return item.basename
end

function i18n.all.zfill(n, width)
   return string.format("%0"..tostring(width).."d", n)
end


--
-- Function registration.
--

for name, func in pairs(i18n.en) do
   I18N.register_function("en", name, func)
end

for name, func in pairs(i18n.ja) do
   I18N.register_function("ja", name, func)
end

for name, func in pairs(i18n.all) do
   I18N.register_function("en", name, func)
   I18N.register_function("ja", name, func)
end
