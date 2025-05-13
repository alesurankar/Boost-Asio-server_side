#include "App.h"

App::App()
{}


std::pair<int, int> App::UpdatePos(const std::string& command)
{
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
    return std::pair<int, int>(x, y);
}
