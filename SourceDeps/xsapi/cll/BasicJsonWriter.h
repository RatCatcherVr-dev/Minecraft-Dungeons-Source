// Copyright (c) Microsoft. All rights reserved.

// Basic support for creating simple Json string objects.

#pragma once

#include <string>
#include <vector>

namespace cll
{
    // Class methods are static, so the necessary state is passed between methods in a variable named "state".
    class BasicJsonWriter
    {
    public:
        
        static void StartObject(std::string & buffer, bool & state);
        static void EndObject(std::string & buffer, bool & state);

        static void StartStruct(std::string & buffer, bool & state, const std::string & fieldName);
        static void EndStruct(std::string & buffer, bool & state);
        static void WriteSerializedStruct(std::string & buffer, bool & state, const std::string & fieldName, const std::string & fieldValue);
        
        // string
        static void WriteField(std::string & buffer, bool & state, const std::string & fieldName, const std::string & fieldValue);
        static void WriteOptionalField(std::string & buffer, bool & state, const std::string & fieldName, const std::string & fieldValue, const std::string & defaultFieldValue);
        
        // int
        static void WriteFieldInt32(std::string & buffer, bool & state, const std::string & fieldName, int fieldValue);
        static void WriteOptionalFieldInt32(std::string & buffer, bool & state, const std::string & fieldName, int fieldValue, int defaultFieldValue);
        
        // long
        static void WriteFieldInt64(std::string & buffer, bool & state, const std::string & fieldName, int64_t fieldValue);
        static void WriteOptionalFieldInt64(std::string & buffer, bool & state, const std::string & fieldName, int64_t fieldValue, int64_t defaultFieldValue);
        
        // double
        static void WriteFieldDouble(std::string & buffer, bool & state, const std::string & fieldName, double fieldValue);
        static void WriteOptionalFieldDouble(std::string & buffer, bool & state, const std::string & fieldName, double fieldValue, double defaultFieldValue);
        
        // string array
        static void WriteFieldArray(std::string & buffer, bool & state, const std::string & fieldName, const std::vector<std::string> & fieldValue);
        static void WriteOptionalFieldArray(std::string & buffer, bool & state, const std::string & fieldName, const std::vector<std::string> & fieldValue);
        
    private:
        
        static void WriteJsonEncodedString(std::string & buffer, const std::string & rawValue);
    };
}
