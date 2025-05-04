#include "App.h"
#include <iostream>

App::App()
{}


void App::UpdatePos(const std::string& command) //6. AppServer(updateParameters)
{
    std::cout << "Step 6, App::UpdatePos: " << command << "\n";
    if (command == "UP")
    {
        y--;
    }
    else if (command == "DWN")
    {
        y++;
    }
    else if (command == "LFT")
    {
        x--;
    }
    else if (command == "RGT")
    {
        x++;
    }
    std::cout << "Step 6--------------\n";
}


//std::pair<int, int> App::ReturnPos()
//{
//    std::cout << "App::ReturnPos: " << "x = " << x << ", y = " << y << ", //7. MSGServer(middleman)\n";
//    return std::pair<int, int>(x, y); //7. MSGServer(middleman)
//}
//
