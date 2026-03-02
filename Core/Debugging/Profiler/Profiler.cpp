#include "Profiler.h"
#include <iomanip>
#include <sstream>

void Profiler::AddFPS(float FPS) {
    #ifdef PROFILER
        static bool firstTime = true;

        if (!firstTime) {
            TotalFPS += FPS;
            TotalFrames++;
        }

        firstTime = false;
    #endif
}

void Profiler::PrintInformation()
{
    #ifdef PROFILER
        if (TotalFrames == 0) {
            MakeAError("No frames recorded!");
            return;
        }

        float AverageFPS = TotalFPS / TotalFrames;

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << AverageFPS;

        MakeASuccess("ProfilerInfo");
        ProfilerInformation(ss.str());

        std::cout << "DEBUG - TotalFPS: " << TotalFPS
            << ", TotalFrames: " << TotalFrames
            << ", Avg: " << AverageFPS << std::endl;
    #endif
}