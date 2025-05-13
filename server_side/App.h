#pragma once
#include <string>
#include <mutex>


class App
{
public:
	App();
	void UpdatePos(const std::string& command);
	std::pair<int, int> ReturnPos();
private:
	int x = 100;
	int y = 100;
//	std::mutex mtx;
};