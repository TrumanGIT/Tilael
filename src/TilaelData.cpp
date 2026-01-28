#include "nlohmann/json.hpp"
#include "TilaelData.h"


using json = nlohmann::json;

//RE::TESNPC* tilaelBaseActor;

FollowerData tilaelData;

bool loadConfiguration(FollowerData& config, const std::string& configPath) {
    try {
        std::ifstream configFile(configPath);
        if (!configFile.is_open()) {
            logger::error("Failed to open config file: {}", ToUTF8(configPath));
            return false;
        }

        std::string raw((std::istreambuf_iterator<char>(configFile)), std::istreambuf_iterator<char>());

        json data = json::parse(raw, nullptr, true, true);

        if (data.contains("level")) {
            config.level = data["level"].get<int>();
        }

        if (data.contains("skillPoints")) {
            config.skillPoints = data["skillPoints"].get<int>();
        }

        return true;
    }
    catch (const json::exception& e) {
        logger::error("cannot read JSON file due to {}", std::string(e.what()));
        return false;
    }
    catch (const std::exception& e) {
        logger::error("cannot read JSON file due to {}", std::string(e.what()));
        return false;
    }
}


bool saveConfiguration(const FollowerData& config, const std::string& configPath) {
    try {
        json data;

        data["level"] = config.level;

        data["skillPoints"] = config.skillPoints; 

        std::ofstream out(configPath, std::ios::trunc);
        if (!out.is_open()) {
            logger::error("Failed to open config file for writing: {}", configPath);
            return false;
        }

        out << data.dump(4);

        logger::info("Successfully saved follower data to template at {}", configPath);

        return true;
    }
    catch (const std::exception& e) {
        logger::error("Failed to write config {}: {}", configPath, e.what());
        return false;
    }
}

void parseTemplates() {
    logger::info("Parsing light templates..");
    std::vector<std::string> paths = GetConfigPaths();
    static uint64_t nextID = 1;

    for (const auto& p : paths) {
        logger::info(" reading.. {}", p);

        std::ifstream configFile(p);
        if (!configFile.is_open()) {
            logger::error("Failed to open config file: {}", p);
            continue;
        }

        std::string raw((std::istreambuf_iterator<char>(configFile)), std::istreambuf_iterator<char>());
        json data = json::parse(raw, nullptr, true, true);

            FollowerData cfg;
            loadConfiguration(cfg, p);
            cfg.configPath = p;
            cfg.print(); 

            tilaelData = cfg; 
    }
}
