#pragma once
#include "MessageHandler.h"
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <deque>
#include <unordered_set>
#include <atomic>


using boost::asio::ip::tcp;

class ChatSession;

class ChatServer 
{
public:
    ChatServer(boost::asio::io_context& io_context, short port); 
    bool Running();
    void Join(std::shared_ptr<ChatSession> session);
    void Leave(std::shared_ptr<ChatSession> session);
    void Broadcast(const std::string& msg);
private:
    void Accept();
private:
    tcp::acceptor acceptor_;
    std::unordered_set<std::shared_ptr<ChatSession>> sessions_;
    std::atomic<bool> is_running{ true };
};


class ChatSession : public std::enable_shared_from_this<ChatSession>
{
public:
    ChatSession(tcp::socket socket, ChatServer* server);
    void Start();
    void Deliver(const std::string& msg);
private:
    void ReadMessage();
    void WriteMessage();
    void Broadcast(const std::string& msg);
private:
    tcp::socket socket_;
    std::string read_msg_;
    std::deque<std::string> write_msgs_;
    ChatServer* server_;
    boost::asio::streambuf buffer_; 
};