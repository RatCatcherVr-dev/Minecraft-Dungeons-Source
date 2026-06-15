// Copyright (c) Microsoft. All rights reserved.

#include "CllPartA.h"
#include <string>

namespace cll
{
    struct Windows7PartA : public cll::CllPartA
    {
        Windows7PartA(const std::string& iKey);

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
