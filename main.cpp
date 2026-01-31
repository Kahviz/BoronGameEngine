#include <iostream>
#include "Runtime/Engine.h"

int main() {
    try {
        Engine engine;
        engine.EngineRun();
    }
    catch (const std::exception& e) {
        std::cerr << "Virhe: " << e.what() << std::endl;
        return -5;
    }

    std::cout << "This is the end..." << std::endl;
    return 0;
}