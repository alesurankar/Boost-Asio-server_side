#include "MessageHandler.h"
#include <iostream>


MessageHandler::MessageHandler()
{}


void MessageHandler::ServerToMSG(const std::string& message)
{
	{
		std::lock_guard<std::mutex> lock(mtx);

		std::pair<int, int> pos = app.UpdatePos(message);

		int x = pos.first;
		int y = pos.second;
		app_position.push(std::make_pair(x, y));
	}
}


std::string MessageHandler::MSGToServer()
{
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (!app_position.empty())
		{
			auto pos = app_position.front();
			app_position.pop();
			msg = std::to_string(pos.first) + "," + std::to_string(pos.second) + "\n";
		}
		else
		{
			msg = "";
		}
	}
	return msg;
}