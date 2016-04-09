#pragma once

namespace Audio
{
    class Stream : public Object
    {
    public:
        enum class Type {
            in, out, all
        };
        
        struct Info {
            StreamId uid;
            Option<bool> active, direction;
            Option<int> terminalType, startingChannel;
            Option<AudioStreamBasicDescription> virtualFormat, physicalFormat;
            Option<vector<AudioStreamBasicDescription>> availableVirtualFormats, availablePhysicalFormats;
        };
        
    private:
        StreamId id;
        Type type;
        
    public:
    };
}
