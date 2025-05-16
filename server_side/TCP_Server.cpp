#include "TCP_Server.h"
#include <iostream>



//namespace beast = boost::beast;
//namespace http = beast::http;



TCP_Server::TCP_Server(boost::asio::io_context& io_context, const std::string& address, short port, std::shared_ptr<MessageHandler> msgHandler_in)
    :
    server_acceptor(io_context, tcp::endpoint(boost::asio::ip::make_address(address), port)),
    msgHandler(msgHandler_in)
{
    Accept();
}


////bool TCP_Server::Running()
////{
////    return is_running;
////}


void TCP_Server::Join(std::shared_ptr<TCP_Session> client_session)
{
    std::cout << "TCP_Server::Join: " << client_session << "\n";      
    active_sessions.insert(client_session);
    std::cout << "--------------\n";
}


////void TCP_Server::RemoveClient(std::shared_ptr<tcp::socket> socket)                                  // CHECK
////{                                                                                                   // CHECK
////    std::lock_guard<std::mutex> lock(clients_mutex);                                                // CHECK
////    clients.erase(std::remove(clients.begin(), clients.end(), socket), clients.end());              // CHECK
////                                                                                                    // CHECK
////    auto it = client_names.find(socket);                                                            // CHECK
////    if (it != client_names.end())                                                                   // CHECK
////    {                                                                                               // CHECK
////        std::cout << "Client (" << it->second << ") disconnected\n";                                // CHECK
////        client_names.erase(it);                                                                     // CHECK
////    }                                                                                               // CHECK
////    else                                                                                            // CHECK
////    {                                                                                               // CHECK
////        std::cout << "Client disconnected\n";                                                       // CHECK
////    }                                                                                               // CHECK
////}                                                                                                   // CHECK

void TCP_Server::Leave(std::shared_ptr<TCP_Session> client_session)
{
    std::cout << "TCP_Server::Leave: " << client_session << "\n"; 
    /*client_session.cancel();
    client_session.shutdown(asio::ip::tcp::socket::shutdown_both);
    client_session.close();*/
    active_sessions.erase(client_session);
    std::cout << "--------------\n\n";
}


void TCP_Server::Accept()
{
    std::cout << "TCP_Server::Accept:\n";
    server_acceptor.async_accept([this](boost::system::error_code ec, tcp::socket socket)
        {
            if (!ec)
            {
                int client_id = client_counter++;
                std::cout << "Client " << client_id << " trying to connect\n";
                auto session = std::make_shared<TCP_Session>(client_id, std::move(socket), shared_from_this(), msgHandler);
                Join(session);
                session->Start();
            }
            else
            {
                std::cerr << "Accept failed: " << ec.message() << "\n";
            }
            Accept();
        });
    std::cout << "--------------\n\n";
}






TCP_Session::TCP_Session(int client_id_in, tcp::socket socket_in, std::weak_ptr<TCP_Server> server_in, std::shared_ptr<MessageHandler> msgHandler_in)
    :
    client_id(client_id_in),
    client_socket(std::make_shared<tcp::socket>(std::move(socket_in))),
    chat_server(server_in),
    msgHandler(msgHandler_in),
    timer(socket_in.get_executor())
{}


void TCP_Session::Start()
{
    auto self = shared_from_this(); 
    std::cout << "Start called on session: " << self.get() << "\n";

    std::cout << "TCP_Session::Start:\n";
    HandleClient(client_socket, client_id); 
    boost::asio::post(client_socket->get_executor(), [this, self]()
        {
            CheckAndSend();
        });
    std::cout << "--------------\n";
}


void TCP_Session::HandleClient(std::shared_ptr<tcp::socket> socket, int client_id)
{                                                                                       
    std::cout << "void TCP_Session::HandleClient(std::shared_ptr<tcp::socket> socket, int client_id): Client ID: " << client_id << "\n";
    //std::string username;                                                               // NEW
    //boost::asio::streambuf buf;                                                         // NEW
    //boost::asio::read_until(*socket, buf, '\n');                                        // NEW
    //std::istream is(&buf);                                                              // NEW
    //std::getline(is, username);                                                         // NEW
    
    //{                                                                                   // NEW
    //    std::lock_guard<std::mutex> lock(clients_mutex);                                // NEW
    //    client_names[socket] = username;                                                // NEW
    //}                                                                                   // NEW
    //                                                                                    // NEW
    //bool valid_username_check = SendSignalToFastAPI(username);                          // NEW
    //std::string log_message;                                                            // NEW
    //if (valid_username_check)                                                           // NEW
    //{                                                                                   // NEW
    //    log_message = "Welcome, " + username + "\n";                                    // NEW
    //    boost::asio::write(*socket, boost::asio::buffer(log_message));                  // NEW
        ReadMessage();                                                                  
    //}                                                                                   // NEW
    //else                                                                                // NEW
    //{                                                                                   // NEW
    //    log_message = "Username " + username + " does not exist\n";                     // NEW
    //    boost::asio::write(*socket, boost::asio::buffer(log_message));                  // NEW
    //    RemoveClient(socket);                                                           // NEW
    //}                                                                                   // NEW                  
}                                                                                         // NEW 

void TCP_Session::ReadMessage()
{
    //float dt = ft.Mark();
    //float dtMs = dt * 1000.0f;
    //std::cout << "void TCP_Session::ReadMessage(): Frame Time: " << dtMs << " ms\n";
    auto self = shared_from_this();
    std::cout << "void TCP_Session::ReadMessage():\n";
    boost::asio::async_read_until(*client_socket, input_buffer, '\n',
        [this, self](boost::system::error_code ec, std::size_t length) 
        {
            if (!ec) 
            {
                std::istream is(&input_buffer);
                std::string msg;
                std::getline(is, msg);

                std::cout << "Step 4, TCP_Session::ReadMessage::Received: " << msg << "\n";

                if (!msg.empty())
                {
                    msgHandler->ServerToMSG(msg);
                }
            }
            else 
            {
                std::cerr << "Client disconnected\n";
                if (auto server = chat_server.lock())
                {
                    server->Leave(shared_from_this());
                }
                client_socket->close(); 
                return;
            }
            ReadMessage();
        });
    std::cout << "Step 4--------------\n";
}


void TCP_Session::CheckAndSend()
{
    //float dt = ft.Mark();
    //float dtMs = dt * 1000.0f;
    //std::cout << "void App::UpdateLoop(): Frame Time: " << dtMs << " ms\n";
     
    auto self = shared_from_this();
    std::cout << "void TCP_Session::CheckAndSend(): Client ID: " << client_id << ", Client Session: " << self.get() << "\n";
    timer.expires_after(std::chrono::milliseconds(8));
    timer.async_wait([this, self](boost::system::error_code ec)
        {
            msg = msgHandler->MSGToServer();

            if (!client_socket->is_open())
            {
                std::cerr << "Socket is not open.\n";
                return;
            }

            boost::asio::async_write(*client_socket, boost::asio::buffer(msg),
                [this, self](const boost::system::error_code& ec, std::size_t)
                {
                    if (!ec)
                    {
                        //std::lock_guard<std::mutex> lock(clients_mutex);                          // NEW
                        //for (auto& client : clients)                                              // NEW
                        //{                                                                         // NEW
                        //    if (client != sender)                                                 // NEW
                        //    {                                                                     // NEW
                        //        boost::system::error_code ec;                                     // NEW
                        //        boost::asio::write(*client, boost::asio::buffer(msg), ec);        // NEW
                        //    }                                                                     // NEW
                        //}                                                                         // NEW
            
                        std::cout << "Step 11: TCP_Session::CheckAndSend: " << msg;
                    }
                    else
                    {
                        std::cerr << "Send error: " << ec.message() << "\n";
                    //    if (auto server = chat_server.lock())
                    //    {
                    //        server->Leave(shared_from_this());
                    //    }
                    //    client_socket->close();
                        return;
                    }
                    //boost::asio::post(client_socket->get_executor(), [this, self]()
                    //    {
                    //        CheckAndSend();
                    //    });
                });
        });
    std::cout << "Step 11--------------\n";
}


////bool TCP_Session::SendSignalToFastAPI(const std::string& username)
////{
////    boost::asio::io_context ioc;
////    boost::asio::ip::tcp::resolver resolver(ioc);
////    auto const results = resolver.resolve("127.0.0.1", "8000");
////
////    boost::asio::ip::tcp::socket fastapi_socket(ioc);
////    boost::asio::connect(fastapi_socket, results.begin(), results.end());
////
////    std::string url = "/check-users/" + username;
////    http::request<http::string_body> req{ http::verb::get, url, 11 };
////    req.set(http::field::host, "localhost");
////    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
////
////    http::write(fastapi_socket, req);
////
////    beast::flat_buffer buffer;
////    http::response<http::string_body> res;
////    http::read(fastapi_socket, buffer, res);
////
////    std::string fastapi_response = res.body();
////    boost::json::value json = boost::json::parse(fastapi_response);
////
////    fastapi_socket.close();
////
////    return json.at("exists").as_bool();
////}