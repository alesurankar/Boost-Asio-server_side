#include "App.h"
#include <iostream>

App::App()
{}


void App::UpdatePos(const std::string& command) //6. AppServer(updateParameters)
{
    std::cout << "Step 7, App::UpdatePos: " << command << "\n";
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
    std::cout << "Step 7--------------\n";
}


std::pair<int, int> App::ReturnPos()
{
    std::cout << "Step 9, App::ReturnPos: " << "x = " << x << ", y = " << y << "\n";
    std::cout << "Step 9--------------\n";
    return std::pair<int, int>(x, y); //7. MSGServer(middleman)
}

