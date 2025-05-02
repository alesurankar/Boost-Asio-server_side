#include "ChatServer.h"
#include <iostream>


ChatServer::ChatServer(boost::asio::io_context& io_context, short port)
    :
    acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{
    Accept();
}

bool ChatServer::Running()
{
    std::cout << "ChatServer::Running:\n";
    return is_running;
}

void ChatServer::Join(std::shared_ptr<ChatSession> session)
{
    std::cout << "ChatServer::Join:\n";
    sessions_.insert(session);
}

void ChatServer::Leave(std::shared_ptr<ChatSession> session)
{
    std::cout << "ChatServer::Leave:\n";
    sessions_.erase(session);
}

void ChatServer::Broadcast(const std::string& msg)
{
    std::cout << "ChatServer::Broadcast:\n";
    for (auto& s : sessions_) 
    {
        s->Deliver(msg);
    }
}

void ChatServer::Accept()
{
    std::cout << "ChatServer::Accept:\n";
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) 
        {
        if (!ec) 
        {
            auto session = std::make_shared<ChatSession>(std::move(socket), this);
            Join(session);
            session->Start();
        }
        Accept();
        });
}



ChatSession::ChatSession(tcp::socket socket, ChatServer* server)
    :
    socket_(std::move(socket)), server_(server)
{}

void ChatSession::Start()
{
    std::cout << "ChatSession::Start:\n";
    ReadMessage();
}

void ChatSession::Deliver(const std::string& msg)
{
    std::cout << "ChatSession::Deliver:\n";
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress) 
    {
        WriteMessage();
    }
}

void ChatSession::ReadMessage()
{
    std::cout << "ChatSession::ReadMessage:\n";
    auto self = shared_from_this();
    boost::asio::async_read_until(socket_, buffer_, '\n',
        [this, self](boost::system::error_code ec, std::size_t length) 
        {
            if (!ec) 
            {
                std::istream is(&buffer_);
                std::string msg;
                std::getline(is, msg);
                std::cout << "Received: " << msg << "\n";

                Broadcast(msg + "\n");

                ReadMessage();
            }
            else 
            {
                std::cerr << "Client disconnected\n";
                if (server_) 
                {
                    server_->Leave(shared_from_this());
                }
            }
        });
}

void ChatSession::WriteMessage()
{
    std::cout << "ChatSession::WriteMessage:\n";
    auto self = shared_from_this();
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/) 
        {
            if (!ec) 
            {
                write_msgs_.pop_front();
                if (!write_msgs_.empty()) 
                {
                    WriteMessage();
                }
            }
            else 
            {
                std::cerr << "Write error\n";
            }
        });
}

void ChatSession::Broadcast(const std::string& msg)
{
    std::cout << "ChatSession::Broadcast:\n";
    if (server_)
    {
        server_->Broadcast(msg);
    }
}