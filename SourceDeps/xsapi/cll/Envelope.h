// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "CllPartA.h"
#include "EventEnums.h"

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace cll
{
    // Describes the User related fields. See https://osgwiki.com/wiki/CommonSchema/user_id"
    struct UserExtension
    {
        // Unique user Id. Clients aren't expected to set this; instead the service will decide the best ID to use here.
        // Clients may set this if they believe they have the best user ID already.
        // Format is <NamespaceIdentifier>:<Id> for example, x:12345678.
        std::string id;
        
        // Local user identifier according to the client. Format is <NamespaceIdentifier>:<Id> for example, x:12345678.
        std::string localId;
        
        // This is the ID of the user associated with this event, deduced from a token such as an MSA ticket or Xbox xtoken.
        std::string authId;
    };
    
    // Describes the device related fields. See https://osgwiki.com/wiki/CommonSchema/device_id
    struct DeviceExtension
    {
        // Unique device Id.
        // Clients aren't expected to set this; instead the service will decide the best ID to use here.
        // Clients may set this if they believe they have the best device ID already.
        // Format is <NamespaceIdentifier>:<Id> for example, x:12345678.
        std::string id;
        
        // Local device identifier according to the client. Format is <NamespaceIdentifier>:<Id> for example, x:12345678.
        std::string localId;
        
        // This is the ID of the device associated with this event, deduced from a token such as an MSA ticket or Xbox xtoken.
        // For MSA tickets this is expected to be the MSA Global ID.
        std::string authId;
        
        // This is the secondary ID of the device associated with this event, deduced from a token such as an MSA ticket or Xbox xtoken.
        // For MSA tickets this is expected to be the MSA Hardware ID.
        std::string authSecId;
        
        // Platform of the device.
        std::string deviceClass;
    };
    
    // Describes the OS properties that would be populated by the client.
    struct OsExtension
    {
        // OS locale, set by the user, in the Windows locale format.
        // Example, en-US for US English. Refer RFC 4646 for the format.
        std::string locale;
        
        // Comma delimited list of experiment ids for experiments installed on the OS.
        // Format is <NamespaceIdentifier>:<ExperimentId> for example, m:12345.
        std::string expId;
    };
    
    // Describes the properties of the running application.
    // This extension could be populated by a client app or a web app.
    struct AppExtension
    {
        // Comma delimited list of experiment ids for experiments installed on the Application.
        // Format is <NamespaceIdentifier>:<ExperimentId> for example, m:12345.
        std::string expId;
        
        // The userId as known by the application
        std::string userId;
    };

    // Describes the fields related to the ios logging library implementation.
    struct IosExtension
    {
        // Logging Library version
        std::string libVer;
        
        // Array of strings that refer back to a key in the X-Tickets http header that the client uploaded along with a batch of events
        std::vector<std::string> ids;
    };

    // Event envelope, class storing all of the event fields.
    struct Envelope
    {
        // Part A fields
        std::string ver;
        std::string name;
        std::string time;
        double popSample;
        std::string epoch;
        int64_t seqNum;
        std::string iKey;
        int64_t flags;
        std::string os;
        std::string osVer;
        std::string appId;
        std::string appVer;
        std::string cV;
        std::map<std::string, std::string> tags;
        
        // extensions
        UserExtension userExt;
        DeviceExtension deviceExt;
        OsExtension osExt;
        AppExtension appExt;
        IosExtension iosExt;
        
        // JSON serialized event Part B and Part C fields
        std::string data;
        
        // auxiliary fields, duplicated some of the part A fields for easier access
        Latency latency;
        Persistence persistence;
        Sensitivity sensitivity;
        std::vector<TicketData> tickets;

        // more auxilary fields
        double sampleId;
        
        Envelope();
        std::string serializeToJson();
    };
}
