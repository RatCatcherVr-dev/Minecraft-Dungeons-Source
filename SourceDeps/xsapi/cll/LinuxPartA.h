// Copyright (c) Microsoft. All rights reserved.

// Populates Linux specific Part A fields

#pragma once

#include <string>

#include "CllPartA.h"

namespace cll
{
    class LinuxPartA : public cll::CllPartA
    {
    public:
        
        LinuxPartA(const std::string & iKey);
        
    protected:
        
        std::string generateHashedIdBase(int lengthInBytes);
        std::string collectOsVer();
        std::string collectOsLocale();
        std::string collectAppId();
        std::string collectAppVer();
        std::string collectDeviceId();
        std::string collectDeviceClass();
        std::string collectUserId();
    };
}
