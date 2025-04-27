#include "ChatServer.h"

namespace beast = boost::beast;
namespace http = beast::http;

ChatServer::ChatServer(boost::asio::io_context& io, const std::string& address, unsigned short port)
    : io_(io),
    acceptor_(io, tcp::endpoint(boost::asio::ip::make_address(address), port))
{
}

void ChatServer::Start() {
    AcceptConnections();
}

void ChatServer::AcceptConnections() {
    auto socket = std::make_shared<tcp::socket>(io_);
    acceptor_.async_accept(*socket, [this, socket](boost::system::error_code ec) {
        if (!ec) {
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

void ChatServer::HandleClient(std::shared_ptr<tcp::socket> socket, int client_id) {
    try {
        std::string username;
        boost::asio::streambuf buf;

        while (true)
        {
            boost::asio::read_until(*socket, buf, '\n');
            std::istream is(&buf);
            std::string username;
            std::getline(is, username);

            // Trim any extra spaces or newlines from the username
            username.erase(username.find_last_not_of(" \n\r\t") + 1);

            // Debug: Output the received username
            std::cout << "Received username: '" << username << "'\n";

            valid_username_check = SendSignalToFastAPI(username);
            if (valid_username_check) {
                // Lock mutex to modify shared resource (client_names_)
                {
                    std::lock_guard<std::mutex> lock(clients_mutex_);
                    client_names_[socket] = username;  // Store the client and its username
                }

                // Send welcome message to the client
                std::string log_message = "Welcome, " + username + "\n";
                std::cout << "Sending welcome message: " << log_message << std::endl;
                boost::asio::write(*socket, boost::asio::buffer(log_message));

                // Broadcast the join message to others
                std::string join_message = username + " has joined the game!\n";
                Broadcast(join_message, socket);

                // Exit the loop as username is valid and user can join the chat
                break;
            }
            else {
                // Send error message to the client if username doesn't exist
                std::string error_message = "Username " + username + " does not exist. Try again: ";
                std::cout << "Sending error message: " << error_message << std::endl;
                boost::asio::write(*socket, boost::asio::buffer(error_message));

                // Clear the buffer before reading the next input
                buf.consume(buf.size());
            }
        }
        char data[128];
        while (socket->is_open()) {
            boost::system::error_code ec;
            size_t len = socket->read_some(boost::asio::buffer(data), ec);
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


void ChatServer::Broadcast(const std::string& msg, std::shared_ptr<tcp::socket> sender) {
    for (auto& client : clients_) {
        if (client != sender && client->is_open()) {  // Only write if the socket is open
            boost::system::error_code ec;
            boost::asio::write(*client, boost::asio::buffer(msg), ec);
            if (ec) {
                std::cerr << "Failed to send message to a client: " << ec.message() << "\n";
            }
        }
    }
}


bool ChatServer::SendSignalToFastAPI(const std::string& username) {
    try {
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("127.0.0.1", "8000");

        boost::asio::ip::tcp::socket fastapi_socket(ioc);
        boost::asio::connect(fastapi_socket, results.begin(), results.end());

        std::string url = "/check-users/" + username;
        http::request<http::string_body> req{ http::verb::get, url, 11 };
        req.set(http::field::host, "localhost");
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(fastapi_socket, req);

        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(fastapi_socket, buffer, res);

        std::string fastapi_response = res.body();
        boost::json::value json = boost::json::parse(fastapi_response);

        fastapi_socket.close();

        return json.at("exists").as_bool();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

void ChatServer::RemoveClient(std::shared_ptr<tcp::socket> socket) {
    std::lock_guard<std::mutex> lock(clients_mutex_);

    auto it = client_names_.find(socket);
    if (it != client_names_.end()) {
        std::string username = it->second;

        std::cout << "Client (" << username << ") disconnected\n";

        // NEW: Broadcast to everyone that this player left
        std::string leave_message = username + " has left the game!\n";
        Broadcast(leave_message, socket); // send to others

        client_names_.erase(it);
    }
    else {
        std::cout << "Client disconnected\n";
    }

    // Safe removal of disconnected clients
    clients_.erase(std::remove(clients_.begin(), clients_.end(), socket), clients_.end());
}

void ChatServer::BroadcastMessage(const std::string& message) {
    for (auto& client : clients_) {
        if (client && client->is_open()) {  // Check if socket is still open
            boost::asio::async_write(*client, boost::asio::buffer(message + "\n"),
                [](boost::system::error_code ec, std::size_t /*length*/) {
                    if (ec) {
                        std::cout << "Failed to send message: " << ec.message() << "\n";
                    }
                }
            );
        }
    }
}
