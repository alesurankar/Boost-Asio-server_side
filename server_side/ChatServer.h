#pragma once
#include "MessageHandler.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <map>

using boost::asio::ip::tcp;

class ChatServer : public std::enable_shared_from_this<ChatServer>
{
public:
    ChatServer(boost::asio::io_context& io, const std::string& address, unsigned short port, std::shared_ptr<MessageHandler> handler);
    void Start();
private:
    void AcceptConnections();
    void HandleClient(std::shared_ptr<tcp::socket> socket, int client_id);
    void RemoveClient(std::shared_ptr<tcp::socket> socket);
private:
    std::shared_ptr<MessageHandler> msgHandler;
    boost::asio::io_context& io_;
    tcp::acceptor acceptor_;
    std::atomic<int> client_counter_;
    std::vector<std::shared_ptr<tcp::socket>> clients_;
    std::mutex clients_mutex_;
    std::map<std::shared_ptr<tcp::socket>, std::string> client_names_;
};