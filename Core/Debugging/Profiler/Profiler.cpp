#include "Profiler.h"
#include <iomanip>
#include <sstream>

void Profiler::AddFPS([[maybe_unused]] float FPS)
{
#ifdef PROFILER
    static bool firstTime = true;

    if (firstTime)
    {
        firstTime = false;
        return;
    }

    TotalFPS += FPS;
    TotalFrames++;
#endif
}

void Profiler::PrintInformation()
{
    #ifdef PROFILER
        if (TotalFrames == 0) {
            CreateError("No frames recorded!");
            return;
        }

        float AverageFPS = TotalFPS / TotalFrames;

        std::stringstream ss;
        ss << "Avarage FPS: " << std::fixed << std::setprecision(2) << AverageFPS;

        CreateSuccess("ProfilerInfo ");
        ProfilerInformation(ss.str());
    #endif
}