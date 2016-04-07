#include <vector>

#define GREETING "ping"
#define RESPONSE "pong"

using AudioBuf = std::vector<float>;
using RingBuffer = boost::lockfree::spsc_queue<AudioBuf, boost::lockfree::capacity<16>>;

/*struct AudioBuf
{
    unsigned channels, frames;
    std::vector<float> data;
};*/
