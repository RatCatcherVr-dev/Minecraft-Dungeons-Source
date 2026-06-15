// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include "CllEvent.h"
#include "EventEnums.h"

#include <string>
#include <map>
#include <sstream>

namespace cll
{
    // This class is used to prepare an event batch upload request.
    //
    // In addition to the headers returned by getHeaders() method it is recommended that
    // the application also compresses the payload body using deflate algorithm
    // and then adds the following headers as well:
    // "Accept-Encoding: gzip, deflate"
    // "Content-Encoding: deflate"
    //
    // Server response protocol is described in https://osgwiki.com/wiki/Vortex/ClientProtocol
    // Here are the guidelines some of the error codes:
    // 200 - some or all of the events were successfully parsed and accepted, do not retry.
    //       Reponse body contains Json with the following root fields
    //          "acc" specifying the number of accepted events and
    //          "rej" specifying the number of rejected events
    // 400 - Bad Request - all of the events were rejected because of JSON parsing errors, do not retry.
    // 401 - Unauthorized - some of the tickets attached to the upload request was invalid
    //       (expired or issued for a wrong site)
    //       Do not retry unless you know that the issue with tickets was resolved.
    // 413 - Request too large - implementation error, too many events were batched or
    //       events were too big. Do not retry unless you reduced the batch size.
    // 415 - Wrong Content - Type - post request had an invalid Content-Type header. Do not retry.
    // 429 - Too Many Requests - retry after the duration in minutes specifyied in Retry-After
    //       header in server response.
    // 5xx - server-side error, retry using exponential backoff algorithm.
    // 503 - similar to 429 and will also contain the recommended Retry-After value in minutes.
    //

    class CllUploadRequestData
    {
    public:
        CllUploadRequestData();

        // add event to payload
        void addEvent(const CllEvent& cllEvent);
        int getEventCount();

        // get the payload body and headers
        std::string getRequestBody();
        size_t getRequestBodyLength();
        std::vector<std::pair<std::string, std::string>> getHeaders();
        const std::vector<TicketData>& getAllTickets();

    private:

        int eventCount;
        std::ostringstream ossEventBuffer;
        std::map<std::string, std::string> tickets;
        std::string msaDeviceTicket;
        std::string xauthDeviceTicket;
        std::vector<TicketData> allTickets;
    };
}
