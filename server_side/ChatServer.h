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
    ChatServer(boost::asio::io_context& io, const std::string& address, unsigned short port);
    void Start();
private:
    void AcceptConnections();
    void HandleClient(std::shared_ptr<tcp::socket> socket, int client_id);
    void Broadcast(const std::string& msg, std::shared_ptr<tcp::socket> sender);
    bool SendSignalToFastAPI(const std::string& username);
    void RemoveClient(std::shared_ptr<tcp::socket> socket);
private:
    boost::asio::io_context& io_;
    tcp::acceptor acceptor_;
    std::atomic<int> client_counter_;
    std::vector<std::shared_ptr<tcp::socket>> clients_;
    std::mutex clients_mutex_;
    std::map<std::shared_ptr<tcp::socket>, std::string> client_names_;
};