//
//  AudioManager.hpp
//  SendAudio
//
//  Created by mutexre on 04/04/16.
//
//

#pragma once

#include <string>
#include <list>
#include <vector>
#include <functional>
#include <CoreAudio/CoreAudio.h>
#include "Option.hpp"

using namespace std;

Option<string> getStringFromCFString(CFStringRef cfstr, unsigned maxChars = 2 * 1024) {
    size_t bufferSize = 256;
    Boolean result;
    
    do {
        unique_ptr<char[]> buffer(new char[bufferSize]);
        result = CFStringGetCString(cfstr, buffer.get(), bufferSize, kCFStringEncodingUTF8);
        if (result)
            return Option<string>(buffer.get());
        else
            bufferSize <<= 1;
    }
    while (bufferSize < maxChars);
    
    return Option<string>();
}

using AudioDeviceId = AudioObjectID;
using AudioStreamId = AudioStreamID;

class AudioDevice
{
    friend class AudioManager;
    
public:
    struct Info {
        Option<string> name, model, manufacturer, serial, firmware, configurationApp;
        Option<string> deviceUID, modelUID;
        Option<UInt32> transportType;
        Option<UInt32> clockDomain, latency, safetyOffset;
        Option<Float64> nominalSampleRate;
        Option<bool> alive, running;
        Option<bool> canBeDefault, canBeDefaultSystem;
        Option<vector<AudioStreamId>> streams;
        Option<vector<AudioDeviceId>> relatedDevices;
        Option<vector<AudioObjectID>> controlList;
        Option<vector<AudioValueRange>> nominalSampleRates;
    };
    
    enum class Type {
        in, out, all
    };
    
    using InputBlock = std::function<void(void)>;
    using OutputBlock = std::function<void(void)>;
    
protected:
    AudioDeviceId id;
    Type type;
    InputBlock inputBlock;
    OutputBlock outputBlock;
    
public:
    void setInputBlock(const InputBlock&);
    void setOutputBlock(const OutputBlock&);
    
    Info getInfo() const {
        return Info{};//AudioManager::getDeviceInfo(id);
    }
    
    AudioTimeStamp getCurrentTime() {
        AudioTimeStamp t;
        AudioDeviceGetCurrentTime(id, &t);
        return t;
    }
};

class AudioStream
{
public:
    enum class Type {
        in, out, all
    };
    
    struct Info {
        Option<bool> active, direction;
        Option<int> terminalType, startingChannel;
        Option<AudioStreamBasicDescription> virtualFormat, physicalFormat;
        Option<vector<AudioStreamBasicDescription>> availableVirtualFormats, availablePhysicalFormat;
    };
    
private:
    AudioStreamId id;
    Type type;
    
public:
};

class AudioManager
{
private:
    template <typename T>
    static Option<vector<T>> getVectorProperty(AudioObjectID id, const AudioObjectPropertyAddress& p)
    {
        if (AudioObjectHasProperty(id, &p))
        {
            UInt32 dataSize;
            
            OSStatus status = AudioObjectGetPropertyDataSize(id, &p, 0, nullptr, &dataSize);
            if (status != kAudioHardwareNoError) throw runtime_error("");
            
            if (dataSize > 0) {
                vector<T> value(dataSize / sizeof(T));
                
                status = AudioObjectGetPropertyData(id, &p, 0, nullptr, &dataSize, value.data());
                if (status != kAudioHardwareNoError) throw runtime_error("");
                
                return Option<vector<T>>(value);
            }
        }
        
        return Option<vector<T>>();
    }
    
    template <typename T>
    static Option<T> getProperty(AudioObjectID id, const AudioObjectPropertyAddress& p)
    {
        if (AudioObjectHasProperty(id, &p))
        {
            UInt32 dataSize;
            
            OSStatus status = AudioObjectGetPropertyDataSize(id, &p, 0, nullptr, &dataSize);
            if (status != kAudioHardwareNoError) throw runtime_error("");
            
            if (dataSize == sizeof(T)) {
                T value;
            
                status = AudioObjectGetPropertyData(id, &p, 0, nullptr, &dataSize, &value);
                if (status != kAudioHardwareNoError) throw runtime_error("");
                
                return Option<T>(value);
            }
            else
                throw runtime_error("");
        }
        
        return Option<T>();
    }
    
    static Option<CFStringRef> getCFStringProperty(AudioObjectID id, const AudioObjectPropertyAddress& p)
    {
        if (AudioObjectHasProperty(id, &p))
        {
            CFStringRef cfstr = nullptr;
            
            UInt32 dataSize = sizeof(cfstr);
            auto status = AudioObjectGetPropertyData(id, &p, 0, nullptr, &dataSize, &cfstr);
            if (status != kAudioHardwareNoError) throw runtime_error("");
            
            return Option<CFStringRef>(cfstr);
        }
        
        return Option<CFStringRef>();
    }
    
    static void takeCFStringProperty(AudioObjectID id,
                                     const AudioObjectPropertyAddress& p,
                                     const function<void(const string& str)>& f)
    {
        auto cfstr = getCFStringProperty(id, p);
        if (cfstr.defined) {
            auto str = getStringFromCFString(cfstr.value);
            if (str.defined) f(str.value);
            CFRelease(cfstr.value);
        }
    }
    
public:
    static vector<AudioDeviceId> getDeviceIds()
    {
        AudioObjectPropertyAddress p = {
            kAudioHardwarePropertyDevices,
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMaster
        };
        
        if (AudioObjectHasProperty(kAudioObjectSystemObject, &p))
        {
            OSStatus status;
            
            UInt32 dataSize;
            status = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &p, 0, nullptr, &dataSize);
            if (status != kAudioHardwareNoError) throw runtime_error("AudioObjectGetPropertyDataSize failed");
            
            auto deviceCount = dataSize / sizeof(AudioObjectID);
            vector<AudioObjectID> devices(deviceCount);
            
            status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &p, 0, nullptr, &dataSize, devices.data());
            if (status != kAudioHardwareNoError) throw runtime_error("AudioObjectGetPropertyData failed");
            
            return devices;
        }
        
        return vector<AudioDeviceId>();
    }
    
    static AudioDevice::Info getDeviceInfo(AudioDeviceId id)
    {
        AudioDevice::Info info;
        
        AudioObjectPropertyAddress p {
            kAudioObjectPropertyName,
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMaster
        };
        
        takeCFStringProperty(id, p, [ &info ](const string& str) {
            info.name.define(str);
        });
        
        p.mSelector = kAudioObjectPropertyModelName;
        takeCFStringProperty(id, p, [ &info ](const string& str) {
            info.model.define(str);
        });
        
        p.mSelector = kAudioObjectPropertyManufacturer;
        takeCFStringProperty(id, p, [ &info ](const string& str) {
            info.manufacturer.define(str);
        });
        
        p.mSelector = kAudioObjectPropertySerialNumber;
        takeCFStringProperty(id, p, [ &info ](const string& str) {
            info.serial.define(str);
        });
        
        p.mSelector = kAudioObjectPropertyFirmwareVersion;
        takeCFStringProperty(id, p, [ &info ](const string& str) {
            info.firmware.define(str);
        });
        
        p.mSelector = kAudioDevicePropertyConfigurationApplication;
        takeCFStringProperty(id, p, [ &info ](const string& str) {
            info.configurationApp.define(str);
        });
        
        p.mSelector = kAudioDevicePropertyDeviceUID;
        takeCFStringProperty(id, p, [ &info ](const string& str) {
            info.deviceUID.define(str);
        });
        
        p.mSelector = kAudioDevicePropertyModelUID;
        takeCFStringProperty(id, p, [ &info ](const string& str) {
            info.modelUID = str;
        });

        p.mSelector = kAudioDevicePropertyTransportType;
        auto transportType = getProperty<UInt32>(id, p);
        if (transportType.defined)
            info.transportType = transportType.value;
        
        p.mSelector = kAudioDevicePropertyClockDomain;
        auto clockDomain = getProperty<UInt32>(id, p);
        if (clockDomain.defined)
            info.clockDomain = clockDomain.value;
        
        p.mSelector = kAudioDevicePropertyLatency;
        auto latency = getProperty<UInt32>(id, p);
        if (latency.defined)
            info.latency = latency.value;
        
        p.mSelector = kAudioDevicePropertySafetyOffset;
        auto safetyOffset = getProperty<UInt32>(id, p);
        if (safetyOffset.defined)
            info.safetyOffset = safetyOffset.value;
        
        p.mSelector = kAudioDevicePropertyNominalSampleRate;
        auto nominalSampleRate = getProperty<Float64>(id, p);
        if (nominalSampleRate.defined)
            info.nominalSampleRate = nominalSampleRate.value;
        
        p.mSelector = kAudioDevicePropertyDeviceIsAlive;
        auto alive = getProperty<UInt32>(id, p);
        if (alive.defined)
            info.alive = alive.value;
        
        p.mSelector = kAudioDevicePropertyDeviceIsRunning;
        auto running = getProperty<UInt32>(id, p);
        if (running.defined)
            info.running = running.value;
        
        p.mSelector = kAudioDevicePropertyDeviceCanBeDefaultDevice;
        auto canBeDefault = getProperty<UInt32>(id, p);
        if (canBeDefault.defined)
            info.canBeDefault = canBeDefault.value;
        
        p.mSelector = kAudioDevicePropertyDeviceCanBeDefaultSystemDevice;
        auto canBeDefaultSystem = getProperty<UInt32>(id, p);
        if (canBeDefaultSystem.defined)
            info.canBeDefaultSystem = canBeDefaultSystem.value;
        
        p.mSelector = kAudioDevicePropertyStreams;
        auto streams = getVectorProperty<AudioStreamID>(id, p);
        if (streams.defined)
            info.streams = streams.value;
        
        p.mSelector = kAudioDevicePropertyRelatedDevices;
        auto relatedDevices = getVectorProperty<AudioDeviceID>(id, p);
        if (relatedDevices.defined)
            info.relatedDevices = relatedDevices.value;

        p.mSelector = kAudioObjectPropertyControlList;
        auto controlList = getVectorProperty<AudioObjectID>(id, p);
        if (controlList.defined)
            info.controlList = controlList.value;
        
        p.mSelector = kAudioDevicePropertyAvailableNominalSampleRates;
        auto nominalSampleRates = getVectorProperty<AudioValueRange>(id, p);
        if (nominalSampleRates.defined)
            info.nominalSampleRates = nominalSampleRates.value;

    /*kAudioDevicePropertyConfigurationApplication        = 'capp',
    kAudioDevicePropertyDeviceUID                       = 'uid ',
    kAudioDevicePropertyModelUID                        = 'muid',
    kAudioDevicePropertyTransportType                   = 'tran',
    kAudioDevicePropertyRelatedDevices                  = 'akin',
    kAudioDevicePropertyClockDomain                     = 'clkd',
    kAudioDevicePropertyDeviceIsAlive                   = 'livn',
    kAudioDevicePropertyDeviceIsRunning                 = 'goin',
    kAudioDevicePropertyDeviceCanBeDefaultDevice        = 'dflt',
    kAudioDevicePropertyDeviceCanBeDefaultSystemDevice  = 'sflt',
    kAudioDevicePropertyLatency                         = 'ltnc',
    kAudioDevicePropertyStreams                         = 'stm#',
    kAudioObjectPropertyControlList                     = 'ctrl',
    kAudioDevicePropertySafetyOffset                    = 'saft',
    kAudioDevicePropertyNominalSampleRate               = 'nsrt',
    kAudioDevicePropertyAvailableNominalSampleRates     = 'nsr#',
    kAudioDevicePropertyIcon                            = 'icon',
    kAudioDevicePropertyIsHidden                        = 'hidn',
    kAudioDevicePropertyPreferredChannelsForStereo      = 'dch2',
    kAudioDevicePropertyPreferredChannelLayout          = 'srnd'*/

    
        /*kAudioStreamPropertyIsActive                    = 'sact',
    kAudioStreamPropertyDirection                   = 'sdir',
    kAudioStreamPropertyTerminalType                = 'term',
    kAudioStreamPropertyStartingChannel             = 'schn',
    kAudioStreamPropertyLatency                     = kAudioDevicePropertyLatency,
    kAudioStreamPropertyVirtualFormat               = 'sfmt',
    kAudioStreamPropertyAvailableVirtualFormats     = 'sfma',
    kAudioStreamPropertyPhysicalFormat              = 'pft ',
    kAudioStreamPropertyAvailablePhysicalFormats    = 'pfta'*/
        
        return info;
    }
    
    static AudioStream::Info getStreamInfo(AudioStreamId id)
    {
        AudioStream::Info info;
        return info;
    }
    
    static AudioDevice* getDeviceById(AudioDeviceId id) {
        return nullptr;
    }
};
