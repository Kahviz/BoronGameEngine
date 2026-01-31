#include "Controller.h"
#include <iostream>

Uint16 PercentToRumble(int percent) {
    int clamped_percent = percent;
    if (clamped_percent < 0) clamped_percent = 0;
    if (clamped_percent > 100) clamped_percent = 100;

    return static_cast<Uint16>((0xFFFF * clamped_percent) / 100);
}

void SetLEDColor(Uint8 red, Uint8 green, Uint8 blue) {
    SDL_Joystick* joystick = SDL_OpenJoystick(0);

    if (!joystick) return;

    if (SDL_SetJoystickLED(joystick, red, green, blue) != 0) {
        SDL_Log("Failed to set LED: %s", SDL_GetError());
    }
}

ControllerEffects::ControllerEffects() {
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5, "1");
    SDL_Init(SDL_INIT_JOYSTICK);
    SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS5_PLAYER_LED, "1");

    if (!SDL_Init(SDL_INIT_GAMEPAD)) {
        std::cout << "SDL Error: " << SDL_GetError() << std::endl;
    }
}

ControllerEffects::~ControllerEffects() {
    SDL_Quit();
}

// Rumble the controller for a specified duration (millisecunds) and strength (0-100)
void ControllerEffects::RumbleEffect(Uint32 duration, int strength)
{
    bool CanLoop = true;
    for (int i = -1; i < 100; ++i) {
        if (CanLoop) {
            SDL_Gamepad* pad = SDL_OpenGamepad(i);

            if (pad) {
                int rumble_strength = PercentToRumble(strength);

                SDL_RumbleGamepad(pad, rumble_strength, rumble_strength, duration);
                SDL_Delay(duration + 50);
                SDL_CloseGamepad(pad);

                break;
            }
        }
        else {
            break;
        }
    }
}
