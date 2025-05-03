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
    std::cout << "ChatServer::Join: " << session << "\n";
    sessions_.insert(session);
    std::cout << "--------------\n";
}


void ChatServer::Leave(std::shared_ptr<ChatSession> session)
{
    std::cout << "ChatServer::Leave: " << session << "\n";
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


void ChatSession::ReadMessage() //4. Server(TCP)
{
    std::cout << "ChatSession::ReadMessage: //4. Server(TCP)\n";
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

                msgHandler->ServerToMSG(msg); //5. MSGServer(middleman)
            }
            else 
            {
                std::cerr << "Client disconnected\n";
                if (auto server = server_.lock())
                {
                    server->Leave(shared_from_this());
                }
            }
            ReadMessage();
        });
    std::cout << "--------------\n";
}


void ChatSession::CheckAndSendMessage(std::pair<int, int> pos) //8. Server(TCP)
{
    std::cout << "ChatSession::WriteMessage: //8. Server(TCP)\n";
    auto self = shared_from_this();
    msgHandler->MSGToServer();
    std::string message = std::to_string(pos.first) + "," + std::to_string(pos.second);

    boost::asio::async_write(socket_, boost::asio::buffer(message + "\n"),
        [this, self](boost::system::error_code ec, std::size_t)
        {
            if (ec)
            {
                std::cerr << "Send error: " << ec.message() << "\n";
            }
        });
    //std::cout << "--------------\n";
}
