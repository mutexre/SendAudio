//
//  Helper.hpp
//  SendAudio
//
//  Created by mutexre on 07/04/16.
//
//

#pragma once

Option<string> getStringFromCFString(CFStringRef cfstr, unsigned maxChars = 2 * 1024);
