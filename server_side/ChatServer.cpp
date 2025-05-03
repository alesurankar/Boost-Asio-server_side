#include "ChatServer.h"
#include <iostream>


ChatServer::ChatServer(boost::asio::io_context& io_context, short port, std::shared_ptr<MessageHandler> handler)
    :
    acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
    msgHandler(handler)
{
    Accept();
}


bool ChatServer::Running()
{
    //std::cout << "ChatServer::Running:\n";
    return is_running;
}


void ChatServer::Join(std::shared_ptr<ChatSession> session)
{
    std::cout << "ChatServer::Join:\n";
    sessions_.insert(session);
    std::cout << "--------------\n";
}


void ChatServer::Leave(std::shared_ptr<ChatSession> session)
{
    std::cout << "ChatServer::Leave:\n";
    sessions_.erase(session);
    std::cout << "--------------\n";
}


void ChatServer::Accept()
{
    std::cout << "ChatServer::Accept:\n";
    acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket) 
        {
        if (!ec) 
        {
            auto session = std::make_shared<ChatSession>(std::move(socket), shared_from_this(), msgHandler);
            Join(session);
            session->Start();
        }
        Accept();
        });
    std::cout << "--------------\n";
}



ChatSession::ChatSession(tcp::socket socket, std::weak_ptr<ChatServer> server, std::shared_ptr<MessageHandler> handler)
    :
    socket_(std::move(socket)), 
    server_(server),
    msgHandler(handler),
    message_timer(socket_.get_executor())
{}


void ChatSession::Start()
{
    std::cout << "ChatSession::Start:\n";
    ReadMessage();
    CheckAndSendMessage();
    std::cout << "--------------\n";
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

                //buffer_.consume(length);

                std::cout << "Received: " << msg << "\n";

                msgHandler->ServerToMSG(msg);

                ReadMessage();
            }
            else 
            {
                std::cerr << "Client disconnected\n";
                if (auto server = server_.lock())
                {
                    server->Leave(shared_from_this());
                }
            }
        });
    std::cout << "--------------\n";
}


void ChatSession::CheckAndSendMessage()
{
    //std::cout << "ChatSession::WriteMessage:\n";
    auto self = shared_from_this();

    auto messageOpt = msgHandler->MSGToServer();
    if (messageOpt.has_value())
    {
        auto [x, y] = messageOpt.value();
        std::string message = std::to_string(x) + "," + std::to_string(y);

        boost::asio::async_write(socket_, boost::asio::buffer(message + "\n"),
            [this, self](boost::system::error_code ec, std::size_t)
            {
                if (ec)
                {
                    std::cerr << "Send error: " << ec.message() << "\n";
                    return;
                }
                CheckAndSendMessage();
            });
    }
    else
    {
        message_timer.expires_after(std::chrono::milliseconds(100));
        message_timer.async_wait([this, self](boost::system::error_code ec)
            {
                if (!ec)
                {
                    CheckAndSendMessage();
                }
            });
    }
    //std::cout << "--------------\n";
}
