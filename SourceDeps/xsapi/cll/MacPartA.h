// Copyright (c) Microsoft. All rights reserved.

// Populates Mac OS X specific Part A fields

#include <string>

#include "CllPartA.h"

@class NSDictionary<Key, Value>;
@class NSString;

namespace cll
{
    class MacPartA : public cll::CllPartA
    {
    public:
        
        MacPartA(const std::string & iKey);
        
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
