#pragma once
#include <mutex>
#include <string>
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
	std::string msg;
	std::string validResponse;
	std::queue<std::string> app_messages;
};