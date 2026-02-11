
#include "Menu.h"
#include "plugin.hpp"
#include "TilaelData.h"
#include <unordered_map>

namespace logger = SKSE::log;

namespace UI {

static int tempSkillPoints = 0;

    void Register() {
        if (!SKSEMenuFramework::IsInstalled()) return;

        SKSEMenuFramework::SetSection("Tilael");

        SKSEMenuFramework::AddSectionItem("Settings", UI::renderTilaelMenu);
    }

    void __stdcall renderTilaelMenu() {

        static bool initialized = false;

        static bool skillPointsSelected = false;

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.455f, 0.753f, 0.988f, 1.0f });

        FontAwesome::PushSolid();
        auto iconUtf8 = FontAwesome::UnicodeToUtf8(0xf043);

        ImGuiMCP::Text("%s Tilael (Level %d)", iconUtf8.c_str(), tilaelData.level);
        ImGuiMCP::PopStyleColor();

        if (tempSkillPoints == 0 && skillPointsSelected) {

            ImGuiMCP::SameLine();

            if (ImGuiMCP::Button("Level Up")) {
                PapyrusSay(tilaelActor, TFQuest_TFQuestSilentLine_00005A96_1, nullptr, false);
                tilaelData.skillPoints -= 5; 
                tilaelData.level++;
                saveConfiguration(tilaelData, tilaelData.configPath);
                skillPointsSelected = false;
                initialized = false;
            }
            if (ImGuiMCP::IsItemHovered())
                ImGuiMCP::SetTooltip("Spend Skill Points");
        }

        ImGuiMCP::Separator();

        if (initialized == false){

        if (!getTilaelActor()) return;

        tempSkillPoints = tilaelData.skillPoints;

        if (tempSkillPoints > 5) {
            tempSkillPoints = 5;
        }

      //  ResetSkillGlobals();

        fillTempAVMap(tilaelActor, tempAVs); 

        fillTempAVMap(tilaelActor, intialAVs);

        }

        getPerksForNPC(tilaelActor);

        getSpellsForNPC(tilaelActor);

        initialized = true;

        ImGuiMCP::Separator();
       // ImGuiMCP::Spacing();

        FontAwesome::PushSolid();

        displayVital(RE::ActorValue::kHealth, healthIcon, { 1.0f, 0.0f, 0.0f, 1.0f });

        ImGuiMCP::SameLine();
        displayVital(RE::ActorValue::kMagicka, magickaIcon, { 0.0f, 0.5f, 1.0f, 1.0f });

        ImGuiMCP::SameLine();
        displayVital(RE::ActorValue::kStamina, staminaIcon, { 0.4f, 1.0f, 0.4f, 1.0f });

        ImGuiMCP::Separator();
        ImGuiMCP::Spacing();

        ImGuiMCP::TextColored(ImGuiMCP::ImVec4{ 0.5f, 0.7f, 1.f, 1.f }, "Skills");
        ImGuiMCP::SameLine();

        float right = ImGuiMCP::GetWindowContentRegionMax().x;
        ImGuiMCP::SetCursorPosX(right - 545.0f); 

        ImGuiMCP::TextColored(
            ImGuiMCP::ImVec4{ 0.5f, 0.7f, 1.f, 1.f },
            "SkillPoints: %d",
            tempSkillPoints
        );
  
        ImGuiMCP::Columns(2, nullptr, false);

        ImGuiMCP::SetColumnWidth(0, 500);
        auto SkillRow = [&](const char* name, RE::ActorValue av)
            {
                const auto avAsInt = static_cast<int>(av);

                ImGuiMCP::Text("%s", name);
                ImGuiMCP::NextColumn();

                ImGuiMCP::Text("%.0f", tempAVs[avAsInt]);
                ImGuiMCP::SameLine();

                ImGuiMCP::PushID(avAsInt);

                bool canIncrement = tempSkillPoints > 0;

                ImGuiMCP::BeginDisabled(!canIncrement);

                ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button,
                    canIncrement ? ImGuiMCP::ImVec4{ 0.5f,0.7f,1.f,1.f } : ImGuiMCP::ImVec4{ 0,0,0,0 });
                ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonHovered,
                    ImGuiMCP::ImVec4{ 0.6f,0.8f,1.f,1.f });
                ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonActive,
                    ImGuiMCP::ImVec4{ 0.4f,0.6f,0.9f,1.f });

                if (ImGuiMCP::SmallButton("+") && canIncrement)
                {
                    if (auto global = GetGlobalForActorValue(av)) {
                        global->value += 1;
                        tempSkillPoints--;
                        tempAVs[avAsInt]++;
                        skillPointsSelected = true;
                    }
                }

                ImGuiMCP::PopStyleColor(3);
                ImGuiMCP::EndDisabled();

                ImGuiMCP::SameLine();

                bool canDecrement = false;
                if (auto global = GetGlobalForActorValue(av)) {
                    canDecrement =
                        skillPointsSelected &&
                        global->value > 0 &&
                        tempAVs[avAsInt] > intialAVs[avAsInt];
                }

                ImGuiMCP::BeginDisabled(!canDecrement);

                ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button,
                    canDecrement ? ImGuiMCP::ImVec4{ 0.85f,0.2f,0.2f,1.f } : ImGuiMCP::ImVec4{ 0,0,0,0 });
                ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonHovered,
                    ImGuiMCP::ImVec4{ 0.95f,0.3f,0.3f,1.f });
                ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonActive,
                    ImGuiMCP::ImVec4{ 0.75f,0.15f,0.15f,1.f });

                if (ImGuiMCP::SmallButton("-") && canDecrement)
                {
                    if (auto global = GetGlobalForActorValue(av)) {
                        global->value--;
                        tempSkillPoints++;
                        tempAVs[avAsInt]--;
                        if (checkIfGlobalsZeroed())
                            skillPointsSelected = false;
                    }
                }

                ImGuiMCP::PopStyleColor(3);
                ImGuiMCP::EndDisabled();

                ImGuiMCP::PopID();
                ImGuiMCP::NextColumn();
            };

        SkillRow("One-Handed", RE::ActorValue::kOneHanded);
        SkillRow("Archery", RE::ActorValue::kArchery);
        SkillRow("Light-Armor", RE::ActorValue::kLightArmor);
        SkillRow("Sneak", RE::ActorValue::kSneak);
        SkillRow("Destruction", RE::ActorValue::kDestruction);
        SkillRow("Restoration", RE::ActorValue::kRestoration);
        ImGuiMCP::EndColumns();
        ImGuiMCP::Separator();
        ImGuiMCP::Spacing();
        ImGuiMCP::TextColored(ImGuiMCP::ImVec4{ 0.5f, 0.7f, 1.f, 1.f }, "Perks");

        ImGuiMCP::SameLine();
        float rightSpells = ImGuiMCP::GetWindowContentRegionMax().x;
        ImGuiMCP::SetCursorPosX(rightSpells - 535.0f);
        ImGuiMCP::TextColored(ImGuiMCP::ImVec4{ 0.5f, 0.7f, 1.f, 1.f }, "Spells");

      
        float childWidth = 500.0f; 
        float childHeight = 160.0f;


        ImGuiMCP::BeginChild("PerkList", ImGuiMCP::ImVec2(childWidth, childHeight), true, ImGuiMCP::ImGuiWindowFlags_ChildWindow);


        const int perksPerColumn = 4;
        const int numColumns = 4;
        ImGuiMCP::Columns(numColumns, nullptr, false);
        for (int i = 0; i < numColumns; i++)
            ImGuiMCP::SetColumnWidth(i, 170.0f);

        int count = 0;
        for (const auto& [perk, descrip] : perks) {
            ImGuiMCP::TextWrapped("%s", perk.c_str());
            if (ImGuiMCP::IsItemHovered())
                ImGuiMCP::SetTooltip("%s", descrip.c_str());

            count++;
            if (count % perksPerColumn == 0)
                ImGuiMCP::NextColumn();
        }

        ImGuiMCP::Unindent(10);
        ImGuiMCP::EndChild();

       
        ImGuiMCP::SameLine();
        ImGuiMCP::BeginChild("SpellList", ImGuiMCP::ImVec2(childWidth, childHeight), true, ImGuiMCP::ImGuiWindowFlags_ChildWindow);
        ImGuiMCP::Indent(10);

        ImGuiMCP::Columns(numColumns, nullptr, false);
        for (int i = 0; i < numColumns; i++)
            ImGuiMCP::SetColumnWidth(i, 150.0f);

        int spellCount = 0;
        for (const auto& [spell, desc] : spells) {
            ImGuiMCP::TextWrapped("%s", spell.c_str());
           // if (ImGuiMCP::IsItemHovered())
               // ImGuiMCP::SetTooltip("%s", desc.c_str());

            spellCount++;
            if (spellCount % perksPerColumn == 0)
                ImGuiMCP::NextColumn();
        }

        ImGuiMCP::Unindent(10);
        ImGuiMCP::EndChild();

        ImGuiMCP::End();

    }

    //  show a copy of the stats to simulate level increasing when pusshing the button
    // skills only actually changed when clicking level up button.
     void fillTempAVMap(RE::Actor* actor, std::unordered_map<int, float>& avMap) {
         if (!actor) return; 
        for (RE::ActorValue av : {
            RE::ActorValue::kOneHanded, RE::ActorValue::kArchery,
                RE::ActorValue::kSneak, RE::ActorValue::kLightArmor, RE::ActorValue::kHeavyArmor,
                RE::ActorValue::kDestruction, RE::ActorValue::kRestoration
        }) {
            int key = static_cast<int>(av);
            if (!avMap.count(key)) {
                avMap[key] = actor->GetActorValueMax(av);
            }
        }
    }

    RE::TESGlobal* GetGlobalForActorValue(RE::ActorValue av) {

        switch (av) {
        case RE::ActorValue::kOneHanded:   return oneHanded;
        case RE::ActorValue::kArchery:     return archery;
        case RE::ActorValue::kSneak:       return sneak;
        case RE::ActorValue::kLightArmor:  return lightArmor;
        case RE::ActorValue::kDestruction: return destruction;
        case RE::ActorValue::kRestoration: return restoration;
        default: return nullptr;
        }
    }

    void displayVital(RE::ActorValue actorValue, const std::string& icon, ImGuiMCP::ImVec4 color) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, color);
        ImGuiMCP::Text("%s", icon.c_str());
        ImGuiMCP::PopStyleColor();
        ImGuiMCP::SameLine();
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
        if (!tilaelActor) return;
        ImGuiMCP::Text("%.0f", tilaelActor->GetActorValueMax(actorValue));
        ImGuiMCP::PopStyleColor();
    }
 
    bool getTilaelActor() {
        if (!tilaelQuest) {
            logger::warn("no tilael quest cannot open menu");
            return false;
        } 

        auto tilaelAliasID = tilaelQuest->aliases[0]->aliasID;

        if (!tilaelAliasID) {
            logger::warn("Tilael could not be captured from her quest alias, her in game menu will not work this instance");
            return false;
        }

        tilaelRefHandle = tilaelQuest->GetAliasedRef(tilaelAliasID);

        if (!tilaelRefHandle) {
            logger::warn("Tilael could not be captured from her quest alias, her in game menu will not work this instance");
            return false;
        }

        tilaelActor = tilaelRefHandle.get()->As<RE::Actor>();

        if (!tilaelActor) {
            logger::warn("Tilael could not be cast as actor, menu wont work");
            return false;
        }

        return true;
    }


    void getPerksForNPC(RE::Actor* actor){

        if (!actor) return; 

      auto actorBase = actor->GetActorBase(); 
      if (!actorBase) return;
 
      int numPerks = actorBase->perkCount; 

      for (int i = 0; i < numPerks; i++) {
          RE::BGSPerk* perk = actorBase->perks[i].perk; 

          if (!perk) continue; 

          std::string name(perk->fullName.c_str());

          RE::BSString desc;
          perk->GetDescription(desc, perk);

          if (desc.c_str() && desc.c_str()[0] != '\0')
          {
              perks[name] = std::string(desc.c_str());
              logger::info("perk {} and its descrip {} ", name, desc);
          }
      }
    }

    void getSpellsForNPC(RE::Actor* actor) {

       if (!actor) return;

       auto actorBase = actor->GetActorBase();

       if (!actorBase) return;

       int numberOfAddedSpells = actor->GetActorRuntimeData().addedSpells.size();

       // added spells
       for (int i = 0; i < numberOfAddedSpells; i++) {
           RE::SpellItem* spell = actor->GetActorRuntimeData().addedSpells[i];

           if (!spell) continue;

               std::string spellName = spell->fullName.c_str();

               RE::BSString desc; 
               spell->GetDescription(desc, spell); 

               spells[spellName] = std::string(desc.c_str());
               logger::info("spell {} and its descrip {} ", spellName, desc);
       }

       if (actorBase->actorEffects)
       {
           //base spells
           int numberOfBaseSpells = actorBase->actorEffects->numSpells;

           for (int i = 0; i < numberOfBaseSpells; i++) {

               RE::SpellItem* spell = actorBase->actorEffects->spells[i];

               if (spell)
               {
                   std::string spellName = spell->fullName.c_str();

                   RE::BSString desc;

                   spell->GetDescription(desc, spell);

                   spells[spellName] = std::string(desc.c_str());
                   logger::info("spell {} and its descrip {} ", spellName, desc);

               }
           }
       }
    }
    

    // communitcate to papyrus through globals 
     void ResetSkillGlobals() {

         if (oneHanded)      oneHanded->value = 0.0f;
         if (archery)        archery->value = 0.0f;
         if (sneak)          sneak->value = 0.0f;
         if (lightArmor)     lightArmor->value = 0.0f;
         if (destruction)    destruction->value = 0.0f;
         if (restoration)    restoration->value = 0.0f;
     }

     // if globakls are zerored then 
     bool checkIfGlobalsZeroed() {

         const RE::TESGlobal* globals[] = {
             oneHanded,
             archery,
             sneak,
             lightArmor,
             destruction,
             restoration
         };

         for (auto g : globals) {
             if (g && g->value != 0.0f) {
                 logger::info("global {} = {}", g->GetName(), g->value);
                 return false;  
             }
         }

         return true;  
     }

} 

// from my other mmod kept incase I want to make a ini file

/*void saveSettingsToIni() {
      logger::info("Saving ReLight.ini...");

      const std::string path = "Data\\SKSE\\Plugins\\ReLight.ini";
      std::ofstream outFile(path, std::ios::trunc);

      if (!outFile.is_open()) {
          logger::error("Failed to open {} for writing!", path);
          return;
      }

      outFile << "; ReLight INI\n";
      outFile << "; Logging Level (0: critical, 1: warnings/errors, 2: info)\n";
      outFile << "loggingLevel=" << loggingLevel << "\n\n";

      outFile << "; disable light references for carryable torches(default = true)\n";
      outFile << "disableTorchLights=" << (disableTorchLights ? "true" : "false") << "\n\n";

      outFile << "; remove fake glow orbs (default = true)\n";
      outFile << "removeFakeGlowOrbs=" << (removeFakeGlowOrbs ? "true" : "false") << "\n\n";

      outFile << "; add esps by name to undisable their lights (usually not needed)\n";
      outFile << "whitelist=";
      for (size_t i = 0; i < whitelist.size(); i++) {
          outFile << whitelist[i];
          if (i + 1 < whitelist.size()) outFile << ",";
      }
      outFile << "\n\n";

      outFile << "; exclude specific nodes\n";
      for (auto& node : exclusionList)
          outFile << node << "\n";

      outFile << "\n; exclude partial nodes\n";
      for (auto& node : exclusionListPartialMatch)
          outFile << node << "\n";

      outFile << "\n; priority list (higher = first match. Usefull for candlechandelier ect to get correct lighting)\n";
      for (auto& node : priorityList)
          outFile << node << "\n";

      outFile.close();
      logger::info("ReLight.ini saved successfully!");

  }*/
