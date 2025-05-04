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
//	void AppToMSG();
//	std::optional<std::pair<int, int>> MSGToServer();
private:
	std::mutex mtx1;
	std::mutex mtx2;
	std::queue<std::string> app_messages;
	App app;
//	std::queue<std::string> client_messages;
//	std::mutex mtx3;
//	std::mutex mtx4;
//	std::queue<std::pair<int, int>> app_position;
//
};