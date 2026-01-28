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
    auto dataHandler = RE::TESDataHandler::GetSingleton();  //  Retrieve it safely here
    if (dataHandler) {
      //  tilaelBaseActor = dataHandler->LookupForm<RE::TESNPC>(0x007, "Tilael.esp");

      //  if (!tilaelBaseActor) {
     //       logger::warn("baseActor Tilael' not found cant update stats");
     //   }

        UI::tilaelQuest = dataHandler->LookupForm<RE::TESQuest>(0x006, "Tilael.esp");

        if (!UI::tilaelQuest) {
            logger::warn("tilael quest not found' not found.");
        }
    }

}