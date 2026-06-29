#pragma once

#include <cstdint>
#include "GLOBALS.h"

namespace Boron {
	namespace Enums {
		enum class InstanceType : uint8_t {
			None = 0,
			Object,
			Instance,
			World
		};
	}
    
	namespace Editor {
		enum class ThemeType : uint8_t {
			None = 0,
			Dark,
			Light
		};
	}
    enum class Keys : uint32_t {
        None = 0,

        // Numbers
        One = GLFW_KEY_1,
        Two = GLFW_KEY_2,
        Three = GLFW_KEY_3,
        Four = GLFW_KEY_4,
        Five = GLFW_KEY_5,
        Six = GLFW_KEY_6,
        Seven = GLFW_KEY_7,
        Eight = GLFW_KEY_8,
        Nine = GLFW_KEY_9,
        Zero = GLFW_KEY_0,

        // Letters
        Q = GLFW_KEY_Q,
        W = GLFW_KEY_W,
        E = GLFW_KEY_E,
        R = GLFW_KEY_R,
        T = GLFW_KEY_T,
        Y = GLFW_KEY_Y,
        U = GLFW_KEY_U,
        I = GLFW_KEY_I,
        O = GLFW_KEY_O,
        P = GLFW_KEY_P,

        A = GLFW_KEY_A,
        S = GLFW_KEY_S,
        D = GLFW_KEY_D,
        F = GLFW_KEY_F,
        G = GLFW_KEY_G,
        H = GLFW_KEY_H,
        J = GLFW_KEY_J,
        K = GLFW_KEY_K,
        L = GLFW_KEY_L,

        Z = GLFW_KEY_Z,
        X = GLFW_KEY_X,
        C = GLFW_KEY_C,
        V = GLFW_KEY_V,
        B = GLFW_KEY_B,
        N = GLFW_KEY_N,
        M = GLFW_KEY_M,

        // Modifiers
        LeftShift = GLFW_KEY_LEFT_SHIFT,
        RightShift = GLFW_KEY_RIGHT_SHIFT,
        LeftCtrl = GLFW_KEY_LEFT_CONTROL,
        RightCtrl = GLFW_KEY_RIGHT_CONTROL,
        LeftAlt = GLFW_KEY_LEFT_ALT,
        RightAlt = GLFW_KEY_RIGHT_ALT,
        LeftSuper = GLFW_KEY_LEFT_SUPER,
        RightSuper = GLFW_KEY_RIGHT_SUPER,

        Space = GLFW_KEY_SPACE,
        Enter = GLFW_KEY_ENTER,
        Escape = GLFW_KEY_ESCAPE,
        Tab = GLFW_KEY_TAB,
        Backspace = GLFW_KEY_BACKSPACE,
        Insert = GLFW_KEY_INSERT,
        Delete = GLFW_KEY_DELETE,

        // Arrows
        Up = GLFW_KEY_UP,
        Down = GLFW_KEY_DOWN,
        Left = GLFW_KEY_LEFT,
        Right = GLFW_KEY_RIGHT,

        // Function keys
        F1 = GLFW_KEY_F1,
        F2 = GLFW_KEY_F2,
        F3 = GLFW_KEY_F3,
        F4 = GLFW_KEY_F4,
        F5 = GLFW_KEY_F5,
        F6 = GLFW_KEY_F6,
        F7 = GLFW_KEY_F7,
        F8 = GLFW_KEY_F8,
        F9 = GLFW_KEY_F9,
        F10 = GLFW_KEY_F10,
        F11 = GLFW_KEY_F11,
        F12 = GLFW_KEY_F12
    };
}