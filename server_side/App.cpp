#include "App.h"
#include <iostream>

App::App()
{
}

void App::Go()
{
    //std::cout << "App::Run:\n";
    ProcessReceivedMessage();
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

void App::ProcessReceivedMessage()
{
    //std::cout << "App::ProcessReceivedMessage:\n";
    std::string message = msgHandler->MSGToApp();

    if (!message.empty())
    {
        UpdatePos(message);
    }
}

void App::UpdatePos(const std::string& command)
{
    std::cout << "App::UpdatePos:\n";
    if (command == "move_up")
    {
        y--;
    }
    else if (command == "move_down")
    {
        y++;
    }
    else if (command == "move_left")
    {
        x--;
    }
    else if (command == "move_right")
    {
        x++;
    }
    msgHandler->AppToMSG(x,y);
    std::cout << "--------------\n";
}