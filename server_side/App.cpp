#include "App.h"
#include <iostream>

App::App(std::atomic<bool>& runFlag, std::shared_ptr<MessageHandler> msgHandler_in)
    :
    msgHandler(msgHandler_in),
    running(runFlag)
{
    //UpdateThread = std::thread(&App::InputLoop, this);
}


//std::pair<int, int> App::UpdatePos(const std::string& command)
//{
//    Unpack(command);
//    return std::pair<int, int>(x, y);
//}


void App::UpdateLoop()
{
    float dt = ft.Mark();
    float dtMs = dt * 1000.0f;
    //std::cout << "Frame Time: " << dtMs << " ms" << std::endl;

    GetMessage();
    UpdateParameters();
    SetMessage();
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
}


void App::GetMessage()
{
    msg = msgHandler->MSGToApp();
}


void App::UpdateParameters()
{
    Unpack(msg);
}


void App::SetMessage()
{
    msgHandler->AppToMSG(msg);
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


void App::PackToString()
{
    msg = std::to_string(x) + "," + std::to_string(y) + "\n";
}
