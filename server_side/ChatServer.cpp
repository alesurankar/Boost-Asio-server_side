#include "ChatServer.h"

namespace beast = boost::beast;
namespace http = beast::http;

ChatServer::ChatServer(boost::asio::io_context& io, const std::string& address, unsigned short port, std::shared_ptr<MessageHandler> handler)
    : 
    io_(io),
    acceptor_(io, tcp::endpoint(boost::asio::ip::make_address(address), port)),
    msgHandler(handler)
{}


void ChatServer::Start() 
{
    AcceptConnections();
}


void ChatServer::AcceptConnections() 
{
    auto socket = std::make_shared<tcp::socket>(io_);
    acceptor_.async_accept(*socket, [this, socket](boost::system::error_code ec) 
        {
        if (!ec) 
        {
            int client_id = client_counter_++;

            {
                std::lock_guard<std::mutex> lock(clients_mutex_);
                clients_.push_back(socket);
                std::cout << "Client " << client_id << " trying to connect\n";
            }

            std::thread(&ChatServer::HandleClient, this, socket, client_id).detach();
        }
        AcceptConnections();
        });
}


void ChatServer::HandleClient(std::shared_ptr<tcp::socket> socket, int client_id) 
{
    try 
    {
        char data[128];
        while (true) 
        {
            boost::system::error_code ec;
            size_t len = socket->read_some(boost::asio::buffer(data), ec);
            if (ec) break;
            std::string message = std::string(data, len);
            if (!message.empty() && message.back() == '\n') 
            {
                message.pop_back();
            }
            msgHandler->ServerToMSG(message); 
            Broadcast(msg, socket);
        }
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Exception in client thread: " << e.what() << "\n";
    }
    catch (...) 
    {
        std::cerr << "Unknown exception in client thread\n";
    }

    RemoveClient(socket);
}


void ChatServer::Broadcast(const std::string& msg, std::shared_ptr<tcp::socket> sender) {
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (auto& client : clients_) {
        if (client != sender) {
            boost::system::error_code ec;
            boost::asio::write(*client, boost::asio::buffer(msg), ec);
        }
    }
}


void ChatServer::RemoveClient(std::shared_ptr<tcp::socket> socket) 
{
    std::lock_guard<std::mutex> lock(clients_mutex_);
    clients_.erase(std::remove(clients_.begin(), clients_.end(), socket), clients_.end());

    auto it = client_names_.find(socket);
    if (it != client_names_.end()) 
    {
        std::cout << "Client (" << it->second << ") disconnected\n";
        client_names_.erase(it);
    }
    else 
    {
        std::cout << "Client disconnected\n";
    }
}