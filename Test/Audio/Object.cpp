#include "Audio.hpp"

namespace Audio
{
    template <typename T>
    static Option<vector<T>> getVectorProperty(AudioObjectID id, const AudioObjectPropertyAddress& p)
    {
        if (AudioObjectHasProperty(id, &p))
        {
            UInt32 dataSize;
            
            OSStatus status = AudioObjectGetPropertyDataSize(id, &p, 0, nullptr, &dataSize);
            if (status != kAudioHardwareNoError) throw runtime_error("");
            
            if (dataSize > 0) {
                unique_ptr<char> data(new char[dataSize]);
                auto n = dataSize / sizeof(T);
                vector<T> value(n);
                
                status = AudioObjectGetPropertyData(id, &p, 0, nullptr, &dataSize, data.get());
                if (status != kAudioHardwareNoError) throw runtime_error("");
                
                memcpy(value.data(), data.get(), sizeof(T) * n);
                
                return value;
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
                
                return value;
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
            
            return cfstr;
        }
        
        return Option<CFStringRef>();
    }
    
    static void takeCFStringProperty(AudioObjectID id,
                                     const AudioObjectPropertyAddress& p,
                                     const function<void(const string& str)>& f)
    {
        auto cfstr = getCFStringProperty(id, p);
        if (cfstr) {
            auto str = getStringFromCFString(cfstr.value);
            if (str) f(str.value);
            CFRelease(cfstr.value);
        }
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

    void getInfo() {
        name = convertCFStringToString(getCFStringProperty(id, ));
    }
}
