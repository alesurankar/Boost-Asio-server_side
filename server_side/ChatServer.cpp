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
                    std::lock_guard<std::mutex> lock(mtx);
                    clients.push_back(socket);
                    std::cout << "Client " << client_id << " trying to connect\n";
                }

                std::thread(&ChatServer::HandleClient, this, socket, client_id).detach();
                std::thread(&ChatServer::SendToClient, this, socket, client_id).detach();
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

            client_names[socket] = username;

            std::string log_message = "Welcome, " + username + "\n";
            std::cout << username << " joined the server\n";
            asio::async_write(*socket, asio::buffer(log_message));
            break;
        }
        char data[128];
        while (socket->is_open()) 
        {
            system::error_code ec;
            size_t len = socket->read_some(asio::buffer(data), ec);

            if (ec) 
            {
                std::cerr << "Error reading from client: " << ec.message() << "\n";
                break;
            }

            std::string message(data, len);
            msg.ServerToMSG(message);
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


void ChatServer::SendToClient(std::shared_ptr<tcp::socket> socket, int client_id)
{
    try
    {
        while (socket->is_open())
        {
            std::optional<std::pair<int, int>> pos = msg.MSGToServer();
            
            if (pos)
            {
                asio::write(*socket, asio::buffer(&pos, sizeof(pos)));
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception in SendToClient: " << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Unknown exception in SendToClient\n";
    }
}


void ChatServer::RemoveClient(std::shared_ptr<tcp::socket> socket) 
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = client_names.find(socket);
    if (it != client_names.end()) 
    {
        std::string username = it->second;

        std::cout << "Client (" << username << ") disconnected\n";
        client_names.erase(it);
    }
    else 
    {
        std::cout << "Client disconnected\n";
    }
    clients.erase(std::remove(clients.begin(), clients.end(), socket), clients.end());
}