#include <iostream>
#include "Runtime/Engine.h"
#include "SetupPaths/MakeFiles.h"
#include <Debugging/Profiler/Profiler.h>
#include <thread>
#include "ErrorHandling/ErrorMessage.h"
#include <format>
#include "UntilitedMath.h"

int main() {
    MakeFiles mf;

    int WaitTime = 2;
    mf.MakeAPPDATAFolders();

#if INEDITOR == 0
    #ifdef NDEBUG
        std::cout << "You are in release mode and InEditor Flag is 0 IF you want info and Editor Gui goto GLOBALS and InEditor = 1 and Go In Debug Option... I made this so it's harder to make cheats!" << std::endl;
    #endif
#endif

    try {
        Engine engine;
        engine.EngineRun();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return -5;
    }
    std::string WaitTimeString = std::format("Waiting For {}", WaitTime," Secund before closing!");

    MakeASuccess(WaitTimeString);
    std::this_thread::sleep_for(std::chrono::seconds(WaitTime));
    MakeASuccess("This is the end...");
    return 0;
}