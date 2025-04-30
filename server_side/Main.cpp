#pragma once
#include "ChatServer.h"
#include "App.h"



int main() 
{
    //App app;

    std::string command;

    boost::asio::io_context io;
    ChatServer server(io, "127.0.0.1", 1234);
    //std::thread networking([&]() 
    //    { 
            server.Start();
            io.run(); 
    //    });

    //while (true)
    //{
    //    app.Run();
    //}
    //
    //networking.join();
    return 0;
}

//Client -> Server -> MSG -> App -> MSG -> Server -> Client