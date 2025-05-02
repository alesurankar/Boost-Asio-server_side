#include "App.h"
#include <iostream>

App::App()
{
}

void App::Go()
{
    std::cout << "App::Run:\n";
    ProcessReceivedMessage();
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

void App::ProcessReceivedMessage()
{
    std::cout << "App::ProcessReceivedMessage:\n";
    std::lock_guard<std::mutex> lock(mtx);

    if (!command_queue.empty())
    {
        std::string command = command_queue.front();
        command_queue.pop();

        if (!command.empty())
        {
            UpdatePos(command);
        }
    }
}

void App::UpdatePos(const std::string& command)
{
    std::cout << "App::UpdatePos:\n";
    if (command == "move_up")
    {
        y--;
    }
    if (command == "move_down")
    {
        y++;
    }
    if (command == "move_left")
    {
        x--;
    }
    if (command == "move_right")
    {
        x++;
    }
    msg.AppToMSG(x,y);
}