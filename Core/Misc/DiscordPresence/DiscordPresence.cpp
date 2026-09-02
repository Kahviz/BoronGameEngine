#include "DiscordPresence.h"
#include "ErrorHandling/ErrorMessage.h"

bool DiscordPresence::initialized = false;

#if BORON_USE_DISCORD

#define DISCORDPP_IMPLEMENTATION
#include <discordpp.h>

#define BORON_DISCORD_APP_ID 1539273049034199060

static discordpp::Client* discordClient = nullptr;
#endif

bool DiscordPresence::Initialize() {
    #if BORON_USE_DISCORD
        if (initialized)
            return true;

        discordClient = new discordpp::Client();

        discordClient->SetApplicationId(BORON_DISCORD_APP_ID);

        initialized = true;
        CreateInfo("Using DC");
    #endif
    
    return true;
}

void DiscordPresence::Update() {
    #if BORON_USE_DISCORD
        if (!initialized || discordClient == nullptr)
            return;

        discordpp::Activity activity{};

        activity.SetType(discordpp::ActivityTypes::Playing);
        activity.SetDetails("Game-engine");
        activity.SetState("Developing BoronEngine");

        discordClient->UpdateRichPresence(
            activity,
            [](const discordpp::ClientResult& result)
            {
                if (!result.Successful())
                {
                    CreateError("Discord presence update failed!");
                }
            }
        );
    #endif
}

void DiscordPresence::Shutdown() {
    #if BORON_USE_DISCORD
        if (!initialized)
            return;

        delete discordClient;
        discordClient = nullptr;

        initialized = false;
    #endif
}