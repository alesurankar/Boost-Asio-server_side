#include "MessageHandler.h"
#include <iostream>

MessageHandler::MessageHandler()
{}

void MessageHandler::ServerToMSG(const std::string& message)  //5. MSGServer(middleman)
{
	std::cout << "MessageHandler::ServerToMSG: " << message << ", //5. MSGServer(middleman)\n";
	{
		std::lock_guard<std::mutex> lock(mtx1);
		{
			app_messages.push(message);
		}
		MSGToApp();
	}
	std::cout << "--------------\n";
}

void MessageHandler::MSGToApp()  //6. AppServer(updateParameters)
{
	std::cout << "MessageHandler::MSGToApp::Queue size : " << app_messages.size() << ", //6. AppServer(updateParameters)\n";
	{
		std::lock_guard<std::mutex> lock(mtx2);
		if (!app_messages.empty())
		{
			std::string message = app_messages.front();
			app.UpdatePos(message);
			app_messages.pop();
			std::cout << "Popped message: " << message << "\n";
		}
	}
	AppToMSG();
}

void MessageHandler::AppToMSG() //7. MSGServer(middleman)
{
	std::cout << "MessageHandler::AppToMSG:  //7. MSGServer(middleman)\n";
	{
		std::lock_guard<std::mutex> lock(mtx3);
		std::pair<int, int> pos = app.ReturnPos();
		int x = pos.first;
		int y = pos.second;
		std::cout << "x = " << x << ", y = " << y << "\n";
		app_position.push(std::make_pair(x, y));
	}
	MSGToServer();
	std::cout << "--------------\n";
}

std::optional<std::pair<int, int>> MessageHandler::MSGToServer()
{
	std::cout << "MessageHandler::MSGToServer:\n";
	std::lock_guard<std::mutex> lock(mtx4);
	if (!app_position.empty())
	{
		auto pos = app_position.front();
		app_position.pop();
		return pos;
	}
	return std::nullopt;
	std::cout << "--------------\n";
}