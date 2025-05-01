#include "App.h"

App::App()
{
}

void App::Run()
{
    ProcessReceivedMessage();
}

void App::ProcessReceivedMessage()
{
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