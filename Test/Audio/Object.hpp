#pragma once

namespace Audio
{
    class Object
    {
    private:
        ClassId classId;
        ObjectId id;
        Option<ObjectId> owner;
        Option<vector<ObjectId>> ownedObjects;
        Option<string> name, model, manufacturer, serial, firmware;
        
    protected:
        template <typename T>
        static Option<vector<T>> getVectorProperty(ObjectId, const PropertyAddress&);
        
        template <typename T>
        static Option<T> getProperty(ObjectId, const PropertyAddress&);
        
        static Option<CFStringRef> getCFStringProperty(ObjectId, const PropertyAddress&);
        static Option<string> getStringProperty(ObjectId, const PropertyAddress&);
        
        static void takeCFStringProperty(ObjectId,
                                         const PropertyAddress&,
                                         const function<void(const string& str)>&);
        
        static ClassId getClass(ObjectId);
        static ClassId getBaseClass(ObjectId);
    
    protected:
        template <typename T> Option<vector<T>> getVectorProperty(const PropertyAddress&);
        template <typename T> Option<T> getProperty(const PropertyAddress&);
        
        Option<CFStringRef> getCFStringProperty(const PropertyAddress&);
        Option<string> getStringProperty(const PropertyAddress&);
        
        void takeCFStringProperty(const PropertyAddress&, const function<void(const string& str)>&);
        
        ClassId getClass();
        ClassId getBaseClass();
    
        void getProperties() {
            classId = getClass();
            baseClassId = getBaseClass();
            name = getStringProperty(PropertyAddress(kAudioObjectPropertyName));
            model = getStringProperty(PropertyAddress(kAudioObjectPropertyModelName));
            manufacturer = getStringProperty(PropertyAddress(kAudioObjectPropertyManufacturer));
            serial = getStringProperty(PropertyAddress(kAudioObjectPropertySerial));
            firmware = getStringProperty(PropertyAddress(kAudioObjectPropertyFirmware));
        }
        
    public:
        Object() {}
        
        Object(ObjectId id) {
            this->id = id;
            getProperties();
        }
        
        virtual ~Object() {}
        
        ClassId getClassId() const { return classId; }
        ObjectId getId() const { return id; }
        
        Option<string> getName() const { return name; }
        Option<string> getModel() const { return model; }
        Option<string> getManufacturer() const { return manufacturer; }
        Option<string> getSerial() const { return serial; }
        Option<string> getFirmware() const { return firmware; }
    };
}
