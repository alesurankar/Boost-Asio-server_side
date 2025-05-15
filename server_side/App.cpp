#include "App.h"
#include <iostream>

App::App(std::atomic<bool>& runFlag, std::shared_ptr<MessageHandler> msgHandler_in)
    :
    msgHandler(msgHandler_in),
    running(runFlag),
    noUpdate(true),
    nextFrame(true)
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
    if (nextFrame)
    {
        //std::cout << "----------------------------------------------\n";
        //float dt = ft.Mark();
        //float dtMs = dt * 1000.0f;
        //std::cout << "void App::Go(): Frame Time: " << dtMs << " ms\n";

        //GetMessage();
        SetMessage();
        nextFrame = false;
        //std::cout << "nextFrame: " << nextFrame << "\n\n";
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(8));
}

//void App::GetMessage()
//{
//    std::lock_guard<std::mutex> lock(mtx_in);
//    std::string in_msg = msgHandler->MSGToApp();
//    if (!in_msg.empty())
//    {
//        msg_toUpdate.push(in_msg);
//        std::cout << "void App::GetMessage(): " << in_msg << " pushed to msg_toUpdate\n";
//    }
//}

void App::SetMessage()
{
    //std::lock_guard<std::mutex> lock(mtx_out);
    std::string out_msg;
    //std::cout << "void App::SetMessage(): msg_isUpdated.size(): " << msg_isUpdated.size() << "\n";
    if (!msg_isUpdated.empty())
    {
        out_msg = msg_isUpdated.front();
        msg_isUpdated.pop();
        //if (!noUpdate)
        //{
            //std::cout << "void App::SetMessage(): " << out_msg << "-poped from msg_isUpdated\n";
            //std::cout << "void App::SetMessage(): msg_isUpdated.size(): " << msg_isUpdated.size() << "\n";
        //}
        msgHandler->AppToMSG(out_msg);
    }
}

///////////////////////////////////////////////////////////////////////////////

void App::UpdateLoop()
{
    while (running)
    {
        if (!nextFrame)
        {
            //float dt = ft.Mark();
            //float dtMs = dt * 1000.0f;
            //std::cout << "void App::UpdateLoop(): Frame Time: " << dtMs << " ms\n";

            //TakeFromQueue();
            UpdateParameters(message);
            PushToQueue();
            nextFrame = true;
            //std::cout << "nextFrame: " << nextFrame << "\n";
            //std::cout << "----------------------------------------------\n\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }
}

//void App::TakeFromQueue()
//{
//    std::lock_guard<std::mutex> lock(mtx_in);
//    if (!msg_toUpdate.empty())
//    {
//        message = msg_toUpdate.front();
//        msg_toUpdate.pop();
//        std::cout << "void App::TakeFromQueue(): " << message << " poped from msg_toUpdate\n";
//    }
//}

void App::UpdateParameters(const std::string& command)
{
    message = "";
    //std::cout << "void App::UpdateParameters(const std::string& command): " << command << " updating\n";
    if (command == "FIRST_MESSAGE")
    {
        return;
    }
    for (char c : command)
    {
        switch (c)
        {
        case 'W': y--; noUpdate = false; break;
        case 'S': y++; noUpdate = false; break;
        case 'A': x--; noUpdate = false; break;
        case 'D': x++; noUpdate = false; break;
        
        default: noUpdate = true; break;
        }
    }
    //std::cout << "void App::UpdateParameters(const std::string& command): noUpdate flag: " << noUpdate <<"\n\n";
}

void App::PushToQueue()
{
    //std::lock_guard<std::mutex> lock(mtx_out);
    message = std::to_string(x) + "," + std::to_string(y);
    msg_isUpdated.push(message);
    //if (!noUpdate)
    //{
        //std::cout << "void App::PushToQueue(): " << message << "-pushed to msg_isUpdated\n";
        //std::cout << "void App::PushToQueue(): msg_isUpdated.size(): " << msg_isUpdated.size() << "\n";
    //}
}