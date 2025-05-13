#pragma once
#include <string>
#include <mutex>


class App
{
public:
	App();
	std::pair<int, int> UpdatePos(const std::string& command);
private:
	int x = 100;
	int y = 100;
};