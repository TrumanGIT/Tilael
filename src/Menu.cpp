
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
                tilaelData.skillPoints = tempSkillPoints;
                tilaelData.level++;
                saveConfiguration(tilaelData, tilaelData.configPath);
                skillPointsSelected = false;
            }
            if (ImGuiMCP::IsItemHovered())
                ImGuiMCP::SetTooltip("Spend Skill Points");
        }

        ImGuiMCP::Separator();

        if (initialized == false){

        if (!getTilaelActor()) return;

        getPerksForNPC(tilaelActor);

        tempSkillPoints = tilaelData.skillPoints;

        ResetSkillGlobals();

        fillTempAVMap(tilaelActor, tempAVs); 

        fillTempAVMap(tilaelActor, intialAVs);

        }

        initialized = true;

        ImGuiMCP::Separator();
        ImGuiMCP::Spacing();

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

        // Push cursor to the right side of the window
        float right = ImGuiMCP::GetWindowContentRegionMax().x;
        ImGuiMCP::SetCursorPosX(right - 350.0f); // tweak width as needed

        ImGuiMCP::TextColored(
            ImGuiMCP::ImVec4{ 0.5f, 0.7f, 1.f, 1.f },
            "SkillPoints: %d",
            tempSkillPoints
        );
  
        ImGuiMCP::Columns(2, nullptr, false);

        auto SkillRow = [&](const char* name, RE::ActorValue av)
            {
                ImGuiMCP::Text("%s", name);
                ImGuiMCP::NextColumn();

                const auto& avAsInt = static_cast<int>(av);
            
                ImGuiMCP::Text("%.0f",tempAVs[avAsInt]);

                if (tempSkillPoints > 0) 
                {
                    ImGuiMCP::SameLine();

                    ImGuiMCP::PushID(avAsInt);

                    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, { 0.5f, 0.7f, 1.f, 1.f });
                    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonHovered, ImGuiMCP::ImVec4{ 0.6f, 0.8f, 1.f, 1.f });
                    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonActive, ImGuiMCP::ImVec4{ 0.4f, 0.6f, 0.9f, 1.f });

                    if (ImGuiMCP::SmallButton("+"))
                    {
                        if (auto global = GetGlobalForActorValue(av)) {
                            global->value += 1.0f;
                            logger::info(" added global value: {}", global->value);
                            tempSkillPoints -= 1;
                            tempAVs[avAsInt] += 1;
                            skillPointsSelected = true;
                        }
                    }
                }

                ImGuiMCP::SameLine();

                ImGuiMCP::PopStyleColor(3);
                ImGuiMCP::PopID();

                if (skillPointsSelected == true) {
                    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button,
                        ImGuiMCP::ImVec4{ 0.85f, 0.2f, 0.2f, 1.f });

                    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonHovered,
                        ImGuiMCP::ImVec4{ 0.95f, 0.3f, 0.3f, 1.f });

                    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonActive,
                        ImGuiMCP::ImVec4{ 0.75f, 0.15f, 0.15f, 1.f });

                    if (ImGuiMCP::SmallButton("- "))
                    {
                        if (auto global = GetGlobalForActorValue(av)) {

                            logger::info("temp av = {} Initial AV = {}", tempAVs[avAsInt], intialAVs[avAsInt]);

                            // Only subtract if above the initial value
                            if (tempAVs[avAsInt] > intialAVs[avAsInt]) {
                                global->value -= 1.0f;
                                tempSkillPoints += 1;
                                tempAVs[avAsInt] -= 1;
                                if (checkIfGlobalsZeroed()) skillPointsSelected = false;
                            }

                            // else: do nothing, but still pop afterwards
                        }
                    }
                }


                ImGuiMCP::PopStyleColor(3);
                ImGuiMCP::PopID();

                ImGuiMCP::NextColumn();
            };
       
        SkillRow("One-Handed", RE::ActorValue::kOneHanded);
        SkillRow("Archery", RE::ActorValue::kArchery);
        SkillRow("Light-Armor", RE::ActorValue::kLightArmor);
        SkillRow("Sneak", RE::ActorValue::kSneak);
        SkillRow("Destruction", RE::ActorValue::kDestruction);
        SkillRow("Restoration", RE::ActorValue::kRestoration);
        ImGuiMCP::Columns(1);

        ImGuiMCP::Separator();
        ImGuiMCP::Spacing();

        ImGuiMCP::TextColored(ImGuiMCP::ImVec4{ 0.5f, 0.7f, 1.f, 1.f }, "Perks");

        ImGuiMCP::BeginChild("PerkList", ImGuiMCP::ImVec2(0, 120), true, ImGuiMCP::ImGuiWindowFlags_AlwaysVerticalScrollbar);
        ImGuiMCP::Indent(10); // slight indent for bullets

        for (const auto& [perk, descrip] : perks) {
            ImGuiMCP::TextWrapped("%s", perk.c_str());

            if (ImGuiMCP::IsItemHovered()) {
                ImGuiMCP::SetTooltip("%s", descrip.c_str());
            }
        }

        ImGuiMCP::Unindent(10);
        ImGuiMCP::EndChild();
            ImGuiMCP::End();
    }

    //  show a copy of the stats to simulate level increasing when pusshing the button
    // only actually changed when clicking level up button.
     void fillTempAVMap(RE::Actor* actor, std::unordered_map<int, float>& avMap) {
        for (RE::ActorValue av : {
            RE::ActorValue::kOneHanded, RE::ActorValue::kTwoHanded, RE::ActorValue::kArchery,
                RE::ActorValue::kBlock, RE::ActorValue::kLightArmor, RE::ActorValue::kHeavyArmor,
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
        case RE::ActorValue::kTwoHanded:   return twoHanded;
        case RE::ActorValue::kArchery:     return archery;
        case RE::ActorValue::kBlock:       return block;
        case RE::ActorValue::kLightArmor:  return lightArmor;
        case RE::ActorValue::kHeavyArmor:  return heavyArmor;
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

     void ResetSkillGlobals() {

         if (oneHanded)      oneHanded->value = 0.0f;
         if (twoHanded)      twoHanded->value = 0.0f;
         if (archery)        archery->value = 0.0f;
         if (block)          block->value = 0.0f;
         if (lightArmor)     lightArmor->value = 0.0f;
         if (heavyArmor)     heavyArmor->value = 0.0f;
         if (destruction)    destruction->value = 0.0f;
         if (restoration)    restoration->value = 0.0f;
     }


     bool checkIfGlobalsZeroed() {

         const RE::TESGlobal* globals[] = {
             oneHanded,
             twoHanded,
             archery,
             block,
             lightArmor,
             heavyArmor,
             destruction,
             restoration
         };

         for (auto g : globals) {
             if (g && g->value != 0.0f) {
                 return false;  
             }
         }

         return true;  // All globals were 0
     }

} 


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