#pragma once
#include "App.h"
#include <string>
#include <mutex>
#include <queue>

class MessageHandler
{
public:
	MessageHandler();
	void ServerToMSG(const std::string& message);
	std::string MSGToServer();
private:
	std::mutex mtx;
	App app;
	std::queue<std::pair<int, int>> app_position;
	std::string msg = "";
};