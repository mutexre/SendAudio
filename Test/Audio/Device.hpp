#pragma once

namespace Audio
{
    class Device : public Object
    {
        friend class Manager;
        
    public:
        struct Info {
            DeviceId uid;
            Option<string> name, model, manufacturer, serial, firmware, configurationApp;
            Option<string> deviceUID, modelUID;
            Option<UInt32> transportType;
            Option<UInt32> clockDomain, latency, safetyOffset;
            Option<Float64> nominalSampleRate;
            Option<bool> alive, running;
            Option<bool> canBeDefault, canBeDefaultSystem;
            Option<vector<StreamId>> streams;
            Option<vector<DeviceId>> relatedDevices;
            Option<vector<ControlId>> controlList;
            Option<vector<AudioValueRange>> nominalSampleRates;
        };
        
        enum class Type {
            in, out, all
        };
        
        using InputBlock = std::function<void(void)>;
        using OutputBlock = std::function<void(void)>;
        
    protected:
        DeviceId id;
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
}
