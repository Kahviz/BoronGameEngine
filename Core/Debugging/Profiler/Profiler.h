#pragma once
#include "Logger/Logger.h"
#include "GLOBALS.h"

class Profiler {
public:
    Profiler() {
        m_totalFrames = 0.0f;
        m_totalFPS = 0.0f;
        CreateSuccess("Profiler created!");
    }

    void AddFPS(float p_fps);
    void PrintInformation();

private:
    float m_totalFrames;
    float m_totalFPS;
};