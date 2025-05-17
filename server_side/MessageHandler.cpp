#include "MessageHandler.h"
//#include <iostream>


MessageHandler::MessageHandler()
{}


void MessageHandler::ServerToMSG(const std::string& message)
{
	std::lock_guard<std::mutex> lock(IN_mtx);
	app_messages.push(message);
	//std::cout << "void MessageHandler::ServerToMSG(const std::string& message): " << message << " pushed to queue\n";
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
			msg.clear();
		}
	}
	return msg;
}

void MessageHandler::AppToMSG(const std::string& response)
{
	std::lock_guard<std::mutex> lock(OUT_mtx);
	validResponse = response;
	//std::cout << "void MessageHandler::AppToMSG(const std::string& response): " << validResponse << "-waiting to be read\n";
	//std::cout << "void MessageHandler::AppToMSG(const std::string& response): " << response << "-pushed to app_responses\n";
}

std::string MessageHandler::MSGToServer()
{
	std::lock_guard<std::mutex> lock(OUT_mtx);
	std::string messageToServer = validResponse + "\n";
	validResponse.clear();
	//std::cout << "std::string MessageHandler::MSGToServer(): " << messageToServer << "-was read\n";
	return messageToServer;
}