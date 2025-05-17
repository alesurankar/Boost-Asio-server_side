#include "App.h"
#include "Constants.h"
#include <iostream>

App::App(std::atomic<bool>& runFlag, std::shared_ptr<MessageHandler> msgHandler_in)
    :
    msgHandler(msgHandler_in),
    running(runFlag),
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

        GetMessage();
        SetMessage();
        nextFrame = false;
        //std::cout << "nextFrame: " << nextFrame << "\n\n";
    }
}

void App::GetMessage()
{
    //std::lock_guard<std::mutex> lock(mtx_in);
    std::string in_msg = msgHandler->MSGToApp();
    if (!in_msg.empty())
    {
        msg_toUpdate.push(in_msg);
        //std::cout << "void App::GetMessage(): " << in_msg << " pushed to msg_toUpdate\n";
    }
}

void App::SetMessage()
{
    //std::lock_guard<std::mutex> lock(mtx_out);
    std::string out_msg;
    //std::cout << "void App::SetMessage(): msg_isUpdated.size(): " << msg_isUpdated.size() << "\n";
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
    while (running)
    {
        if (!nextFrame)
        {
            //float dt = ft.Mark();
            //float dtMs = dt * 1000.0f;
            //std::cout << "void App::UpdateLoop(): Frame Time: " << dtMs << " ms\n";

            TakeFromQueue();
            UpdateGame();
            UpdateParameters(message);
            PushToQueue();
            nextFrame = true;
            //std::cout << "nextFrame: " << nextFrame << "\n";
            //std::cout << "----------------------------------------------\n\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(4));
    }
}

void App::TakeFromQueue()
{
    std::lock_guard<std::mutex> lock(mtx_in);
    if (!msg_toUpdate.empty())
    {
        message = msg_toUpdate.front();
        msg_toUpdate.pop();
        //std::cout << "void App::TakeFromQueue(): " << message << " poped from msg_toUpdate\n";
    }
}

void App::UpdateParameters(std::string command)
{
    message = "";

    if (command.compare(0, Constants::prefix_len, Constants::prefix) == 0)
    {
        const char* data = command.c_str() + Constants::prefix_len;

        // Parse x
        x = 0;
        while (*data >= '0' && *data <= '9') 
        {
            x = x * 10 + (*data - '0');
            ++data;
        }

        y = 0;
        if (*data == ',') ++data;

        // Parse y
        while (*data >= '0' && *data <= '9') 
        {
            y = y * 10 + (*data - '0');
            ++data;
        }
    }

    //std::cout << "void App::UpdateParameters(const std::string& command): " << command << " updating\n";
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
        
        default: break;
        }
    }
    //std::cout << "void App::UpdateParameters(const std::string& command): x = : " << x <<"\n";
    //std::cout << "void App::UpdateParameters(const std::string& command): y = : " << y << "\n\n";
}

void App::UpdateGame()
{
    if (xEnemy < 100)
    {
        moveLeft = false;
    }
    if (xEnemy > 700)
    {
        moveLeft = true;
    }
    if (yEnemy < 100)
    {
        moveUP = false;
    }
    if (yEnemy > 500)
    {
        moveUP = true;
    }
    if (moveUP)
    {
        yEnemy--;
    }
    else if (!moveUP)
    {
        yEnemy++;
    }
    if (moveLeft)
    {
        xEnemy--;
    }
    else if (!moveLeft)
    {
        xEnemy++;
    }
}

void App::PushToQueue()
{
    //std::lock_guard<std::mutex> lock(mtx_out);
    std::string player = "player:" + std::to_string(x) + "," + std::to_string(y) +"|";
    std::string game = "enemy:" + std::to_string(xEnemy) + "," + std::to_string(yEnemy);
    message = player + game;
    msg_isUpdated.push(message);
}