
#include "plugin.hpp"
#include "logger.hpp"
#include "menu.h"
#include "TilaelData.h"


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