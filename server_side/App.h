#pragma once
#include <string>
#include <mutex>


class App
{
public:
	App();
	void UpdatePos(const std::string& command);
//	std::pair<int, int> ReturnPos();
private:
	int x = 0;
	int y = 0;
//	std::mutex mtx;
};