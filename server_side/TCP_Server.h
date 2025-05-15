#pragma once
#define _WIN32_WINNT 0x0A00
#include "MessageHandler.h"
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <deque>
#include <unordered_set>
#include <atomic>
#include "FrameTimer.h"


using boost::asio::ip::tcp;

class ChatSession;

class TCP_Server : public std::enable_shared_from_this<TCP_Server>
{
public:
    TCP_Server(boost::asio::io_context& io_context, short port, std::shared_ptr<MessageHandler> msgHandler_in);
//    bool Running();
    void Join(std::shared_ptr<ChatSession> client_session);
    void Leave(std::shared_ptr<ChatSession> client_session);
private:
    void Accept();
private:
    tcp::acceptor server_acceptor;
    std::unordered_set<std::shared_ptr<ChatSession>> active_sessions;
    std::shared_ptr<MessageHandler> msgHandler;
//    std::atomic<bool> is_running{ true };
};


class ChatSession : public std::enable_shared_from_this<ChatSession>
{
public:
    ChatSession(tcp::socket socket_in, std::weak_ptr<TCP_Server> server_in, std::shared_ptr<MessageHandler> msgHandler_in);
    void Start();
private:
    void ReadMessage();
    void CheckAndSend();
private:
    tcp::socket client_socket;
    std::weak_ptr<TCP_Server> chat_server;
    boost::asio::streambuf input_buffer;
    std::shared_ptr<MessageHandler> msgHandler;
    std::string msg;
    boost::asio::steady_timer timer;
    FrameTimer ft;
//    std::string read_msg_;
//    std::deque<std::string> write_msgs_;
};