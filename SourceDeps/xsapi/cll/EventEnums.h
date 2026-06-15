// Copyright (c) Microsoft. All rights reserved.

// Defines the enumerations used to log the events

#pragma once

namespace cll
{
    // Event Flags
    // See https://osgwiki.com/wiki/CommonSchema/flags

    // Event Latency (Normal or Realtime) controls whether events has to be uploaded immediately or
    // could wait to be batched with other events.
    // See https://osgwiki.com/wiki/Common_Schema_Event_Latency
    enum Latency
    {
        LatencyUnspecified = 0,
        LatencyNormal = 0x0100,
        LatencyRealtime = 0x0200
    };

    // Event Persistence (Normal or Critical) controls which events to delete first if we are out of space.
    // See https://osgwiki.com/wiki/Common_Schema_Event_Persistence
    enum Persistence
    {
        PersistenceUnspecified = 0,
        PersistenceNormal = 0x01,
        PersistenceCritical = 0x02
    };

    // Event Sensitivity controls how sensitive the event is. None means it goes to the XPert and normal
    // CLL journals, Flag means it goes to UserSensitive journal, Hash means the PII info - user id, device id,
    // etc in the event needs to be hashed and there should be a separate epoch and seqNum and we should never
    // batch these with low-sensitivity, Drop means similar measures as Hash except that the Part A PII fields
    // would be dropped, not hashed.
    // See https://osgwiki.com/wiki/Common_Schema_Event_Sensitivity
    enum Sensitivity
    {
        // this value is not part of the Common Schema spec but it is necessary
        // for the callers to signal "use the default sensitivity from metadata or settings"
        // if nether sources specifies sensitivity it will eventually default to None
        SensitivityUnspecified = 0x000001,

        SensitivityNone = 0x000000,
        SensitivityMark = 0x080000,
        SensitivityHash = 0x100000,
        SensitivityDrop = 0x200000
    };

    // Sample Rate (0% - 100%) determines the percentage of devices randomly sampled in for this event collection
    // based on their device.localId. The smallest sampleRate step 0.001 (%).
    // SampleRate_Epsilon is used as an accuracy limit for floating point number comparisions.
    const double SampleRate_NoSampling = 100.0;
    const double SampleRate_10_percent = 10.0;
    const double SampleRate_0_percent = 0.0;
    const double SampleRate_Unspecified = -1.0;
    const double SampleRate_Epsilon = 0.00001;
}
