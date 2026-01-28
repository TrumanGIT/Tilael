#pragma once

#include "SKSEMenuFramework.h"

namespace UI {

   static  const auto healthIcon = FontAwesome::UnicodeToUtf8(0xf004);   // heart for Health
   static  const auto magickaIcon = FontAwesome::UnicodeToUtf8(0xf72b);  // wand for Magicka
   static  const auto staminaIcon = FontAwesome::UnicodeToUtf8(0xf70c);  // running for Stamina

   static  const auto upArrowIcon = FontAwesome::UnicodeToUtf8(0xf004);   //up arrow for increasing skills

    void Register();
    void __stdcall renderTilaelMenu();

    extern RE::TESQuest* tilaelQuest;

    extern RE::ObjectRefHandle tilaelRefHandle;

    extern RE::Actor* tilaelActor;

    extern unordered_map<std::string, std::string>perks;

    bool getTilaelActor(); 

    void displayVital(RE::ActorValue actorValue,const std::string& icon, ImGuiMCP::ImVec4 color);

    void getPerksForNPC(RE::Actor* actor);

    inline MENU_WINDOW tilaelMenuWindow;

};