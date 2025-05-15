#include "App.h"
#include <iostream>

App::App(std::atomic<bool>& runFlag, std::shared_ptr<MessageHandler> msgHandler_in)
    :
    msgHandler(msgHandler_in),
    running(runFlag)
{
    UpdateThread = std::thread(&App::UpdateLoop, this);
}

App::~App()
{
    if (UpdateThread.joinable())
    {
        UpdateThread.join();
    }
}

void App::Go()
{
    //float dt = ft.Mark();
    //float dtMs = dt * 1000.0f;
    //std::cout << "Frame Time: " << dtMs << " ms" << std::endl;

    GetMessage();
    SetMessage();
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
}

void App::GetMessage()
{
    std::lock_guard<std::mutex> lock(mtx_in);
    std::string in_msg = msgHandler->MSGToApp();
    msg_toUpdate.push(in_msg);
}

void App::SetMessage()
{
    std::lock_guard<std::mutex> lock(mtx_out);
    std::string out_msg;
    if (!msg_isUpdated.empty())
    {
        out_msg = msg_isUpdated.front();
        msg_isUpdated.pop();
        msgHandler->AppToMSG(out_msg);
    }
}

///////////////////////////////////////////////////////////////////////////////

void App::UpdateLoop()
{
    //float dt = ft.Mark();
    //float dtMs = dt * 1000.0f;
    //std::cout << "Frame Time: " << dtMs << " ms" << std::endl;

    TakeFromQueue();
    UpdateParameters(message);
    PushToQueue();
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
}

void App::TakeFromQueue()
{
    std::lock_guard<std::mutex> lock(mtx_in);
    if (!msg_toUpdate.empty())
    {
        message = msg_toUpdate.front();
        msg_toUpdate.pop();
    }
}

void App::UpdateParameters(const std::string& command)
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

void App::PushToQueue()
{
    std::lock_guard<std::mutex> lock(mtx_out);
    message = std::to_string(x) + "," + std::to_string(y) + "\n";
    msg_toUpdate.push(message);
}