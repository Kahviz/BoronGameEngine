#pragma once
#include <vector>
#include <string>

class Console {
public:
	void write(std::string line);
	const std::vector<std::string>& getConsoleLog() { return m_consoleLog; }
private:
	std::vector<std::string> m_consoleLog;
};