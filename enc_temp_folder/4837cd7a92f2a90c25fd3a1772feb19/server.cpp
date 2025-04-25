// server.cpp
#include <boost/asio.hpp>
#include <iostream>
#include <thread>

int main() {
    using namespace boost::asio;
    io_context io;

    ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(ip::make_address("127.0.0.1"), 1234));

    while (true) {
        ip::tcp::socket socket(io);
        acceptor.accept(socket);

        std::thread([s = std::move(socket)]() mutable {
            char data[128];
            boost::system::error_code ec;
            size_t len = s.read_some(buffer(data), ec);
            if (!ec) {
                std::string msg(data, len);
                std::cout << "Received: " << msg << "\n";
                write(s, buffer("Echo: " + msg));
            }
            }).detach();
    }
}