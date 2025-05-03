#pragma once
#include "App.h"
#include <string>
#include <mutex>
#include <queue>
#include <optional>

class MessageHandler
{
public:
	MessageHandler();
	void ServerToMSG(const std::string& message);
	void MSGToApp();
	void AppToMSG();
	std::optional<std::pair<int, int>> MSGToServer();
private:
	App app;
	std::queue<std::string> app_messages;
	std::queue<std::string> client_messages;
	std::mutex mtx;
	std::queue<std::pair<int, int>> app_position; 

};