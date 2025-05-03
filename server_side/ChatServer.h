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

class ChatServer : public std::enable_shared_from_this<ChatServer>
{
public:
    ChatServer(boost::asio::io_context& io_context, short port, std::shared_ptr<MessageHandler> handler);
    bool Running();
    void Join(std::shared_ptr<ChatSession> session);
    void Leave(std::shared_ptr<ChatSession> session);
private:
    void Accept();
private:
    std::shared_ptr<MessageHandler> msgHandler;
    tcp::acceptor acceptor_;
    std::unordered_set<std::shared_ptr<ChatSession>> sessions_;
    std::atomic<bool> is_running{ true };
};


class ChatSession : public std::enable_shared_from_this<ChatSession>
{
public:
    ChatSession(tcp::socket socket, std::weak_ptr<ChatServer> server, std::shared_ptr<MessageHandler> handler);
    void Start();
private:
    void ReadMessage();
    void CheckAndSendMessage();
private:
    std::shared_ptr<MessageHandler> msgHandler;
    tcp::socket socket_;
    std::string read_msg_;
    std::deque<std::string> write_msgs_;
    std::weak_ptr<ChatServer> server_;
    boost::asio::streambuf buffer_;
    boost::asio::steady_timer message_timer;
};