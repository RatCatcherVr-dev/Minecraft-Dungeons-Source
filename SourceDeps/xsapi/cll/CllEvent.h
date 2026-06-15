// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include <string>
#include <vector>
#include "EventEnums.h"
#include "CllPartA.h"

namespace cll
{
    struct CllEvent
    {
    public:

        // Indicates whether this event is to be uploaded to Vortex.
        // Some of the events may have a non-100% sampling rate which would cause them to be excluded
        // from the uploads on certain devices.
        bool inSample{ false };

        // Event metadata properties, see EventEnums.h for the additional info.
        Latency latency{ LatencyUnspecified };
        Persistence persistence{ PersistenceUnspecified };
        Sensitivity sensitivity{ SensitivityUnspecified };

        // Event body JSON serialized as UTF-8 string.
        std::string data;

        // Optional list of MSA/Xauth user/device tickets corresponding to this event.
        std::vector<TicketData> tickets;

    public:
        // Size of the serialized event, including headers.
        long getSerializedSize() const;

        // Serialize event for temporary storage (e.g. on disk).
        // This is not the same format as used to upload events to Vortex.
        std::string serialize() const;

        // Deserialize the event read from temporary storage.
        int deserialize(const std::string& inputData);
    };
}
