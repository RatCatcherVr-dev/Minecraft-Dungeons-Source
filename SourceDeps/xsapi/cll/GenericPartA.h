// Copyright (c) Microsoft. All rights reserved.

// Populates Linux specific Part A fields

#pragma once

#include <string>

#include <cll\CllPartA.h>

namespace cll
{
    class GenericPartA : public cll::CllPartA
    {
    public:
        GenericPartA(
            std::string&& iKey,
            std::string&& appId,
            std::string&& appVer,
            std::string&& osName,
            std::string&& osLocale,
            std::string&& osVersion,
            std::string&& deviceClass,
            std::string&& deviceId
        );
    };
}
