#pragma once

namespace Audio
{
    class System : public Object
    {
    public:
        System() : Object(kAudioObjectSystemObject) {
        }
    };

    /*class Manager
    {
    public:
        static vector<DeviceId> getDeviceIds()
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
            
            return vector<DeviceId>();
        }
        
        static Device::Info getDeviceInfo(DeviceId id)
        {
            Device::Info info;
            
            AudioObjectPropertyAddress p {
                kAudioObjectPropertyName,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };
            
            info.uid = id;
            
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
            if (transportType) info.transportType = transportType.value;
            
            p.mSelector = kAudioDevicePropertyClockDomain;
            auto clockDomain = getProperty<UInt32>(id, p);
            if (clockDomain) info.clockDomain = clockDomain.value;
            
            p.mSelector = kAudioDevicePropertyLatency;
            auto latency = getProperty<UInt32>(id, p);
            if (latency) info.latency = latency.value;
            
            p.mSelector = kAudioDevicePropertySafetyOffset;
            auto safetyOffset = getProperty<UInt32>(id, p);
            if (safetyOffset) info.safetyOffset = safetyOffset.value;
            
            p.mSelector = kAudioDevicePropertyNominalSampleRate;
            auto nominalSampleRate = getProperty<Float64>(id, p);
            if (nominalSampleRate) info.nominalSampleRate = nominalSampleRate.value;
            
            p.mSelector = kAudioDevicePropertyDeviceIsAlive;
            auto alive = getProperty<UInt32>(id, p);
            if (alive) info.alive = alive.value;
            
            p.mSelector = kAudioDevicePropertyDeviceIsRunning;
            auto running = getProperty<UInt32>(id, p);
            if (running) info.running = running.value;
            
            p.mSelector = kAudioDevicePropertyDeviceCanBeDefaultDevice;
            auto canBeDefault = getProperty<UInt32>(id, p);
            if (canBeDefault) info.canBeDefault = canBeDefault.value;
            
            p.mSelector = kAudioDevicePropertyDeviceCanBeDefaultSystemDevice;
            auto canBeDefaultSystem = getProperty<UInt32>(id, p);
            if (canBeDefaultSystem) info.canBeDefaultSystem = canBeDefaultSystem.value;
            
            p.mSelector = kAudioDevicePropertyStreams;
            auto streams = getVectorProperty<AudioStreamID>(id, p);
            if (streams) info.streams = streams.value;
            
            p.mSelector = kAudioDevicePropertyRelatedDevices;
            auto relatedDevices = getVectorProperty<AudioDeviceID>(id, p);
            if (relatedDevices) info.relatedDevices = relatedDevices.value;

            p.mSelector = kAudioObjectPropertyControlList;
            auto controlList = getVectorProperty<AudioObjectID>(id, p);
            if (controlList) info.controlList = controlList.value;
            
            p.mSelector = kAudioDevicePropertyAvailableNominalSampleRates;
            auto nominalSampleRates = getVectorProperty<AudioValueRange>(id, p);
            if (nominalSampleRates) info.nominalSampleRates = nominalSampleRates.value;
            
            return info;
        }
        
        static Stream::Info getStreamInfo(StreamId id)
        {
            Stream::Info info;
            
            AudioObjectPropertyAddress p {
                kAudioObjectPropertyName,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };
            
            info.uid = id;
            
            p.mSelector = kAudioStreamPropertyIsActive;
            auto active = getProperty<UInt32>(id, p);
            if (active) info.active = active.value;
            
            p.mSelector = kAudioStreamPropertyDirection;
            auto direction = getProperty<UInt32>(id, p);
            if (direction) info.direction = direction.value;
            
            p.mSelector = kAudioStreamPropertyStartingChannel;
            auto startingChannel = getProperty<UInt32>(id, p);
            if (startingChannel) info.startingChannel = startingChannel.value;
            
            p.mSelector = kAudioStreamPropertyVirtualFormat;
            auto virtualFormat = getProperty<AudioStreamBasicDescription>(id, p);
            if (virtualFormat) info.virtualFormat = virtualFormat.value;
            
            p.mSelector = kAudioStreamPropertyPhysicalFormat;
            auto physicalFormat = getProperty<AudioStreamBasicDescription>(id, p);
            if (physicalFormat) info.physicalFormat = physicalFormat.value;
            
            p.mSelector = kAudioStreamPropertyAvailableVirtualFormats;
            auto availableVirtualFormats = getVectorProperty<AudioStreamBasicDescription>(id, p);
            if (availableVirtualFormats)
                info.availableVirtualFormats = availableVirtualFormats.value;
            
            p.mSelector = kAudioStreamPropertyAvailablePhysicalFormats;
            auto availablePhysicalFormats = getVectorProperty<AudioStreamBasicDescription>(id, p);
            if (availablePhysicalFormats)
                info.availablePhysicalFormats = availablePhysicalFormats.value;
            
            return info;
        }
        
        static Device* getDeviceById(DeviceId id) {
            return nullptr;
        }
        
        static ClassId getClassId(ObjectId id) {
            AudioObjectPropertyAddress p {
                kAudioObjectPropertyClass,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };
            
            return getProperty<ClassId>(id, p).value;
        }
        
        static ClassId getBaseClassId(ObjectId id) {
            AudioObjectPropertyAddress p {
                kAudioObjectPropertyBaseClass,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };
            
            return getProperty<ClassId>(id, p).value;
        }
        
        static Option<ObjectId> getOwner(ObjectId id) {
            AudioObjectPropertyAddress p {
                kAudioObjectPropertyOwner,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };
            
            return getProperty<ObjectId>(id, p);
        }
    };*/
}
