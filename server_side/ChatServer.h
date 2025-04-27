#pragma once
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

class ChatServer {
public:
    ChatServer(boost::asio::io_context& io_in, const std::string& address_in, unsigned short port_in);
    void Start();
private:
    void AcceptConnections();
    void HandleClient(std::shared_ptr<tcp::socket> socket, int client_id);
    void Broadcast(const std::string& msg, std::shared_ptr<tcp::socket> sender);
    bool SendSignalToFastAPI(const std::string& username);
    void RemoveClient(std::shared_ptr<tcp::socket> socket);
private:
    boost::asio::io_context& io;
    tcp::acceptor acceptor;
    std::atomic<int> client_counter;
    std::vector<std::shared_ptr<tcp::socket>> clients;
    std::mutex clients_mutex;
    std::map<std::shared_ptr<tcp::socket>, std::string> client_names;
    std::atomic<bool> valid_username_check;
};