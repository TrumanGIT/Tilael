#pragma once

#include "SKSEMenuFramework.h"

#include <unordered_map>

namespace UI {

   static  const auto healthIcon = FontAwesome::UnicodeToUtf8(0xf004);   // heart for Health
   static  const auto magickaIcon = FontAwesome::UnicodeToUtf8(0xf72b);  // wand for Magicka
   static  const auto staminaIcon = FontAwesome::UnicodeToUtf8(0xf70c);  // running for Stamina

   static  const auto upArrowIcon = FontAwesome::UnicodeToUtf8(0xf004);   //up arrow for increasing skills


   extern RE::TESTopic* TFQuest_TFQuestSilentLine_00005A96_1;

    void Register();
    void __stdcall renderTilaelMenu();

    extern RE::TESQuest* tilaelQuest;

    extern RE::ObjectRefHandle tilaelRefHandle;

    extern RE::Actor* tilaelActor;

    extern unordered_map<std::string, std::string>perks;

    bool getTilaelActor(); 

    void displayVital(RE::ActorValue actorValue,const std::string& icon, ImGuiMCP::ImVec4 color);

    void getPerksForNPC(RE::Actor* actor);

    RE::TESGlobal* GetGlobalForActorValue(RE::ActorValue av);

    void ResetSkillGlobals();

    inline void PapyrusSay(RE::TESObjectREFR* target, RE::TESTopic* toSay, RE::Character* toSpeakAs, bool playerHead) {
        if (target && toSay) {
            using func_t = void (*)(std::int64_t, std::int64_t, RE::TESObjectREFR*, RE::TESTopic*, RE::Character*, bool);
            static REL::Relocation<func_t> func{ REL::VariantID(
                55689, 56220, 0x9D1470) };  // takes SE, AE, then VR offset b/c VR might no exist they are filled manually
            return func(NULL, NULL, target, toSay, toSpeakAs, playerHead);
        }
    }

    inline MENU_WINDOW tilaelMenuWindow;

};