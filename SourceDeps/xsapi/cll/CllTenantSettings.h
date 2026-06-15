// Copyright (c) Microsoft. All rights reserved.

#pragma once

#include <atomic>
#include <iostream>
#include <map>
#include <random>
#include <mutex>

#include "EventEnums.h"
#include "CllEvent.h"
#include "CllPartA.h"
#include "CllUploadRequestData.h"
#include "Envelope.h"
#include "CorrelationVector.h"

namespace cll
{
    class CllTenantSettings
    {
    public:

        // constructor (requires all overridable Part A data to be provided by the caller)
        CllTenantSettings(const CllPartA& partA);

        //
        // After this class is constructed it must be populated with the cloud settings.
        //
        // These settings need to be retrieved on application start by invoking HTTPS GET method to download
        // the JSON resources from:
        // 1) CLL settings URL, and
        // 2) Application settings URL
        //
        // On successful call to retrieve settings the app would receive a JSON-serialize object which has these fields:
        // {
        //     "refreshInterval":"25",
        //     "queryUrl":"/settings/v2.0/androidLL/app",
        //     "settings":
        //      {
        //          "settings1":"value1",                    <= string-to-string map with settings
        //          ...
        //      }
        //
        // String-to-string map inside of the "settings" child of the received JSON object is then passed to
        // initializeCllSettings or initializeAppSettings respectively.
        //
        // Application should also cache these settings where possible to make sure it can access them later if after restart
        // network connection is initially unavailable so that it can start collecting telemetry sooner.
        //

        // Returns URL for downloading the CLL settings
        std::string getCllSettingsUrl();

        // Returns URL for downloading the application settings
        std::string getAppSettingsUrl();
        
        // Initialize or update with the retrieved CLL and application settings
        // by parsing the "settings" child in server response JSON as a string-to-string map.
        void initializeCllSettings(const std::map<std::string, std::string>& cllSettings);
        void initializeAppSettings(const std::map<std::string, std::string>& appSettings);

        bool isInitialized();

    public:

        //
        // NOTE: Public methods below could only be used after this object instance was successfully initialized.
        //

        // Get the minimal recommended interval until the next check to download settings.
        // Checking settings too often could overload the settings cloud service.
        int getSettingsRefreshIntervalInMinutes();

        // overridable Part A field accessors
        void setExperimentId(const std::string& experimentId);
        void setAppUserId(const std::string& userId);
        std::string getAppUserId();

        // The following 3 settings are used in a formula to calculate the "exponential back off" retry interval
        // in case of non-realtime event upload failure.
        //
        // The actual formula is:
        // int interval = BASERETRYPERIOD * CONSTANTFORRETRYPERIOD ^ (rand : between 0 and power - 1));
        // where power starts from 1 and increments with each retry attempt as long as the twice the maximum interval
        // for the next retry would be smaller than MAXRETRYPERIOD.
        int getBaseRetryPeriod();
        int getConstantForRetryPeriod();
        int getMaxRetryPeriod();

        // Maximum size of the serialized event UTF-8 string in bytes.
        long getMaxEventSizeInBytes();

        // Maximum number of events per upload request.
        long getMaxEventsPerPost();

        // Maximum amount of disk space the CLL can use to store the events pending retry and upload before
        // it starts deleting the oldest files.
        long getMaxFilesSpace();

        // Maximum number of non-critical events in memory queue before it is serialized to disk.
        long getNormalEventMemoryQueueSize();

        // Maximum amount of time an event with normal latency would have to wait before we attempt to upload it.
        int getQueueDrainInterval();

        // Frequency of CLL internal snapshot (heartbeat) event generation, in minutes.
        int getSnapshotScheduleInterval();

        // Method used to populate a new CllEvent which is used to pass event data to CLL
        // cllEvent - reference to a new default-constructed instance of CllEvent
        // iKey - the iKey for the event, can be empty, in which case the iKey in the part a data will be used
        // name - fully-qualified event name, following the CS 2.1 notation of CompanyName.FeatureArea.EventName
        // data - JSON-serialized UTF-8 part B and C event fields
        // utcTimestamp - ISO 8601-formatted millisecond-accurate UTC timestamp of the event (YYYY-MM-DDTHH:MM:SS.fffffffffZ)
        // latency - latency level - Realtime events are sent out asap while normal latency events are batch before sending.
        // persistence - persistence level - Critical persistence events must be immediately written to the persistent storage.
        // sensitivity - sensitiviy flags of this event
        // sampleRate - desired sampling rate for this event.
        // tickets - list of user/device MSA/Xauth tickets associated with this event.
        // cv - instance of CorrelationVector to be sent with the event; ignored if not initialized.
        void populateEvent(CllEvent& cllEvent, const std::string& iKey, const std::string& name, const std::string& data, const std::string& utcTimestamp, Latency latency, Persistence persistence, Sensitivity sensitivity, double sampleRate, const std::vector<TicketData>& tickets, CorrelationVector const& cv);
        
        // Method used to add the populated CllEvent to an instance of CllUploadRequestData
        // success error code indicates success, failure means that the batch is full and is ready to be sent or
        // that the event size was too big, see ExceptionCodes.h.
        int addEventToRequest(const CllEvent& cllEvent, CllUploadRequestData& requestData);

        // Indicates whether the CLL may upload data from this device to Vortex.
        // This value together with CllEvent::inSample flag must be checked before upload events to Vortex.
        bool isUploadEnabled();

        // Event upload URL
        std::string getEventUploadUrl();
        
        // Method used to test setting initialization.
        std::string getCllSetting(const std::string& settingName);
        Latency getLatencyForEvent(const std::string& eventName, Latency defaultLatency);
        Persistence getPersistenceForEvent(const std::string& eventName, Persistence defaultPersistence);
        Sensitivity getSensitivityForEvent(const std::string& eventName, Sensitivity defaultSensitivity);
        double getSampleRateForEvent(const std::string& eventName, double defaultSampleRate);

    private:

        // setting names
        static constexpr const char* CLL_SETTINGS_APPSETTINGSURL = "APPSETTINGSURL";
        static constexpr const char* CLL_SETTINGS_BASERETRYPERIOD = "BASERETRYPERIOD";
        static constexpr const char* CLL_SETTINGS_CLLSETTINGSURL = "CLLSETTINGSURL";
        static constexpr const char* CLL_SETTINGS_CONSTANTFORRETRYPERIOD = "CONSTANTFORRETRYPERIOD";
        static constexpr const char* CLL_SETTINGS_HTTPTIMEOUTINTERVAL = "HTTPTIMEOUTINTERVAL";
        static constexpr const char* CLL_SETTINGS_MAXEVENTSIZEINBYTES = "MAXEVENTSIZEINBYTES";
        static constexpr const char* CLL_SETTINGS_MAXEVENTSPERPOST = "MAXEVENTSPERPOST";
        static constexpr const char* CLL_SETTINGS_MAXFILESSPACE = "MAXFILESSPACE";
        static constexpr const char* CLL_SETTINGS_MAXRETRYPERIOD = "MAXRETRYPERIOD";
        static constexpr const char* CLL_SETTINGS_NORMALEVENTMEMORYQUEUESIZE = "NORMALEVENTMEMORYQUEUESIZE";
        static constexpr const char* CLL_SETTINGS_QUEUEDRAININTERVAL = "QUEUEDRAININTERVAL";
        static constexpr const char* CLL_SETTINGS_SNAPSHOTSCHEDULEINTERVAL = "SNAPSHOTSCHEDULEINTERVAL";
        static constexpr const char* CLL_SETTINGS_SYNCREFRESHINTERVAL = "SYNCREFRESHINTERVAL";
        static constexpr const char* CLL_SETTINGS_UPLOADENABLED = "UPLOADENABLED";
        static constexpr const char* CLL_SETTINGS_VORTEXPRODURL = "VORTEXPRODURL";

        int tryGetIntCllSetting(const std::string& settingName);
        long tryGetLongCllSetting(const std::string& settingName);
        void populateKnownCllSettings();
        void populateEnvelope(Envelope& envelope, CorrelationVector const& cv, const std::string& iKey, const std::string& name, const std::string& data, const std::string& utcTimestamp, Latency latency, Persistence persistence, Sensitivity sensitivity, double sampleRate, const std::vector<TicketData>& tickets);
        double calculateSampleId(const std::string& deviceId);
        void validatePartA(const CllPartA& partA);
        bool isValidIKey(const std::string& iKey);
        std::string overwriteIfNotEmpty(const std::string& value, const std::string& newValue);
        void getRandomDeviceId(std::string& deviceId, double& sampleId);
        std::string getCloudSetting(const std::string& qualifiedEventName, const std::string& settingName);

    private:

        std::mutex m_lock;
        bool m_isInitialized;
        int m_initalizationState;
        std::random_device m_randomDevice;
        std::map<std::string, std::string> m_defaultCllSettings;

        // Cloud settings for the CLL and the application
        std::map<std::string, std::string> m_cllSettings;
        std::map<std::string, std::string> m_appSettings;

        // Built-in Part A
        std::string m_libraryVersion;
        std::string m_csVer;

        // Raw and hashed data sessions have separate epoch, seqNum, sample, device and user ids
        std::string m_rawEpoch;
        std::string m_hashedEpoch;
        std::atomic<std::uint32_t> m_rawSeqNum;
        std::atomic<std::uint32_t> m_hashedSeqNum;
        double m_rawSampleId;
        double m_hashedSampleId;
        
        // Overridable Part A
        ApplicationData m_appData;
        OsData m_osData;
        DeviceData m_rawDeviceData;
        DeviceData m_hashedDeviceData;
        UserData m_rawUserData;
        UserData m_hashedUserData;

        // cached parsed setting values
        int m_baseRetryPeriod;
        int m_constantForRetryPeriod;
        long m_maxEventSizeInBytes;
        long m_maxEventsPerPost;
        long m_maxFilesSpace;
        int m_maxRetryPeriod;
        long m_normalEventMemoryQueueSize;
        int m_queueDrainInterval;
        int m_snapshotScheduleInterval;
        int m_syncRefreshInterval;
        std::string m_eventUploadUrl;
        bool m_isUploadEnabled;
    };
}
