// server.cpp
#include <boost/asio.hpp>
#include <iostream>

int main() {
    using namespace boost::asio;
    io_context io;

    ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(ip::make_address("127.0.0.1"), 1234));
    ip::tcp::socket socket(io);

    acceptor.accept(socket);

    char data[128];
    size_t len = socket.read_some(buffer(data));

    std::cout << "Received: " << std::string(data, len) << "\n";

    std::cin.get();
}
