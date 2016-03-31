#import <stdio.h>
#import <string>
#import <vector>
#import <thread>
#import <boost/asio.hpp>
#import <Novocaine/Novocaine.h>
#import <boost/lockfree/spsc_queue.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

#define GREETING "ping"
#define RESPONSE "pong"

using RingBuffer = boost::lockfree::spsc_queue<int, boost::lockfree::capacity<1024>>;

void acceptClient(tcp::acceptor& acceptor, RingBuffer& ringbuffer) {
    printf("waiting for client...\n");
    
    tcp::socket socket(acceptor.get_io_service());
    acceptor.accept(socket);
    
    boost::system::error_code error;
    write(socket, buffer(GREETING), error);
    
    array<char, 128> buf;
    size_t len = socket.read_some(buffer(buf), error);
    printf("received \"%s\" (%ld bytes)\n", buf.data(), len);
    
    thread t([&ringbuffer, &socket]() {
        while (true) {
            int value;
            ringbuffer.pop(value);
            printf("%u\n", value);
            
            boost::system::error_code error;
            socket.write_some(buffer(&value, sizeof(value)), error);
        };
    });
    t.join();
}

int main(int argc, const char * argv[]) {
    RingBuffer ringbuffer;
    auto rb_ptr = &ringbuffer;
    
    Novocaine* audioManager = [Novocaine audioManager];
    
    audioManager.inputBlock = ^(float* data, UInt32 numFrames, UInt32 numChannels) {
        static int c = 0;
        vector<float> buf(numFrames * numChannels);
        rb_ptr->push(c);
        c++;
    };
    [audioManager play];
    
    try {
        io_service io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 8080));
        acceptClient(acceptor, ringbuffer);
    }
    catch (exception& e) {
        printf("error: %s\n", e.what());
    }
    
    return 0;
}
