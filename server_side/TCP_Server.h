#pragma once
#define _WIN32_WINNT 0x0A00
#include "MessageHandler.h"
#include "FrameTimer.h"
#include <boost/asio.hpp>

#include <boost/beast.hpp>
#include <boost/json.hpp>

#include <iostream>
#include <memory>
#include <unordered_set>
#include <atomic>
#include <mutex>


using boost::asio::ip::tcp;

class TCP_Session;

class TCP_Server : public std::enable_shared_from_this<TCP_Server>
{
public:
    TCP_Server(boost::asio::io_context& io_context, const std::string& address, short port, std::shared_ptr<MessageHandler> msgHandler_in);
    void Leave(std::shared_ptr<TCP_Session> client_session);
private:
    void Join(std::shared_ptr<TCP_Session> client_session);
    void Accept();
private:
    std::atomic<int> client_counter;
    tcp::acceptor server_acceptor;
    std::unordered_set<std::shared_ptr<TCP_Session>> active_sessions;
    std::vector<std::shared_ptr<tcp::socket>> clients;
    std::shared_ptr<MessageHandler> msgHandler;
};


class TCP_Session : public std::enable_shared_from_this<TCP_Session>
{
public:
    TCP_Session(tcp::socket socket_in, std::weak_ptr<TCP_Server> server_in, std::shared_ptr<MessageHandler> msgHandler_in);
    void Start();
    void Stop();
private:
    void HandleClient(); 
    void ReadMessage();
    void CheckAndSend();
    void ConnectToFastAPI();
    bool AskFastAPI();
    void SaveToFastAPI();
private:
    FrameTimer ft;
    std::string msg; 
    std::string username;
    std::shared_ptr<tcp::socket> client_socket;
    boost::asio::ip::tcp::socket fastapi_socket;
    tcp::resolver resolver;
    std::weak_ptr<TCP_Server> chat_server;
    boost::asio::streambuf input_buffer;
    std::shared_ptr<MessageHandler> msgHandler;
    boost::asio::steady_timer timer;
    std::mutex clients_mutex; 
};