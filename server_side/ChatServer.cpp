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
        AcceptConnections(); // Accept the next connection
        });
}

void ChatServer::HandleClient(std::shared_ptr<tcp::socket> socket, int client_id) {
    try {
        std::string username;
        boost::asio::streambuf buf;
        boost::asio::read_until(*socket, buf, '\n');
        std::istream is(&buf);
        std::getline(is, username);

        {
            std::lock_guard<std::mutex> lock(clients_mutex_);
            client_names_[socket] = username;
        }

        bool valid_username_check = SendSignalToFastAPI(username);
        std::string log_message;
        if (valid_username_check) {
            log_message = "Welcome, " + username + "\n";
        }
        else {
            log_message = "Username " + username + " does not exist\n";
        }
        boost::asio::write(*socket, boost::asio::buffer(log_message));

        char data[128];
        while (true) {
            boost::system::error_code ec;
            size_t len = socket->read_some(boost::asio::buffer(data), ec);
            if (ec) break;
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
    std::lock_guard<std::mutex> lock(clients_mutex_);
    for (auto& client : clients_) {
        if (client != sender) {
            boost::system::error_code ec;
            boost::asio::write(*client, boost::asio::buffer(msg), ec);
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
    clients_.erase(std::remove(clients_.begin(), clients_.end(), socket), clients_.end());

    auto it = client_names_.find(socket);
    if (it != client_names_.end()) {
        std::cout << "Client (" << it->second << ") disconnected\n";
        client_names_.erase(it);
    }
    else {
        std::cout << "Client disconnected\n";
    }
}
