#pragma once
#include "MessageHandler.h"
#include <string>
#include <mutex>


class App
{
public:
	App();
	void Run(); 
	void ProcessReceivedMessage();
	void UpdatePos(const std::string& command);
private:
	MessageHandler msg;
	int x = 0;
	int y = 0;
	std::queue<std::string> command_queue;
	std::mutex mtx;
};