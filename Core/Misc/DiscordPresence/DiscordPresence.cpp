#include "DiscordPresence.h"
#include "Logger/Logger.h"

bool DiscordPresence::m_inited = false;

#if BORON_USE_DISCORD
#define DISCORDPP_IMPLEMENTATION
#include <discordpp.h>

#define BORON_DISCORD_APP_ID 1539273049034199060

static discordpp::Client* discordClient = nullptr;
#endif

bool DiscordPresence::Init() {
    #if BORON_USE_DISCORD
        if (m_inited) {
            return true;
        }

        discordClient = new discordpp::Client();

        discordClient->SetApplicationId(BORON_DISCORD_APP_ID);

        m_inited = true;
        CreateInfo("Using discord rich precence");
    #endif
    
    return true;
}

void DiscordPresence::Update() {
    #if BORON_USE_DISCORD
        if (!m_inited || discordClient == nullptr) {
            return;
        }

        discordpp::Activity activity{};

        activity.SetType(discordpp::ActivityTypes::Playing);
        activity.SetDetails("Game-engine");
        activity.SetState("Developing BoronEngine");

        discordClient->UpdateRichPresence(
            activity,
            [](const discordpp::ClientResult& result)
            {
                if (!result.Successful()) {
                    CreateError("Discord presence update failed!");
                }
            }
        );
    #endif
}

void DiscordPresence::Destroy() {
    #if BORON_USE_DISCORD
        if (!m_inited) {
            return;
        }

        delete discordClient;
        discordClient = nullptr;

        m_inited = false;
    #endif
}