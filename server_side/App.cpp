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

void App::ProcessReceivedMessage() //6. AppServer(updateParameters)
{
    std::cout << "App::ProcessReceivedMessage: //6. AppServer(updateParameters)\n";
    std::string message = msgHandler->MSGToApp(); //6. AppServer(updateParameters)

    if (!message.empty())
    {
        UpdatePos(message);
    }
}

void App::UpdatePos(const std::string& command) //6. AppServer(updateParameters)
{
    std::cout << "App::UpdatePos: " << command << "//6. AppServer(updateParameters)\n";
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
    msgHandler->AppToMSG(x,y); //7. MSGServer(middleman)
    std::cout << "--------------\n";
}