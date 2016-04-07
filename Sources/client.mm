#import <stdio.h>
#import <array>
#import <boost/asio.hpp>
#import <Novocaine/Novocaine.h>
#import <boost/lockfree/spsc_queue.hpp>
#import "shared.h"

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

bool handshake(tcp::socket& socket) {
    std::array<char, 128> buf;
    boost::system::error_code error;
    
    size_t len = socket.read_some(buffer(buf), error);
    printf("received \"%s\" (%ld bytes)\n", buf.data(), len);
    
    len = socket.write_some(buffer("pong"), error);
    
    return true;
}

void startAudio(const shared_ptr<RingBuffer>& ringbuffer) {
    Novocaine* audioManager = [Novocaine audioManager];
    audioManager.outputBlock = ^(float* data, UInt32 numFrames, UInt32 numChannels) {
        size_t samplesCount = numFrames * numChannels;
        AudioBuf buf;
        ringbuffer->pop(buf);
        memcpy(data, buf.data(), samplesCount * sizeof(float));
    };
    [audioManager play];
}

int main(int argc, const char * argv[]) {
    try {
        io_service io;
        tcp::resolver resolver(io);
        tcp::resolver::query query("127.0.0.1", "8080");
        tcp::resolver::iterator endpoint_iter = resolver.resolve(query);
        tcp::socket socket(io);
        connect(socket, endpoint_iter);
        
        auto ringbuffer = make_shared<RingBuffer>();
        
        if (handshake(socket)) {
            startAudio(ringbuffer);
            boost::system::error_code error;
            do {
                AudioBuf buf(1024);
                size_t len = read(socket, buffer(buf), error);
                ringbuffer->push(buf);
                //printf("%ld %d\n", len, error.value());
            }
            while (!error.value());
        }
    }
    catch (std::exception& e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}
