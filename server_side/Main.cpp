#pragma once
#include "ChatServer.h"
#include "App.h"
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>

int main()
{
    std::shared_ptr<MessageHandler> msgHandler = std::make_shared<MessageHandler>();
    std::shared_ptr<ChatServer> global_server;
	std::atomic<bool> running{ true };

	App theApp(running, msgHandler);

    boost::asio::io_context io;
    global_server = std::make_shared<ChatServer>(io, 1234, msgHandler);
    
	std::thread networking([&]()
		{
			io.run();
		});

	while (running)
	{
		theApp.UpdateLoop();
	}

	running = false;

	io.stop(); 

	if (networking.joinable())
	{
		networking.join();
	}
    return 0;
}