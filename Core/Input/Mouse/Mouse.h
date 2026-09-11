#pragma once
#include "BoronMathLibrary.h"
#include "GLOBALS.h"
#include "Logger/Logger.h"
#include "Window/Window.h"

class Mouse {
public:
	Mouse() {
		CreateInfo("Mouse created");
	}

	static void updateMouse(Window* window);
	static BML::Vec2 getMousePos();
	static float getMousePosX();
	static float getMousePosY();

	static BML::Vec2 getDelta();
	static float getDeltaX();
	static float getDeltaY();
	static bool isLeftClicked();
	static bool isRightClicked();
private:
	static BML::Vec2 m_lastMousePos;
	static BML::Vec2 m_mousePos;
	static bool m_firstmouse;
	static bool m_leftClicked;
	static bool m_rightClicked;
};