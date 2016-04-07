#include <stdio.h>
#include "AudioManager.hpp"
#define BOOST_TEST_MODULE AudioManagerTest
#include <boost/test/unit_test.hpp>

int main(int argc, char** argv)
{
    auto ids = AudioManager::getDeviceIds();
    for (auto id : ids)
    {
        AudioDevice::Info info = AudioManager::getDeviceInfo(id);
        
        if (info.name.defined) {
            printf(" %s (", info.name.value.c_str());
            
            if (info.alive.defined)
                printf("%s, ", info.alive.value ? "alive" : "not alive");
        
            if (info.running.defined)
                printf("%s", info.running.value ? "running" : "not running");
            
            printf("):\n");
            printf("\tUID: %d\n", id);
        }
        else
            printf("Device %d:\n", id);
        
        if (info.model.defined)
            printf("\tModel: %s\n", info.model.value.c_str());
        
        if (info.manufacturer.defined)
            printf("\tManufacturer: %s\n", info.manufacturer.value.c_str());
        
        if (info.serial.defined)
            printf("\tSerial: %s\n", info.serial.value.c_str());
        
        if (info.firmware.defined)
            printf("\tFirmware: %s\n", info.firmware.value.c_str());
        
        if (info.deviceUID.defined)
            printf("\tPersistent device UID: %s\n", info.deviceUID.value.c_str());
        
        if (info.modelUID.defined)
            printf("\tPersistent model UID: %s\n", info.modelUID.value.c_str());
        
        if (info.clockDomain.defined)
            printf("\tClock domain: 0x%X\n", info.clockDomain.value);
        
        if (info.latency.defined)
            printf("\tLatency: %u\n", info.latency.value);
        
        if (info.safetyOffset.defined)
            printf("\tSafety offset: %u\n", info.safetyOffset.value);
        
        if (info.nominalSampleRate.defined)
            printf("\tNominal sample rate: %g\n", info.nominalSampleRate.value);
        
        if (info.canBeDefault.defined)
            printf("\tCan be default: %s\n", info.canBeDefault.value ? "yes" : "no");
        
        if (info.canBeDefaultSystem.defined)
            printf("\tCan be default system device: %s\n", info.canBeDefaultSystem.value ? "yes" : "no");
        
        if (info.configurationApp.defined)
            printf("\tConfig App: %s\n", info.configurationApp.value.c_str());
        
        if (info.transportType.defined) {
            printf("\tTransport type: \'");
            
            UInt32 t = info.transportType.value;
            for (int shift = 24; shift >= 0; shift -= 8)
                printf("%c", (t & (0xff << shift)) >> shift);
            
            printf("\'\n");
        }
        
        if (info.streams.defined && !info.streams.value.empty()) {
            printf("\tStreams: ");
            for (auto id : info.streams.value)
                printf("%u ", id);
            printf("\n");
        }
        
        if (info.relatedDevices.defined && !info.relatedDevices.value.empty() && !(info.relatedDevices.value.size() == 1 && info.relatedDevices.value[0] == id)) {
            printf("\tRelated devices: ");
            for (auto rid : info.relatedDevices.value)
                if (rid != id) printf("%u ", rid);
            printf("\n");
        }
        
        if (info.controlList.defined && !info.controlList.value.empty()) {
            printf("\tControl list: ");
            for (auto _id : info.controlList.value)
                printf("%u ", _id);
            printf("\n");
        }
        
        if (info.nominalSampleRates.defined && !info.nominalSampleRates.value.empty()) {
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
        
        printf("\n");
    }
    
    return 0;
}