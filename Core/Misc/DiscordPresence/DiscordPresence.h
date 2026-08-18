#pragma once

class DiscordPresence
{
public:
    static bool Initialize();
    static void Update();
    static void Shutdown();

private:
    static bool initialized;
};