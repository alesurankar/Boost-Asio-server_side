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
	std::string MSGToServer();
private:
	std::mutex msg_mtx;
	std::mutex pos_mtx;
	std::queue<std::string> app_messages;
	App app;
	std::queue<std::pair<int, int>> app_position;
	std::string msg = "";
};