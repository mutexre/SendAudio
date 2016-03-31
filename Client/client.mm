#include <stdio.h>
#include <array>
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;

int main(int argc, const char * argv[]) {
    try {
        io_service io;
        tcp::resolver resolver(io);
        tcp::resolver::query query("127.0.0.1", "8080");
        tcp::resolver::iterator endpoint_iter = resolver.resolve(query);
        tcp::socket socket(io);
        connect(socket, endpoint_iter);
        
        std::array<char, 128> buf;
        boost::system::error_code error;
        
        auto len = socket.read_some(buffer(buf), error);
        printf("received \"%s\" (%ld bytes)\n", buf.data(), len);
        
        socket.write_some(buffer("pong\0"), error);
        
        while (true) {
            int value;
            auto len = socket.read_some(buffer(&value, sizeof(value)), error);
            printf("%u\n", value);
        }
    }
    catch (std::exception& e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}
