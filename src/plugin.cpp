
#include "plugin.hpp"
#include "logger.hpp"
#include "menu.h"
#include "TilaelData.h"

inline void PapyrusSay(RE::TESObjectREFR* target, RE::TESTopic* toSay, RE::Character* toSpeakAs, bool playerHead) {
    if (target && toSay) {
        using func_t = void (*)(std::int64_t, std::int64_t, RE::TESObjectREFR*, RE::TESTopic*, RE::Character*, bool);
        static REL::Relocation<func_t> func{ REL::VariantID(
            55689, 56220, 0x9D1470) };  // takes SE, AE, then VR offset b/c VR might no exist they are filled manually
        return func(NULL, NULL, target, toSay, toSpeakAs, playerHead);
    }
}

struct OurEventSink : public RE::BSTEventSink<RE::LevelIncrease::Event> {
    OurEventSink() = default;
    OurEventSink(const OurEventSink&&) = delete;
    OurEventSink& operator=(const OurEventSink&) = delete;
    OurEventSink& operator=(OurEventSink&&) = delete;
public:
    static OurEventSink* GetSingleton() {
        static OurEventSink singleton;
        return &singleton;
    }

    RE::BSEventNotifyControl ProcessEvent(const RE::LevelIncrease::Event* event,
        RE::BSTEventSource<RE::LevelIncrease::Event>*) override {
     

        tilaelData.skillPoints += 5; 
        tilaelData.level++; 

        saveConfiguration(tilaelData,"data\\SKSE\\Plugins\\Tilael\\Tilael.json" ); 

        tilaelData.print(); 

        logger::debug("player level event called tilael skill points = {}", tilaelData.skillPoints);
        return RE::BSEventNotifyControl::kContinue;
    }
};

static void MessageHandler(SKSE::MessagingInterface::Message* msg) {
    switch (msg->type) {

    case SKSE::MessagingInterface::kPostLoadGame:
    {
               break;
    }
    case SKSE::MessagingInterface::kNewGame:
    {
    

        break;
    }
    case SKSE::MessagingInterface::kDataLoaded:
    {

         initialize();

         parseTemplates();

        RE::LevelIncrease::GetEventSource()->AddEventSink(OurEventSink::GetSingleton());
        
        break;
    }
    default:
        break;

    }
}


SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);
    setupLog(spdlog::level::info);
    logger::info("Tilael SKSE Plugin is Loaded");
    SKSE::GetMessagingInterface()->RegisterListener(MessageHandler);
    UI::Register();

    return true;
}