#pragma once

#include <string>
#include <list>
#include <vector>
#include <functional>
#include <CoreAudio/CoreAudio.h>
#include "Option.hpp"

using namespace std;

namespace Audio
{
    class PropertyAddress;
    class Object;
    class Plugin;
    class TransportManager;
    class Box;
    class Device;
    class EndpointDevice;
    class Stream;
    class System;
}

#include "Types.hpp"
#include "Helper.hpp"
#include "PropertyAddress.hpp"
#include "Object.hpp"
#include "Device.hpp"
#include "Stream.hpp"
#include "System.hpp"
