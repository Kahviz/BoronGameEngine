#include <iostream>
#include "Runtime/Engine.h"
#include "SetupPaths/MakeFiles.h"
#include "Misc/Profiler/Profiler.h"

int main() {
    MakeFiles mf;

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
        return -5;
    }

    std::cout << "This is the end..." << std::endl;
    return 0;
}