#pragma once

#include "logger.hpp"
#include <filesystem>
#include "Menu.h"

namespace fs = std::filesystem;

struct FollowerData {
	
	std::vector<RE::FormID> perks; 

    int level = 0; 

    int skillPoints = 0; 

    std::string configPath{};

     void print() {
         logger::info("Tilael Lvl: {} skill points: {}", level, skillPoints);
    }
   
};


extern FollowerData tilaelData;

extern RE::TESQuest* tilaelQuest;

extern RE::TESGlobal* oneHanded;
extern RE::TESGlobal* twoHanded;
extern RE::TESGlobal* archery;
extern RE::TESGlobal* block;
extern RE::TESGlobal* lightArmor;
extern RE::TESGlobal* heavyArmor;
extern RE::TESGlobal* destruction;
extern RE::TESGlobal* restoration;

inline std::string ToUTF8(const fs::path& p) {
    auto u8 = p.u8string();
    return std::string(reinterpret_cast<const char*>(u8.c_str()));
}

inline std::string GetConfigDir() {
    const auto root = std::filesystem::path(REL::Module::get().filename()).parent_path();
    return (root / "Data" / "SKSE" / "Plugins" / PRODUCT_NAME / "").string();
}

inline std::vector<std::string> GetConfigPaths() {
    const fs::path dir = GetConfigDir();

    std::vector<std::string> paths;

    std::error_code ec;
    if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec)) {
        logger::critical("Config directory {} does not exist.", ToUTF8(dir));
        return paths;
    }

    auto it = fs::recursive_directory_iterator(dir, fs::directory_options::skip_permission_denied, ec);
    fs::recursive_directory_iterator end;

    if (ec) {
        logger::critical("Cannot iterate over {}: {}", ToUTF8(dir), ec.message());
    }

    while (it != end) {
        const auto& p = it->path();

        if (fs::is_regular_file(p, ec) && p.extension() == ".json") {
            logger::info("Found config file: {}", ToUTF8(p));
            paths.push_back(ToUTF8(p));
        }

        ec.clear();

        it.increment(ec);

        if (ec) {
            logger::critical("Skipping path under {}: {}", ToUTF8(dir), ec.message());
            ec.clear();
        }
    }

    return paths;
}




bool loadConfiguration(const FollowerData& followerData, const std::string& configPath);

bool saveConfiguration(const FollowerData& followerData, const std::string& configPath);

void parseTemplates();

inline void initialize() {
    auto dataHandler = RE::TESDataHandler::GetSingleton();  // Retrieve it safely here
    if (!dataHandler) {
        logger::warn("TESDataHandler not found.");
        return;
    }

    // Quest
    UI::tilaelQuest = dataHandler->LookupForm<RE::TESQuest>(0x006, "Tilael.esp");
    if (!UI::tilaelQuest) {
        logger::warn("tilael quest not found.");
    }
    
    // Silent dialogue topic
    UI::TFQuest_TFQuestSilentLine_00005A96_1 = dataHandler->LookupForm<RE::TESTopic>(0x700, "Tilael.esp");
    if (!UI::TFQuest_TFQuestSilentLine_00005A96_1) {
        logger::warn("Silent dialogue topic FE005A96 not found.");
    }

    // Globals using only the last 3 digits
     oneHanded = dataHandler->LookupForm<RE::TESGlobal>(0x901, "Tilael.esp");
     twoHanded = dataHandler->LookupForm<RE::TESGlobal>(0x902, "Tilael.esp");
     archery = dataHandler->LookupForm<RE::TESGlobal>(0x903, "Tilael.esp");
     block = dataHandler->LookupForm<RE::TESGlobal>(0x904, "Tilael.esp");
     lightArmor = dataHandler->LookupForm<RE::TESGlobal>(0x905, "Tilael.esp");
     heavyArmor = dataHandler->LookupForm<RE::TESGlobal>(0x906, "Tilael.esp");
     destruction = dataHandler->LookupForm<RE::TESGlobal>(0x907, "Tilael.esp");
     restoration = dataHandler->LookupForm<RE::TESGlobal>(0x908, "Tilael.esp");

    // Optional: warn if any globals are missing
    if (!oneHanded) logger::warn("OneHanded global not found.");
    if (!twoHanded) logger::warn("TwoHanded global not found.");
    if (!archery) logger::warn("Archery global not found.");
    if (!block) logger::warn("Block global not found.");
    if (!lightArmor) logger::warn("LightArmor global not found.");
    if (!heavyArmor) logger::warn("HeavyArmor global not found.");
    if (!destruction) logger::warn("Destruction global not found.");
    if (!restoration) logger::warn("Restoration global not found.");
}
