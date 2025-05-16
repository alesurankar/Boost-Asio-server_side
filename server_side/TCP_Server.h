#pragma once
#define _WIN32_WINNT 0x0A00
#include "MessageHandler.h"
#include "FrameTimer.h"
#include <boost/asio.hpp>
//#include <boost/beast.hpp>
//#include <boost/json.hpp>
#include <iostream>
#include <memory>
//#include <unordered_map>
//#include <deque>
#include <unordered_set>
#include <atomic>
#include <mutex>
//#include <map>


using boost::asio::ip::tcp;

class TCP_Session;

class TCP_Server : public std::enable_shared_from_this<TCP_Server>
{
public:
    TCP_Server(boost::asio::io_context& io_context, const std::string& address, short port, std::shared_ptr<MessageHandler> msgHandler_in);
//    bool Running();
    void Join(std::shared_ptr<TCP_Session> client_session);
    void Leave(std::shared_ptr<TCP_Session> client_session);
//    //void RemoveClient(std::shared_ptr<tcp::socket> socket);
private:
    void Accept();
private:
    std::atomic<int> client_counter;
    tcp::acceptor server_acceptor;
    std::unordered_set<std::shared_ptr<TCP_Session>> active_sessions;
    std::vector<std::shared_ptr<tcp::socket>> clients;
    std::shared_ptr<MessageHandler> msgHandler;
//    std::atomic<bool> is_running{ true };
//    std::map<std::shared_ptr<tcp::socket>, std::string> client_names;
};


class TCP_Session : public std::enable_shared_from_this<TCP_Session>
{
public:
    TCP_Session(int client_id_in, tcp::socket socket_in, std::weak_ptr<TCP_Server> server_in, std::shared_ptr<MessageHandler> msgHandler_in);
    void Start();
private:
    void HandleClient(std::shared_ptr<tcp::socket> socket, int client_id); 
    //void HandleClient(tcp::socket socket, int client_id);
    void ReadMessage();
    void CheckAndSend();
//    //bool SendSignalToFastAPI(const std::string& username);
private:
    int client_id;
    std::string msg;
    //tcp::socket client_socket;
    std::shared_ptr<tcp::socket> client_socket;
    std::weak_ptr<TCP_Server> chat_server;
    boost::asio::streambuf input_buffer;
    std::shared_ptr<MessageHandler> msgHandler;
    boost::asio::steady_timer timer;
    std::mutex clients_mutex;
//    FrameTimer ft;
//    //boost::asio::io_context& io_;
//    //tcp::acceptor acceptor_;
};