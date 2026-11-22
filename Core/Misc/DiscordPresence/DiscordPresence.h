#pragma once

class DiscordPresence
{
public:
    static bool Init();
    static void Update();
    static void Destroy();
private:
    static bool m_inited;
};