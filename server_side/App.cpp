#include "App.h"
#include <iostream>

App::App()
{}


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
    std::cout << "--------------\n";
}

std::pair<int, int> App::ReturnPos()
{
    std::cout << "App::ReturnPos: " << "x = " << x << ", y = " << y << ", //7. MSGServer(middleman)\n";
    return std::pair<int, int>(x, y); //7. MSGServer(middleman)
}
