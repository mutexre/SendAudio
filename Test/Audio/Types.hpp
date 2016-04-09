#pragma once

namespace Audio
{
    using ClassId = AudioClassID;
    using ObjectId = AudioObjectID;
    using DeviceId = AudioObjectID;
    using StreamId = AudioStreamID;
    using ControlId = AudioObjectID;
    
    class PropertyAddress : public AudioObjectPropertyAddress
    {
    public:
        using Selector = AudioObjectPropertySelector;
        
        enum class Scope : AudioObjectPropertyScope
        {
            global      = kAudioObjectPropertyScopeGlobal,
            in          = kAudioObjectPropertyScopeInput,
            out         = kAudioObjectPropertyScopeOutput,
            playThrough = kAudioObjectPropertyScopePlayThrough,
        };
        
        using Element = AudioObjectPropertyElement;
        static const Element kMasterElement = kAudioObjectPropertyElementMaster;
        
    public:
        PropertyAddress(Selector sel, Scope scope = Scope::global, Element element = kMasterElement) {
            this->mSelector = sel;
            this->mScope = (UInt32)scope;
            this->mElement = element;
        }
    };
}
