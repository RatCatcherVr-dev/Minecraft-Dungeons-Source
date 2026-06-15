// Copyright (c) Microsoft. All rights reserved.

// Populates iOS specific Part A fields

#pragma once

#import <Foundation/Foundation.h>
#include <string>

#include "CllPartA.h"

using std::shared_ptr;
using std::string;

namespace cll
{
    class iOSPartA : public cll::CllPartA
    {
    public:
    
        iOSPartA(const string & iKey);
    
    protected:
    
        std::string generateHashedIdBase(int lengthInBytes);
        std::string collectOsVer();
        std::string collectOsLocale();
        std::string collectAppId();
        std::string collectAppVer();
        std::string collectDeviceId();
        std::string collectDeviceClass();
        std::string collectUserId();
    
        NSDictionary<NSString *,id> * getBundleInfo();
    };
}
