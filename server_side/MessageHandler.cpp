#include "MessageHandler.h"
#include <iostream>

MessageHandler::MessageHandler()
{
}

void MessageHandler::ServerToMSG(const std::string& message)
{
	std::cout << "MessageHandler::ServerToMSG:\n";
	std::lock_guard<std::mutex> lock(mtx);
    app_messages.push(message);
	std::cout << "--------------\n";
}

std::string MessageHandler::MSGToApp()
{
	//std::cout << "MessageHandler::MSGToApp::Queue size : " << app_messages.size() << "\n";
	std::lock_guard<std::mutex> lock(mtx);
	if (!app_messages.empty())
	{
		std::string msg = app_messages.front();
		app_messages.pop();
		std::cout << "Popped message: " << msg << "\n";

		return msg;
	}
	return "";
}

void MessageHandler::AppToMSG(int x, int y)
{
	std::cout << "MessageHandler::AppToMSG:\n";
	std::lock_guard<std::mutex> lock(mtx);
	app_position.push(std::make_pair(x, y));
	std::cout << "--------------\n";
}

std::optional<std::pair<int, int>> MessageHandler::MSGToServer()
{
	//std::cout << "MessageHandler::MSGToServer:\n";
	std::lock_guard<std::mutex> lock(mtx);
	if (!app_position.empty())
	{
		auto pos = app_position.front();
		app_position.pop();
		return pos;
	}
	else
	{
		return std::nullopt;
	}
	std::cout << "--------------\n";
}