#include <boost/asio.hpp>     // Include Boost.Asio for networking
#include <iostream>           // For std::cout
#include <thread>             // For std::thread

std::atomic<int> client_counter{ 0 }; // 2.Global counter for assigning unique IDs

int main() {
    using namespace boost::asio;
    io_context io;  // 1.Main I/O context that handles all async operations

    // 1.Create a TCP acceptor that listens on IP 127.0.0.1 and port 1234
    ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(ip::make_address("127.0.0.1"), 1234));

    while (true) {
        ip::tcp::socket socket(io); // 1.Create a socket for the next incoming connection
        acceptor.accept(socket);    // 1.Wait for a client to connect (blocking call)

        int client_id = client_counter++; // 2.Assign a new client ID

        // 1.Start a new thread to handle the connected client
        std::thread([s = std::move(socket), client_id]() mutable {
            try {
                std::string welcome = "You are client #" + std::to_string(client_id) + "\n";
                write(s, buffer(welcome));      // 2.Send the client their ID

                char data[128];                             // 1.Buffer to hold incoming data
                boost::system::error_code ec;               // 1.Error code object for handling failures
                size_t len = s.read_some(buffer(data), ec); // 1.Read data from the client
                if (!ec) {      // 1.Check if read was successful
                    std::string msg(data, len);                 // 1.Convert received data into a string
                    std::cout << "Client " << client_id << " says: " << msg << "\n";
                    write(s, buffer("Echo from server to client #" + std::to_string(client_id) + ": " + msg));
                }
            }
        catch (std::exception& e) {
            std::cerr << "Error with client " << client_id << ": " << e.what() << "\n";
        }
        }).detach(); // 1.Detach the thread so it runs independently
    }
}