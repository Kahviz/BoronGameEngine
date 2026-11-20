#include "Console.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

void Console::Write(std::string p_text, Boron::Enums::ConsoleLineType p_type) {
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
	consoleLine.text = p_text;
	consoleLine.timestamp = stream.str();
	consoleLine.type = p_type;

	m_consoleLog.push_back(consoleLine);

	if (!m_writed) {
		m_writed = true;
	}
}

void Console::setMaxConsoleSize(const int& p_maxConsoleSize) {
	m_maxConsoleSize = p_maxConsoleSize;
}

void Console::Update() {
	if (m_writed) {
		m_writed = false;
	}
}

const std::vector<ConsoleLine>& Console::getConsoleLog() {
	return m_consoleLog;
}

bool Console::isWrited() {
	return m_writed;
}
