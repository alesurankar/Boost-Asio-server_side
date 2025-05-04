#include "MessageHandler.h"
#include <iostream>


MessageHandler::MessageHandler()
{}


void MessageHandler::ServerToMSG(const std::string& message)  //5. MSGServer(middleman)
{
	std::cout << "Step 5, MessageHandler::ServerToMSG: ";
	{
		std::lock_guard<std::mutex> lock(mtx);
		app_messages.push(message);
		std::cout << "Pushed message: " << message << "\n";
	}
	MSGToApp();
	std::cout << "Step 5--------------\n";
}


void MessageHandler::MSGToApp()  //6. AppServer(updateParameters)
{
	std::cout << "Step 6, MessageHandler::MSGToApp::Queue size : \n";
	{
	    std::lock_guard<std::mutex> lock(mtx);
	    if (!app_messages.empty())
	    {
	    	std::string message = app_messages.front();
	    	app.UpdatePos(message);
	    	app_messages.pop();
	    	std::cout << "Popped message: " << message << "\n";
	    }
	}
	AppToMSG();
	std::cout << "Step 6--------------\n";
}


void MessageHandler::AppToMSG() //7. MSGServer(middleman)
{
	std::cout << "Step 8, MessageHandler::AppToMSG: \n";
	{
		std::lock_guard<std::mutex> lock(mtx);
		std::pair<int, int> pos = app.ReturnPos();
		int x = pos.first;
		int y = pos.second;
		std::cout << "x = " << x << ", y = " << y << "\n";
		app_position.push(std::make_pair(x, y));
	}
	std::cout << "Step 8--------------\n";
}

std::string MessageHandler::MSGToServer()
{
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (!app_position.empty())
		{
			std::cout << "Step 10, MessageHandler::MSGToServer: \n";
			auto pos = app_position.front();
			app_position.pop();
			msg = std::to_string(pos.first) + "," + std::to_string(pos.second) + "\n";
			std::cout << "Popped position and created message: " << msg;
			std::cout << "Step 10--------------\n";
		}
		else
		{
			msg = "";
		}
	}
	return msg;
}