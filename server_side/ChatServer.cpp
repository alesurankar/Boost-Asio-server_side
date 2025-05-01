#include "ChatServer.h"


ChatServer::ChatServer(boost::asio::io_context& io_context, short port)
    :
    acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{
    Accept();
}

void ChatServer::Join(std::shared_ptr<ChatSession> session)
{
    sessions_.insert(session);
}

void ChatServer::Leave(std::shared_ptr<ChatSession> session)
{
    sessions_.erase(session);
}

void ChatServer::Broadcast(const std::string& msg)
{
    for (auto& s : sessions_) 
    {
        s->Deliver(msg);
    }
}

void ChatServer::Accept()
{
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) 
        {
        if (!ec) 
        {
            auto session = std::make_shared<ChatSession>(std::move(socket), this);
            Join(session);
            session->Start();
        }
        Accept(); // keep accepting
        });
}



ChatSession::ChatSession(tcp::socket socket, ChatServer* server)
    :
    socket_(std::move(socket)), server_(server)
{}

void ChatSession::Start()
{
    ReadMessage();
}

void ChatSession::Deliver(const std::string& msg)
{
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress) 
    {
        WriteMessage();
    }
}

void ChatSession::ReadMessage()
{
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

                ReadMessage(); // read next message
            }
            else {
                std::cerr << "Client disconnected\n";
                if (server_) {
                    server_->Leave(shared_from_this());
                }
            }
        });
}

void ChatSession::WriteMessage()
{
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
    if (server_)
    {
        server_->Broadcast(msg);
    }
}