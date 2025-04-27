#include "ChatServer.h"

using namespace boost;

ChatServer::ChatServer(asio::io_context& io_in, const std::string& address_in, unsigned short port_in)
    : 
    io(io_in),
    acceptor(io_in, tcp::endpoint(asio::ip::make_address(address_in), port_in))
{}

void ChatServer::Start() 
{
    AcceptConnections();
}

void ChatServer::AcceptConnections() 
{
    auto socket = std::make_shared<tcp::socket>(io);
    acceptor.async_accept(*socket, [this, socket](system::error_code ec)
        {
            if (!ec)
            {
                int client_id = client_counter++;
                {
                    std::lock_guard<std::mutex> lock(clients_mutex);
                    clients.push_back(socket);
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
        std::string username;
        asio::streambuf buf;

        while (true)
        {
            asio::read_until(*socket, buf, '\n');
            std::istream is(&buf);
            std::getline(is, username);

            valid_username_check = SendSignalToFastAPI(username);
            if (valid_username_check) 
            {
                {
                    std::lock_guard<std::mutex> lock(clients_mutex);
                    client_names[socket] = username;
                }

                std::string log_message = "Welcome, " + username + "\n";
                std::cout << username << "joined the server\n";
                asio::write(*socket, asio::buffer(log_message));

                std::string join_message = username + " has joined the game!\n";
                Broadcast(join_message, socket);
                break;
            }
            else 
            {
                std::string error_message = "Username " + username + " does not exist. Try again: ";
                asio::write(*socket, asio::buffer(error_message));

                buf.consume(buf.size());
            }
        }
        char data[128];
        while (socket->is_open()) {
            system::error_code ec;
            size_t len = socket->read_some(asio::buffer(data), ec);
            if (ec) {
                std::cerr << "Error reading from client: " << ec.message() << "\n";
                break;
            }
            std::string msg = username + ": " + std::string(data, len) + "\n";
            std::cout << msg;
            Broadcast(msg, socket);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Exception in client thread: " << e.what() << "\n";
    }
    catch (...) {
        std::cerr << "Unknown exception in client thread\n";
    }

    RemoveClient(socket);
}


void ChatServer::Broadcast(const std::string& msg, std::shared_ptr<tcp::socket> sender)
{
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto& client : clients)
    {
        if (client != sender && client->is_open())
        {
            auto message = std::make_shared<std::string>(msg);
            auto buffer = asio::buffer(*message);

            asio::async_write(*client, buffer,
                [this, message, client](const system::error_code& ec, std::size_t bytes_transferred) 
                {
                    if (ec) 
                    {
                        std::cerr << "Failed to send message to client: " << ec.message() << "\n";
                    }
                    else 
                    {
                        std::cout << "Successfully sent " << bytes_transferred << " bytes to client.\n";
                    }
                });
        }
    }
}



bool ChatServer::SendSignalToFastAPI(const std::string& username) 
{
    try 
    {
        asio::io_context ioc;
        asio::ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("127.0.0.1", "8000");

        asio::ip::tcp::socket fastapi_socket(ioc);
        asio::connect(fastapi_socket, results.begin(), results.end());

        std::string url = "/check-users/" + username;
        beast::http::request<beast::http::string_body> req{ beast::http::verb::get, url, 11 };
        req.set(beast::http::field::host, "localhost");
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        beast::http::write(fastapi_socket, req);

        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(fastapi_socket, buffer, res);

        std::string fastapi_response = res.body();
        json::value json = json::parse(fastapi_response);

        fastapi_socket.close();

        return json.at("exists").as_bool();
    }
    catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

void ChatServer::RemoveClient(std::shared_ptr<tcp::socket> socket) 
{
    std::lock_guard<std::mutex> lock(clients_mutex);
    auto it = client_names.find(socket);
    if (it != client_names.end()) 
    {
        std::string username = it->second;

        std::cout << "Client (" << username << ") disconnected\n";

        std::string leave_message = username + " has left the game!\n";
        Broadcast(leave_message, socket);

        client_names.erase(it);
    }
    else 
    {
        std::cout << "Client disconnected\n";
    }
    clients.erase(std::remove(clients.begin(), clients.end(), socket), clients.end());
}