#include "DiscordPresence.h"

#define DISCORDPP_IMPLEMENTATION
#include <discordpp.h>

#include "ErrorHandling/ErrorMessage.h"

#define BORON_DISCORD_APP_ID 1539273049034199060

bool DiscordPresence::initialized = false;

static discordpp::Client* discordClient = nullptr;

bool DiscordPresence::Initialize()
{
    if (initialized)
        return true;

    discordClient = new discordpp::Client();

    discordClient->SetApplicationId(BORON_DISCORD_APP_ID);

    initialized = true;
    return true;
}

void DiscordPresence::Update()
{
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
}

void DiscordPresence::Shutdown()
{
    if (!initialized)
        return;

    delete discordClient;
    discordClient = nullptr;

    initialized = false;
}