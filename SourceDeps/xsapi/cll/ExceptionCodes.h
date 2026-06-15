// Copyright (c) Microsoft. All rights reserved.

// Return and Exception codes for the logging library.

#pragma once

enum ExceptionCodes
{
    // Success Error Codes
    Ok = 0,

    // Exception Error Codes
    InitializationException = 1000,
    TimerStartException = 1001,
    TimerStopException = 1002,
    NetworkSendException = 1003,
    GenericLogException = 1004,
    BadJsonInSettingsFile = 1005,
    BadEventFormat = 1006,
    EventIsTooLarge = 1007,

    // Failure Error Codes (Not exceptions)
    NotRunning = 2000,
    SettingsFileEmpty = 2001,
    NotInitialized = 2002,
    NotStarted = 2003,
    AlreadyStarted = 2004,
    AlreadyInitialized = 2005,
    MaxEventCountIsReached = 2006,
};
