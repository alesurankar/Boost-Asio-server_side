#pragma once
#include <string>
#include <mutex>
#include <queue>

class MessageHandler
{
public:
	MessageHandler();

	void ServerToMSG(const std::string& message);
	std::string MSGToApp();
	void AppToMSG(const std::string& response);
	std::string MSGToServer();
private:
	std::mutex IN_mtx;
	std::mutex OUT_mtx;
	std::queue<std::string> app_messages;
	std::queue<std::string> app_responses;
	//std::queue<std::pair<int, int>> app_position;
	std::string msg = "";
	std::string response = "";
};