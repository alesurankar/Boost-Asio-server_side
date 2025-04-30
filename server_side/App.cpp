#include "App.h"

App::App()
{
}

void App::Run()
{
    UpdatePos(msg.MSGToApp());
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