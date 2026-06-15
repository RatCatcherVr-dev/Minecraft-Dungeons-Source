// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include <string>
#include "EventEnums.h"

namespace cll
{
    struct ApplicationData
    {
        std::string iKey;
        std::string appId;
        std::string appVer;
    };

    struct OsData
    {
        std::string osName;
        std::string osVer;
        std::string osLocale;
    };

    struct DeviceData
    {
        std::string deviceId;
        std::string deviceClass;

        // Persistent (stable, but possibly resettable by user or on schedule) hashed id base used to derive all other
        // identifiers such as device id, user id, epoch) when requested by the application.
        // This value should not otherwise be logged or be related to any other value sent with the telemetry.
        std::string hashedIdBase;
    };

    struct UserData
    {
        std::string userId;
        std::string appUserId;
        std::string experimentId;
    };

    enum TicketType
    {
        TicketTypeUnspecified = 0,
        TicketTypeMsaUser = 0x01,
        TicketTypeMsaDevice = 0x02,
        TicketTypeXauthUser = 0x03,
        TicketTypeXauthDevice = 0x04
    };

    struct TicketData
    {
        // Type of the ticket (MSA/Xauth) and whether it is specific to the user or device.
        // User tickets may contain device claims which will be used where available.
        // Device tickets may contain user claims but these claims would be ignored by the server.
        // Device tickets could be used to re-validate the expired user tickets.
        TicketType ticketType;

        // A "nickname" for the ticket which is used to reference the ticket header in the event body.
        // These must be unique accross the events in the same batch,
        // It must not contain any PII or sensitive information.
        std::string ticketId;
        
        // The encrypted value of the ticket.
        std::string ticketValue;
    };

    struct EventMetadata
    {
        Latency latency;
        Persistence persistence;
        Sensitivity sensitivity;
        double sampleRate;
    };

    struct CllPartA
    {
        ApplicationData m_appData;
        OsData m_osData;
        UserData m_userData;
        DeviceData m_deviceData;
    };
}