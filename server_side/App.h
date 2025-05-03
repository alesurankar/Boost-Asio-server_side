#pragma once
#include "MessageHandler.h"
#include <string>
#include <mutex>


class App
{
public:
	App();
	void Go();
	void ProcessReceivedMessage();
	void UpdatePos(const std::string& command);
private:
	std::shared_ptr<MessageHandler> msgHandler = std::make_shared<MessageHandler>();
	int x = 0;
	int y = 0;
	std::queue<std::string> command_queue;
	std::mutex mtx;
};