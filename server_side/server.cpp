#include <boost/asio.hpp>     // Include Boost.Asio for networking
#include <iostream>           // For std::cout
#include <thread>             // For std::thread

int main() {
    using namespace boost::asio;
    io_context io;  // Main I/O context that handles all async operations

    // Create a TCP acceptor that listens on IP 127.0.0.1 and port 1234
    ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(ip::make_address("127.0.0.1"), 1234));

    while (true) {
        ip::tcp::socket socket(io); // Create a socket for the next incoming connection
        acceptor.accept(socket);    // Wait for a client to connect (blocking call)

        // Start a new thread to handle the connected client
        std::thread([s = std::move(socket)]() mutable {
            char data[128];                             // Buffer to hold incoming data
            boost::system::error_code ec;               // Error code object for handling failures
            size_t len = s.read_some(buffer(data), ec); // Read data from the client
            if (!ec) {      // Check if read was successful
                std::string msg(data, len);                 // Convert received data into a string
                std::cout << "Received: " << msg << "\n";   // Print the message to the server console
                
                // Send a response back to the client with "Echo: " prefix
                write(s, buffer("Echo: " + msg));
            }
            }).detach(); // Detach the thread so it runs independently
    }
}