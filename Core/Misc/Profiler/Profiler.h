#pragma once
#include "ErrorHandling/ErrorMessage.h"
#include "GLOBALS.h"

class Profiler {
public:
    Profiler() {
        TotalFrames = 0.0f;
        TotalFPS = 0.0f;
        MakeASuccess("Profiler created!");
    }

    void AddFPS(float FPS);
    void PrintInformation();

private:
    float TotalFrames;
    float TotalFPS;
};