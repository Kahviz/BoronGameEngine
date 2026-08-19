#pragma once

#include <vector>
#include <string>
#include "Enums.h"

struct ConsoleLine
{
	Boron::Enums::ConsoleLineType type = Boron::Enums::ConsoleLineType::Info;
	std::string text = "NULL";
	std::string timestamp = "NULL";
};

class Console {
public:
	void write(std::string text, Boron::Enums::ConsoleLineType type);
	void setMaxConsoleSize(const int& maxConsoleSize);
	void update();

	const std::vector<ConsoleLine>& getConsoleLog() { return m_consoleLog; }
	bool isWrited() { return writed; }
private:
	bool writed = false;
	int m_maxConsoleSize = 100;
	std::vector<ConsoleLine> m_consoleLog;
};