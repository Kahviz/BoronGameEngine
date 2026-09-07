#include "Profiler.h"
#include <iomanip>
#include <sstream>

void Profiler::AddFPS([[maybe_unused]] float p_fps) {
    #ifdef PROFILER
        static bool firstTime = true;

        if (firstTime)
        {
            firstTime = false;
            return;
        }

        m_totalFPS += p_fps;
        m_totalFrames++;
    #endif
}

void Profiler::PrintInformation()
{
    #ifdef PROFILER
        if (m_totalFrames == 0) {
            CreateError("No frames recorded!");
            return;
        }

        float AverageFPS = m_totalFPS / m_totalFrames;

        std::stringstream ss;
        ss << "Avarage FPS: " << std::fixed << std::setprecision(2) << AverageFPS;

        CreateSuccess("ProfilerInfo: ");
        ProfilerInformation(ss.str());
    #endif
}