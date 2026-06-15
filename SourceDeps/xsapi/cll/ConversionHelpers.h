// Copyright (c) Microsoft. All rights reserved.

// Utilities to convert between numbers and strings

#pragma once

#include <string>
#include <vector>

namespace cll
{
    class ConversionHelpers
    {
    public:
        // Locale independent parsing of numeric strings
        static bool HexString2ULong(const std::string& s, size_t startPos, unsigned long& result);
        static bool String2UInt(const std::string& s, size_t startPos, unsigned int& result);
        static bool String2Int(const std::string& s, size_t startPos, int& result);
        static bool String2ULong(const std::string& s, size_t startPos, unsigned long& result);
        static bool String2Long(const std::string& s, size_t startPos, long& result);
        static bool String2Double(const std::string& s, size_t startPos, double& result);
        static bool String2Lower(const std::string& s, size_t startPos, size_t count, std::string& loweredString);
        
        // Locale-independent check if the left string starts with the right string
        static bool StringBeginsWith(const std::string & left, const std::string & right);

        static void SplitString(const std::string & s, const char separator, std::vector<std::string> & parts);
        static bool AreAllCharactersWhitelisted(const std::string & stringToTest, const std::string & whitelist);

        static int GetDigitCount(long number);
    };
}
