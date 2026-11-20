#pragma once

#include <vector>
#include <string>
#include "Enums.h"

struct ConsoleLine {
	Boron::Enums::ConsoleLineType type = Boron::Enums::ConsoleLineType::Info;
	std::string text = "NULL";
	std::string timestamp = "NULL";
};

class Console {
public:
	void Write(std::string text, Boron::Enums::ConsoleLineType p_type);
	void Update();
	void setMaxConsoleSize(const int& p_maxConsoleSize);

	const std::vector<ConsoleLine>& getConsoleLog();
	bool isWrited();
private:
	bool m_writed = false;
	int m_maxConsoleSize = 100;
	std::vector<ConsoleLine> m_consoleLog;
};