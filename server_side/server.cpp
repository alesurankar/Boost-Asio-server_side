#include <boost/asio.hpp>     // Include Boost.Asio for networking
#include <iostream>           // For std::cout
#include <thread>             // For std::thread
#include <atomic>
#include <mutex>              // 3: Needed for std::mutex to protect shared client list
#include <vector>             // 3: To store all connected clients

using boost::asio::ip::tcp; 

std::atomic<int> client_counter{ 0 }; // 2.Global counter for assigning unique IDs
std::vector<std::shared_ptr<tcp::socket>> clients;  // 3: Store all connected clients
std::mutex clients_mutex;                           // 3: Mutex to protect the clients vector

// 3: Function to broadcast a message to all clients except the sender
void broadcast(const std::string& msg, std::shared_ptr<tcp::socket> sender) {
    std::lock_guard<std::mutex> lock(clients_mutex); // Lock to safely access clients list
    for (auto& client : clients) {
        if (client != sender) { // Don't send the message back to the sender
            boost::system::error_code ec;
            boost::asio::write(*client, boost::asio::buffer(msg), ec); // Send message to this client
        }
    }
}

int main() {
    boost::asio::io_context io;

    // 1.Create a TCP acceptor that listens on IP 127.0.0.1 and port 1234
    tcp::acceptor acceptor(io, tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 1234));

    while (true) {
        auto socket = std::make_shared<tcp::socket>(io); // 3: use shared_ptr to share socket
        acceptor.accept(*socket);    // 1.Wait for a client to connect (blocking call)

        int client_id = client_counter++; // 2.Assign a new client ID

        {
            std::lock_guard<std::mutex> lock(clients_mutex); // NEW: Lock to modify shared clients list
            clients.push_back(socket);                        // NEW: Add this socket to the list
        }

        // 1.Start a new thread to handle the connected client
        std::thread([socket, client_id]() mutable {
            try {
                std::string welcome = "You are client #" + std::to_string(client_id) + "\n";
                boost::asio::write(*socket, boost::asio::buffer(welcome));      // 2.Send the client their ID

                char data[128];                             // 1.Buffer to hold incoming data
                while (true) { // 3: loop to keep reading messages from the client
                    boost::system::error_code ec;               // 1.Error code object for handling failures
                    size_t len = socket->read_some(boost::asio::buffer(data), ec); // 1.Read data from the client
                    if (ec) break; // 3. Exit loop if client disconnects
                    std::string msg = "Client #" + std::to_string(client_id) + ": " + std::string(data, len);
                    msg += "\n";
                    std::cout << msg;                     // 3. Log message on server
                    broadcast(msg, socket);               // 3: Send message to all other clients
                }
            }
            catch (...) {}

                // 3: Remove this client from the list on disconnect
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.erase(std::remove(clients.begin(), clients.end(), socket), clients.end());
                std::cout << "Client #" << client_id << " disconnected\n";
            }).detach(); // Detach thread to handle client independently
    }
}