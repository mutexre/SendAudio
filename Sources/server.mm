#import <stdio.h>
#import <string>
#import <vector>
#import <thread>
#import <boost/asio.hpp>
#import <CoreAudio/CoreAudio.h>
#import <Novocaine/Novocaine.h>
#import <boost/lockfree/spsc_queue.hpp>
#import <opus.h>
#import "shared.h"

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

bool handshake(tcp::socket& socket) {
    boost::system::error_code error;
    size_t len = socket.write_some(buffer(GREETING), error);
    
    array<char, 128> buf;
    len = socket.read_some(buffer(buf), error);
    printf("received \"%s\" (%ld bytes)\n", buf.data(), len);
    
    return true;
}

void transmit(const shared_ptr<RingBuffer>& ringbuffer, tcp::socket& socket) {
    thread t([&ringbuffer, &socket]() {
        boost::system::error_code error;
        AudioBuf buf(2048);
        do {
            if (ringbuffer->pop(buf)) {
                size_t len = socket.write_some(buffer(buf), error);
            }
            chrono::nanoseconds ns(1000000000 / (44100 / 256));
            this_thread::sleep_for(ns);
        }
        while (!error.value());
    });
    t.join();
}

void acceptClient(tcp::acceptor& acceptor, const shared_ptr<RingBuffer>& ringbuffer, bool& anyClient) {
    printf("waiting for client...\n");
    
    tcp::socket socket(acceptor.get_io_service());
    acceptor.accept(socket);
    
    if (handshake(socket)) {
        anyClient = true;
        transmit(ringbuffer, socket);
    }
}

struct AudioCallbackData {
    RingBuffer* ringbuffer;
    unsigned* clientsCount;
};

OSStatus audioCallback(AudioObjectID          inDevice,
                       const AudioTimeStamp*  inNow,
                       const AudioBufferList* inInputData,
                       const AudioTimeStamp*  inInputTime,
                       AudioBufferList*       outOutputData,
                       const AudioTimeStamp*  inOutputTime,
                       AudioCallbackData*     callbackData)
{
    unsigned clientCount = *(callbackData->clientsCount);
    if (clientCount > 0) {
        RingBuffer* ringbuffer = callbackData->ringbuffer;
        for (int i = 0; i < inInputData->mNumberBuffers; i++) {
            const AudioBuffer& buf = inInputData->mBuffers[i];
            
            //AudioBuf buf(samplesCount);
            //memcpy(buf.data(), data, samplesCount * sizeof(float));
            //callbackData->ringbuffer->push(buf);
        }
    }
    return kAudioHardwareNoError;
}

/*void startAudio(AudioDeviceID dev, AudioDeviceIOProcID callback)
{
    OSStatus status;
    AudioObjectPropertyAddress propertyAddr;
    UInt32 propertySize, bufferByteCount;
    Float64 nominalSampleRate;

    printf("startAudio\n");

    // Get the output device
    propertyAddr.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
    propertyAddr.mScope = kAudioObjectPropertyScopeGlobal;
    propertyAddr.mElement = kAudioObjectPropertyElementMaster;
    propertySize = sizeof(dev);
    status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddr, 0, 0, &propertySize, &dev);
    if (status)
    {
        printf("AudioObjectGetPropertyData returned %d\n", status);
        return;
    }

    if (dev == kAudioDeviceUnknown)
    {
        printf("AudioObjectGetPropertyData: outputDeviceID is kAudioDeviceUnknown\n");
        return;
    }

    // Configure the output device	
    propertyAddr.mSelector = kAudioDevicePropertyBufferSize;
    propertyAddr.mScope = kAudioDevicePropertyScopeOutput;
    propertyAddr.mElement = kAudioObjectPropertyElementMaster;
    propertySize = sizeof(bufferByteCount);
    bufferByteCount = SAMPLES_PER_BUFFER * sizeof(float);
    status = AudioObjectSetPropertyData(gOutputDeviceID, &propertyAddr, 0, 0, propertySize, &bufferByteCount);
    if (status)
    {
        printf("AudioObjectSetPropertyData: returned %d when setting kAudioDevicePropertyBufferSize to %d\n", status, SAMPLES_PER_BUFFER);
        return;
    }

    propertyAddr.mSelector = kAudioDevicePropertyNominalSampleRate;
    propertyAddr.mScope = kAudioDevicePropertyScopeOutput;
    propertyAddr.mElement = kAudioObjectPropertyElementMaster;
    propertySize = sizeof(nominalSampleRate);
    nominalSampleRate = gSampleRate;
    status = AudioObjectSetPropertyData(gOutputDeviceID, &propertyAddr, 0, 0, propertySize, &nominalSampleRate);
    if (status)
    {
        printf("AudioObjectSetPropertyData: returned %d when setting kAudioDevicePropertyNominalSampleRate to %d\n", status, SAMPLES_PER_BUFFER);
        return;
    }

    propertyAddr.mSelector = kAudioDevicePropertyNominalSampleRate;
    propertyAddr.mScope = kAudioDevicePropertyScopeOutput;
    propertyAddr.mElement = kAudioObjectPropertyElementMaster;
    propertySize = sizeof(nominalSampleRate);
    status = AudioObjectGetPropertyData(gOutputDeviceID, &propertyAddr, 0, 0, &propertySize, &nominalSampleRate);
    if (status)
    {
        printf("AudioObjectSetPropertyData: returned %d when setting kAudioDevicePropertyNominalSampleRate to %d\n", status, SAMPLES_PER_BUFFER);
        return;
    }
    printf("nominalSampleRate=%f\n", nominalSampleRate);
    gSampleRate = nominalSampleRate;

    // Start sound running
    status = AudioDeviceCreateIOProcID(gOutputDeviceID, waveIOProc, nullptr, &gIOProcID);
    if (status)
    {
        printf("AudioDeviceAddIOProc: returned %d\n", status);
        return;
    }

    status = AudioDeviceStart(gOutputDeviceID, gIOProcID);
    if (status)
    {
        printf("AudioDeviceStart: returned %d\n", status);
        return;
    }
}*/

/*void stopAudio()
{
    OSStatus err;
    err = AudioDeviceStop(gOutputDeviceID, waveIOProc);
}*/

/*void startAudio(const shared_ptr<RingBuffer>& ringbuffer, const bool& anyClient) {
    Novocaine* audioManager = [Novocaine audioManager];
    
    unsigned i = 0;
    for (NSString* device in audioManager.deviceNames)
        printf("%u %s\n", i++, device.UTF8String);

    //for (AudioDeviceID device in audioManager.deviceIDs) {
      //  printf("%u\n", device);
    
    [audioManager enumerateAudioDevices];
    
    audioManager.inputBlock = ^(float* data, UInt32 numFrames, UInt32 numChannels) {
        if (anyClient) {
            size_t samplesCount = numFrames * numChannels;
            AudioBuf buf(samplesCount);
            memcpy(buf.data(), data, samplesCount * sizeof(float));
            ringbuffer->push(buf);
        }
    };
    
    [audioManager play];
}*/

int main(int argc, const char * argv[]) {
    auto ringbuffer = make_shared<RingBuffer>();
    bool anyClient = false;
    
    //startAudio(ringbuffer, anyClient);
    
    try {
        io_service io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 8080));
        while (true) acceptClient(acceptor, ringbuffer, anyClient);
    }
    catch (exception& e) {
        printf("error: %s\n", e.what());
    }
    
    return 0;
}
