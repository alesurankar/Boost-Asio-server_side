#pragma once
#include "ChatServer.h"
#include "App.h"
#include <memory>
#include <chrono>



int main() 
{
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

        while (true)
        {
            app.Run();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
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
