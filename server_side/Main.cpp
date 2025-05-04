#pragma once
#include "ChatServer.h"
#include <iostream>
#include <memory>
#include <thread>


int main()
{
    std::shared_ptr<MessageHandler> msgHandler = std::make_shared<MessageHandler>();
    std::cout << "Main:\n";
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


////Client -> Server -> MSG -> App -> MSG -> Server -> Client
//
////1. AppClient(input) -> 
////2. MSGClient(middleman) -> 
////3. Client(TCP) -> 
////4. Server(TCP) -> 
////5. MSGServer(middleman) -> 
////6. AppServer(updateParameters) -> 
////7. MSGServer(middleman) -> 
////8. Server(TCP) -> 
////9. Client(TCP) ->
////10. MSGClient(middleman) ->
////11. AppClient(displayOutput)