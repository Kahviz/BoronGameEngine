#include "Console.h"

void Console::write(std::string line)
{
	m_consoleLog.push_back(line);
}
