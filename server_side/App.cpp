#include "App.h"

App::App()
{}


std::pair<int, int> App::UpdatePos(const std::string& command)
{
    Unpack(command);
    return std::pair<int, int>(x, y);
}


void App::Unpack(const std::string& command)
{
    if (command == "FIRST_MESSAGE")
    {
        return;
    }
    for (char c : command)
    {
        switch (c)
        {
        case 'W': y--; break;
        case 'S': y++; break;
        case 'A': x--; break;
        case 'D': x++; break;
            // Add other input handling here
        default: break;
        }
    }
}