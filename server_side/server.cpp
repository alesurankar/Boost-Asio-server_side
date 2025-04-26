#include <boost/asio.hpp>
#include <boost/beast.hpp>  // <-- Add Boost.Beast header
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <map>

using boost::asio::ip::tcp; 

std::atomic<int> client_counter{ 0 };
std::vector<std::shared_ptr<tcp::socket>> clients;
std::mutex clients_mutex;
std::map<std::shared_ptr<tcp::socket>, std::string> client_names;

// 3: Function to broadcast a message to all clients except the sender
void broadcast(const std::string& msg, std::shared_ptr<tcp::socket> sender) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto& client : clients) {
        if (client != sender) {
            boost::system::error_code ec;
            boost::asio::write(*client, boost::asio::buffer(msg), ec);
        }
    }
}

int main() {
    // Adding a simple use of Boost.Beast's HTTP functionality
    boost::beast::http::request<boost::beast::http::string_body> req{ boost::beast::http::verb::get, "/", 11 };
    req.set(boost::beast::http::field::host, "localhost");
    req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    std::cout << "Using Boost.Beast version: " << BOOST_BEAST_VERSION_STRING << std::endl;

    boost::asio::io_context io;
    tcp::acceptor acceptor(io, tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 1234));

    while (true) {
        auto socket = std::make_shared<tcp::socket>(io);
        acceptor.accept(*socket);
        int client_id = client_counter++;

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(socket);
        }

        // Start a new thread to handle the connected client
        std::thread([socket, client_id]() mutable {
            try {
                // Authentication step (expecting username)
                std::string username;
                boost::asio::read_until(*socket, boost::asio::dynamic_buffer(username), '\n');
                username = username.substr(0, username.find('\n')); // Clean username
                std::string welcome = "Welcome, " + username + "\n";
                boost::asio::write(*socket, boost::asio::buffer(welcome));

                // Store the username
                client_names[socket] = username;

                char data[128];
                while (true) {
                    boost::system::error_code ec;
                    size_t len = socket->read_some(boost::asio::buffer(data), ec);
                    if (ec) break;
                    std::string msg = username + ": " + std::string(data, len) + "\n";
                    std::cout << msg;
                    broadcast(msg, socket);
                }
            }
            catch (...) {}

            // Remove this client from the list on disconnect
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.erase(std::remove(clients.begin(), clients.end(), socket), clients.end());
            client_names.erase(socket);
            std::cout << "Client " << client_id << " disconnected\n";
        }).detach();
    }
}