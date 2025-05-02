#pragma once
#include "ChatServer.h"
#include "App.h"
#include <memory>
#include <chrono>
#include <iostream>


int main() 
{
    std::cout << "Main:\n";
    App app;

    std::string command;
    std::shared_ptr<ChatServer> global_server;

    try {
        boost::asio::io_context io;
        global_server = std::make_shared<ChatServer>(io, 1234);

        std::thread networking([&]()
            {
                io.run();
            });

        while (global_server->Running())
        {
            app.Go();
        }
        networking.join();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

//Client -> Server -> MSG -> App -> MSG -> Server -> Client