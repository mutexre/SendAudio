#include <stdio.h>
#include "Audio.hpp"
#define BOOST_TEST_MODULE AudioManagerTest
#include <boost/test/unit_test.hpp>

using namespace Audio;

void printAudioDeviceInfo(const Device::Info& info);
void printAudioStreamInfo(const Stream::Info& info, const char* prepend = "");
void printAudioStreamBasicDescription(const AudioStreamBasicDescription& d, const char* prepend = "");
void printAudioFormatFlags(AudioFormatFlags f);

void print4(unsigned x) {
    for (int shift = 24; shift >= 0; shift -= 8)
        printf("%c", (x & (0xff << shift)) >> shift);
}

void printAudioDeviceInfo(const Device::Info& info) {
    if (info.name)
    {
        printf(" %s (", info.name.value.c_str());
        
        if (info.alive)
            printf("%s, ", info.alive.value ? "alive" : "not alive");
    
        if (info.running)
            printf("%s", info.running.value ? "running" : "not running");
        
        printf("):\n");
        printf("\tUID: %d\n", info.uid);
    }
    else
        printf("Device %d:\n", info.uid);
    
    printf("\tClass: ");
    auto classId = Manager::getClassId(info.uid);
    print4(classId);
    printf("\n");
    
    printf("\tBase class: ");
    auto baseClassId = Manager::getBaseClassId(info.uid);
    print4(baseClassId);
    printf("\n");
    
    auto owner = Manager::getOwner(info.uid);
    printf("\n", owner.);
    
    if (info.model)
        printf("\tModel: %s\n", info.model.value.c_str());
    
    if (info.manufacturer)
        printf("\tManufacturer: %s\n", info.manufacturer.value.c_str());
    
    if (info.serial)
        printf("\tSerial: %s\n", info.serial.value.c_str());
    
    if (info.firmware)
        printf("\tFirmware: %s\n", info.firmware.value.c_str());
    
    if (info.deviceUID)
        printf("\tPersistent device UID: %s\n", info.deviceUID.value.c_str());
    
    if (info.modelUID)
        printf("\tPersistent model UID: %s\n", info.modelUID.value.c_str());
    
    if (info.clockDomain)
        printf("\tClock domain: 0x%X\n", info.clockDomain.value);
    
    if (info.latency)
        printf("\tLatency: %u\n", info.latency.value);
    
    if (info.safetyOffset)
        printf("\tSafety offset: %u\n", info.safetyOffset.value);
    
    if (info.nominalSampleRate)
        printf("\tNominal sample rate: %g\n", info.nominalSampleRate.value);
    
    if (info.canBeDefault)
        printf("\tCan be default: %s\n", info.canBeDefault.value ? "yes" : "no");
    
    if (info.canBeDefaultSystem)
        printf("\tCan be default system device: %s\n", info.canBeDefaultSystem.value ? "yes" : "no");
    
    if (info.configurationApp)
        printf("\tConfig App: %s\n", info.configurationApp.value.c_str());
    
    if (info.transportType) {
        printf("\tTransport type: \'");
        print4(info.transportType.value);
        printf("\'\n");
    }
    
    if (info.streams && !info.streams.value.empty()) {
        printf("\tStreams:\n");
        for (auto id : info.streams.value) {
            auto info = Manager::getStreamInfo(id);
            printAudioStreamInfo(info, "\t\t");
        }
    }
    
    if (info.relatedDevices && !info.relatedDevices.value.empty() && !(info.relatedDevices.value.size() == 1 && info.relatedDevices.value[0] == info.uid)) {
        printf("\tRelated devices: ");
        for (auto id : info.relatedDevices.value)
            if (id != info.uid) printf("%u ", id);
        printf("\n");
    }
    
    if (info.controlList && !info.controlList.value.empty()) {
        printf("\tControl list: ");
        for (auto _id : info.controlList.value)
            printf("%u ", _id);
        printf("\n");
    }
    
    if (info.nominalSampleRates && !info.nominalSampleRates.value.empty()) {
        printf("\tNominal sample rates: ");
        
        auto& nominalSampleRates = info.nominalSampleRates.value;
        
        auto iter = nominalSampleRates.begin();
        while (iter != nominalSampleRates.end())
        {
            if (iter != nominalSampleRates.begin())
                printf(", ");
            
            auto range = *iter;
            if (range.mMinimum == range.mMaximum)
                printf("%g", range.mMinimum);
            else
                printf("%g - %g", range.mMinimum / 1000., range.mMaximum / 1000.);
            
            iter++;
        }
        
        printf("\n");
    }
}

void printAudioStreamBasicDescription(const AudioStreamBasicDescription& d, const char* prepend) {
    /*    Float64             mSampleRate;
    AudioFormatID       mFormatID;
    AudioFormatFlags    mFormatFlags;
    UInt32              mBytesPerPacket;
    UInt32              mFramesPerPacket;
    UInt32              mBytesPerFrame;
    UInt32              mChannelsPerFrame;
    UInt32              mBitsPerChannel;
    UInt32              mReserved;*/
    printf("%sSample rate: %g\n", prepend, d.mSampleRate);
    printf("%sFormat ID: \'", prepend); print4(d.mFormatID); printf("\'\n");
    printf("%sFormat Flags: ", prepend); printAudioFormatFlags(d.mFormatFlags); printf("\n");
    printf("%sBytes per packet: %u\n", prepend, d.mBytesPerPacket);
    printf("%sFrames per packet: %u\n", prepend, d.mFramesPerPacket);
    printf("%sBytes per frame: %u\n", prepend, d.mBytesPerFrame);
    printf("%sChannels per frame: %u\n", prepend, d.mChannelsPerFrame);
    printf("%sBits per channel: %u\n", prepend, d.mBitsPerChannel);
}

void printAudioFormatFlags(AudioFormatFlags f) {
/* kAudioFormatFlagIsFloat                     = (1U << 0),     // 0x1
    kAudioFormatFlagIsBigEndian                 = (1U << 1),     // 0x2
    kAudioFormatFlagIsSignedInteger             = (1U << 2),     // 0x4
    kAudioFormatFlagIsPacked                    = (1U << 3),     // 0x8
    kAudioFormatFlagIsAlignedHigh               = (1U << 4),     // 0x10
    kAudioFormatFlagIsNonInterleaved            = (1U << 5),     // 0x20
    kAudioFormatFlagIsNonMixable                = (1U << 6),     // 0x40 */
    
    bool isFloat = f & kAudioFormatFlagIsFloat;
    bool isInteger = f & kAudioFormatFlagIsSignedInteger;
    bool nonInterleaved = f & kAudioFormatFlagIsNonInterleaved;
    bool isPacked = f & kAudioFormatFlagIsPacked;
    bool alignedHigh = f & kAudioFormatFlagIsAlignedHigh;
    bool bigEndian = f & kAudioFormatFlagIsBigEndian;
    bool nonMixable = f & kAudioFormatFlagIsNonMixable;
    
    printf("%s, %s, %s, %s", isFloat ? "float" : (isInteger ? "int" : ""),
                         nonInterleaved ? "non-interleaved" : "interleaved",
                         bigEndian ? "big endian" : "little endian",
                         isPacked ? "packed" : "");
}

void printAudioStreamInfo(const Stream::Info& info, const char* prepend) {
    printf("%s%u", prepend, info.uid);
    
    if (info.active || info.direction) {
        printf(": ");

        if (info.direction)
            printf("%s", info.direction.value ? "input" : "output");

        if (info.active)
            printf(", %s", info.active.value ? "active" : "inactive");
        
        printf("\n");
    }
    else
        printf("\n");
    
    if (info.virtualFormat) {
        printf("%sVirtual format:\n", prepend);
        printAudioStreamBasicDescription(info.virtualFormat.value, "\t\t\t");
    }
    
    if (info.availableVirtualFormats) {
        printf("%sAvailable virtual formats:\n", prepend);
        for (auto& fmt : info.availableVirtualFormats.value) {
            printAudioStreamBasicDescription(fmt, "\t\t\t");
            printf("\n");
        }
    }
    
    if (info.physicalFormat) {
        printf("%sPhysical format:\n", prepend);
        printAudioStreamBasicDescription(info.physicalFormat.value, "\t\t\t");
    }
    
    printf("\n");
}

int main(int argc, char** argv)
{
    auto ids = Manager::getDeviceIds();
    for (auto id : ids)
    {
        Device::Info info = Manager::getDeviceInfo(id);
        printAudioDeviceInfo(info);
        printf("\n");
    }
    
    return 0;
}
