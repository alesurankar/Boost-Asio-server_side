#pragma once
#include "ChatServer.h"
#include <iostream>
#include <memory>
#include <thread>


int main()
{
    std::cout << "Main:\n";
    std::shared_ptr<MessageHandler> msgHandler = std::make_shared<MessageHandler>();
    std::shared_ptr<ChatServer> global_server;

    try 
    {
        boost::asio::io_context io;
        global_server = std::make_shared<ChatServer>(io, 1234, msgHandler);

        io.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }


    return 0;
}