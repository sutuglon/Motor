/*
 *  bundle.cpp
 *  GLSprite
 *
 *  Created by Chema on 8/24/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <string>
#include <CoreFoundation/CFBundle.h>
#include "bundle.h"

std::string GetBundleResourcePath(const std::string& file) {
    std::string ret;
    
    CFBundleRef mainBundle;
    mainBundle = CFBundleGetMainBundle();
    if(mainBundle) {
        CFURLRef resourceURL;
        resourceURL = CFBundleCopyResourceURL( 
            mainBundle,
            CFStringCreateWithCString( NULL, file.c_str(), kCFStringEncodingASCII), 
            NULL, NULL);

        if(resourceURL)
        {                   
            char fileurl[255];
            if(CFURLGetFileSystemRepresentation( resourceURL, true, reinterpret_cast<UInt8*>(fileurl), 255)) {
                ret = fileurl;
            }
        }
    }
    return ret;
}

