#include "Console.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

void Console::write(std::string text, Boron::Enums::ConsoleLineType type)
{
	if (m_consoleLog.size() >= m_maxConsoleSize) {
		m_consoleLog.erase(m_consoleLog.begin());
	}
	std::time_t now = std::time(nullptr);
	std::tm localTime = *std::localtime(&now);

	std::ostringstream stream;

	stream << "["
		<< std::setfill('0') << std::setw(2) << localTime.tm_hour << ":"
		<< std::setw(2) << localTime.tm_min << ":"
		<< std::setw(2) << localTime.tm_sec
		<< "]: ";

	ConsoleLine consoleLine;
	consoleLine.text = text;
	consoleLine.timestamp = stream.str();
	consoleLine.type = type;

	m_consoleLog.push_back(consoleLine);

	writed = true;
}

void Console::setMaxConsoleSize(const int& maxConsoleSize)
{
	m_maxConsoleSize = maxConsoleSize;
}

void Console::update()
{
	if (writed) {
		writed = false;
	}
}
