#include "ChatServer.h"

int main() {
    boost::asio::io_context io;
    ChatServer server(io, "127.0.0.1", 1234);
    server.Start();
    io.run();

    return 0;
}
