#pragma once
#include "SDL3/SDL.h"
#include "SDL3/SDL_timer.h"

class ControllerEffects {

public:
    ControllerEffects();

    ~ControllerEffects();

    void RumbleEffect(Uint32 duration, int strength);
private:

};