#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <map>

using boost::asio::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;

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

// Function to send a basic signal to FastAPI server (GET request)
bool send_signal_to_fastapi(const std::string& username) {
    try {
        boost::asio::io_context ioc;

        // Resolver to convert the server address and port
        boost::asio::ip::tcp::resolver resolver(ioc);

        // FastAPI server address and port
        auto const results = resolver.resolve("127.0.0.1", "8000");

        // Create the socket
        boost::asio::ip::tcp::socket fastapi_socket(ioc);

        // Connect to the FastAPI server
        boost::asio::connect(fastapi_socket, results.begin(), results.end());

        // Create the HTTP GET request
        std::string url = "/check-users/" + username;  // use username as table_name
        http::request<http::string_body> req{ http::verb::get, url, 11 };
        req.set(http::field::host, "localhost");
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the request to the server
        http::write(fastapi_socket, req);

        // Prepare to read the response
        beast::flat_buffer buffer;
        http::response<http::string_body> res;

        // Receive the response
        http::read(fastapi_socket, buffer, res);

        // Output the response
        //std::cout << "Response from FastAPI: " << res << std::endl;

        // Parse the FastAPI JSON response
        std::string fastapi_response = res.body();
        boost::json::value json = boost::json::parse(fastapi_response);

        // Form the response to be sent to the client (echoing back the JSON data)
        std::string response_to_client = "Server: " + fastapi_response + "\n";

        // Send the FastAPI response back to the client
        //boost::asio::write(fastapi_socket, boost::asio::buffer(response_to_client));

        // Gracefully close the connection
        fastapi_socket.close();

        bool exists = json.at("exists").as_bool();
        return exists;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

int main() {
    boost::asio::io_context io;
    tcp::acceptor acceptor(io, tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 1234));

    while (true) {
        auto socket = std::make_shared<tcp::socket>(io);
        acceptor.accept(*socket);
        int client_id = client_counter++;

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(socket); 
            std::cout << "Client " << client_id << " trying to connect\n";
        }

        // Start a new thread to handle the connected client
        std::thread([socket, client_id]() mutable {
            try {
                std::string username;
                boost::asio::streambuf buf;
                boost::asio::read_until(*socket, buf, '\n');
                std::istream is(&buf);
                std::getline(is, username);

                client_names[socket] = username;

                // Send the username to FastAPI for table existence check
                std::string log_message;

                bool valid_username_check = send_signal_to_fastapi(username);
                if (valid_username_check == true)
                {
                    log_message = "Welcome, " + username + "\n";
                }
                else
                {
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
                    broadcast(msg, socket);
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Exception in client thread: " << e.what() << "\n";
            }
            catch (...) {
                std::cerr << "Unknown exception in client thread\n";
            }

            // Remove this client from the list on disconnect
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.erase(std::remove(clients.begin(), clients.end(), socket), clients.end());
            client_names.erase(socket);
            auto it = client_names.find(socket);
            if (it != client_names.end()) {
                std::cout << "Client " << client_id << " (" << it->second << ") disconnected\n";
            }
            else {
                std::cout << "Client " << client_id << " disconnected\n";
            }
        }).detach();
    }
}