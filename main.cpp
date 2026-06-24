#include <iostream>
#include "Runtime/Engine.h"
#include "SetupPaths/MakeFiles.h"
#include <Debugging/Profiler/Profiler.h>
#include <thread>
#include "ErrorHandling/ErrorMessage.h"
#include <format>
#include "BoronMathLibrary.h"
#include "GLOBALS.h"
int main() {
    CreateInfo("BoronEngine started!");

    #if VULKAN == 1
        #if DIRECTX11 == 1
            CreateError("Cannot have multiple graphicsbackends at once");
            return -1;
        #endif
    #endif

#if VULKAN == 0
    #if DIRECTX11 == 0
        CreateError("You have to have one graphics backend");
        return -1;
    #endif
#endif

    MakeFiles mf;
    mf.MakeAPPDATAFolders();

    const int WaitTime = 1;

    #if INEDITOR == 0
        #ifdef NDEBUG
            std::cout << "\033[1;33m[INFO]\033[0m You are in release mode and InEditor Flag is 0." << std::endl;
            std::cout << "\033[1;33m[INFO]\033[0m If you want Editor Gui, set InEditor = 1 in GLOBALS." << std::endl;
            std::cout << "\033[1;33m[INFO]\033[0m This makes it harder to make cheats!" << std::endl;
        #endif
    #endif

    try {
        Engine engine;
        engine.EngineRun();
    }
    catch (const std::exception& e) {
        std::cerr << "\033[1;31m[ERROR]\033[0m " << e.what() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return -5;
    }
    catch (...) {
        std::cerr << "\033[1;31m[ERROR]\033[0m Unknown exception occurred!" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        return -6;
    }

    std::string WaitTimeString = std::format("Waiting For {} second(s) before closing!", WaitTime);

    CreateSuccess(WaitTimeString);
    std::this_thread::sleep_for(std::chrono::seconds(WaitTime));

    CreateSuccess("This is the end...");

    return 0;
}
