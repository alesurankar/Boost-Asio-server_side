#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "MessageHandler.h"
#include <boost/json.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <map>

using boost::asio::ip::tcp;

class ChatServer {
public:
    ChatServer(boost::asio::io_context& io_in, const std::string& address_in, unsigned short port_in);
    void Start();
private:
    void AcceptConnections();
    void HandleClient(std::shared_ptr<tcp::socket> socket, int client_id);
    void SendToClient(std::shared_ptr<tcp::socket> socket, int client_id);
    void RemoveClient(std::shared_ptr<tcp::socket> socket);
private:
    MessageHandler msg;
    boost::asio::io_context& io;
    tcp::acceptor acceptor;
    std::mutex mtx;
    std::atomic<int> client_counter;
    std::vector<std::shared_ptr<tcp::socket>> clients;
    std::map<std::shared_ptr<tcp::socket>, std::string> client_names;
    std::atomic<bool> valid_username_check;
};