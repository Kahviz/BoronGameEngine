#pragma once
#include "BoronMathLibrary.h"
#include "GLOBALS.h"
#include "ErrorHandling/ErrorMessage.h"
#include "Window/Window.h"

class Mouse {
public:
	Mouse() {
		CreateInfo("Mouse created");
	}

	static void updateMouse(Window* window);
	static BML::Vec2 getMousePos();
	static BML::Vec2 GetDelta();
	static float GetDeltaX();
	static float GetDeltaY();
private:
	static BML::Vec2 s_lastMousePos;
	static BML::Vec2 s_mousePos;
	static bool s_firstmouse;
};