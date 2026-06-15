// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include <mutex>
#include <string>

namespace cll
{
    // Implementation of the Common Schema standard vector clock type.
    // See https://osgwiki.com/wiki/CorrelationVector for more details.
    // Class methods are thread-safe.
    // Boolean-value methods return false to indicate failures.

    /*

    Quick start:

        #include <CorrelationVector.hpp>

        // Construct and initialize a correlation vector with a random base value,
        // share that value across the app components which are going to use it.
        // There could be, for example, one CV per app, per scenario, per user.
        CorrelationVector m_appCV;
        m_appCV.Initialize(2);

        // Get the next value, log it and/or pass it to your outgoing service call.
        std::string curCV = m_appCV.GetNextValue();
        EventProperties eventData("Microsoft.OneSDK.Example.HelloWorldEvent");
        eventData.SetProperty(CorrelationVector::PropertyName, curCV);

    Or, if you are receiving CorrelationVector string as an input
    from an upstream caller and would like to keep using and extending it:

        #include <CorrelationVector.hpp>

        // Construct a correlation vector and initialize it with the provided base value.
        CorrelationVector m_appCV;
        m_appCV.SetValue("jj9XLhDw7EuXoC2L");

        // Extend that value.
        m_appCV.Extend();

        // Get the next value, log it and/or pass it to your downstream dependency.
        std::string curCV = m_appCV.GetNextValue();
        EventProperties eventData("Microsoft.OneSDK.Example.HelloWorldEvent");
        eventData.SetProperty(CorrelationVector::PropertyName, curCV);

    */

    class CorrelationVector
    {
    public:

        // Constructs an uninitialized, not yet ready to use correlation vector
        CorrelationVector();

        // Initializes CV with a random base value and a current vector count of 0.
        // Version could be 1 or 2, where version allows for the longer base and full
        // CorrelationVector length. Use version 2 only if you are sure that
        // the downstream consumers support it.
        bool initialize(int version);

        // Resets CV value to an uninitialized state.
        // When uninitialized the internal CV value is "" and all Extend/Increment calls are ignored.
        void uninitialize();

        // Returns the flag, specifying whether the CV has been initialized or not.
        bool isInitialized() const;

        // Atomically reads the current CV string representation and increments it for the next use.
        // Returns the read value or empty string if CV is not initialized.
        std::string getNextValue();

        // Returns the current CV string representation or empty string if not initialized.
        std::string getValue() const;

        // Adds .0 to the end of the current correlation vector,
        // or does nothing if the maximum length was reached.
        bool extend();

        // Increments the last extension of the correlation vector,
        // or does nothing if the maximum length was reached.
        bool increment();

        // Checks to see if we can add an extra vector.
        // Returns false if the extension will put us over the maximum size of a correlation vector.
        bool canExtend();

        // Checks to see if we can increment the current vector.
        // Returns false if the extension will put us over the maximum size of a correlation vector.
        bool canIncrement();

        // Attempts to set the base and current vector values given the string representation.
        // String could also contain just the base value.
        // Version of CV is auto-detected based on the length of the base value.
        bool setValue(const std::string& cv);

    public:

        // String constant to use for sending a CV through EventProperties.SetProperty API.
        static constexpr const char* PropertyName = "__TlgCV__";

        // String constant to use when sending a CV through any transport other than
        // this telemetry API, e.g. as a name for an HTTP header.
        static constexpr const char* HeaderName = "MS-CV";

    private:

        // Version specific constants.
        static const size_t c_maxCVLength_v1;
        static const size_t c_baseCVLength_v1;

        static const size_t c_maxCVLength_v2;
        static const size_t c_baseCVLength_v2;

        // Helper strings used for input validation.
        static const std::string s_base64CharSet;
        static const std::string s_base10CharSet;
        static const std::string s_maxVectorElementValue;

        // Internal state variables.
        mutable std::mutex m_lock;
        bool m_isInitialized;
        std::string m_baseVector;
        unsigned int m_currentVector;
        size_t m_maxLength;

        // Randomly generates a string for the base vector.
        static std::string initializeInternal(size_t baseLength);

        // Internal, unsynchronized class method implementations.
        std::string getValueInternal() const;
        bool incrementInternal();
        size_t getLengthInternal(unsigned int vectorValue);
        bool canExtendInternal();
        bool canIncrementInternal();

        // Calculates the length of the specified integer.
        size_t getDigitCount(unsigned int value);
    };
}
