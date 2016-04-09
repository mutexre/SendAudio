//
//  Helper.cpp
//  SendAudio
//
//  Created by mutexre on 07/04/16.
//
//

#include "Audio.hpp"

Option<string> getStringFromCFString(CFStringRef cfstr, unsigned maxChars) {
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