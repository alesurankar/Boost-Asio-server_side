#include "MessageHandler.h"

MessageHandler::MessageHandler()
{
}

void MessageHandler::ServerToMSG(const std::string& message)
{
    std::unique_lock<std::mutex> lock(mtx);
    app_messages.push(message);
}

std::string MessageHandler::MSGToApp()
{
	std::string msg = app_messages.front();
	app_messages.pop();
	return msg;
}

void MessageHandler::AppToMSG(int x, int y)
{
	std::lock_guard<std::mutex> lock(mtx);
	app_position.push(std::make_pair(x, y));
}

std::optional<std::pair<int, int>> MessageHandler::MSGToServer()
{
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
}