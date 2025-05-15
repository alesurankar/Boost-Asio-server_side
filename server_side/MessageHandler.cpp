#include "MessageHandler.h"
#include <iostream>


MessageHandler::MessageHandler()
{}


void MessageHandler::ServerToMSG(const std::string& message)
{
	std::lock_guard<std::mutex> lock(IN_mtx);
	app_messages.push(message);
	std::cout << "void MessageHandler::ServerToMSG(const std::string& message): " << message << " pushed to queue\n";
}


std::string MessageHandler::MSGToApp()
{
	{
		std::lock_guard<std::mutex> lock(IN_mtx);
		if (!app_messages.empty())
		{
			msg = app_messages.front();
			app_messages.pop();
		}
		else
		{
			msg = "";
		}
	}
	return msg;
}


void MessageHandler::AppToMSG(const std::string& response)
{
	std::lock_guard<std::mutex> lock(OUT_mtx);
	app_responses.push(response);
}


std::string MessageHandler::MSGToServer()
{
	{
		std::lock_guard<std::mutex> lock(OUT_mtx);
		if (!app_responses.empty())
		{
			response = app_responses.front();
			app_responses.pop();
		}
		else
		{
			response = "";
		}
	}
	return response;
}


//void MessageHandler::ServerToMSG(const std::string& message)
//{
//	{
//		std::lock_guard<std::mutex> lock(mtx);
//
//		std::pair<int, int> pos = app.UpdatePos(message);
//
//		int x = pos.first;
//		int y = pos.second;
//		app_position.push(std::make_pair(x, y));
//	}
//}


//std::string MessageHandler::MSGToServer()
//{
//	{
//		std::lock_guard<std::mutex> lock(pos_mtx);
//		if (!app_position.empty())
//		{
//			auto pos = app_position.front();
//			app_position.pop();
//			msg = std::to_string(pos.first) + "," + std::to_string(pos.second) + "\n";
//		}
//		else
//		{
//			msg = "";
//		}
//	}
//	return msg;
//}