// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include <xsapi/types.h>
#include <xsapi/system.h>
#include <xsapi/multiplayer.h>

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_BEGIN

#if HC_PLATFORM == HC_PLATFORM_WIN32

class invite_notification_event_args
{
public:
    const string_t& invited_xbox_user_id() const { return m_invitedXboxUserId; }
    const string_t& sender_xbox_user_id() const { return m_senderXboxUserId; }
    const string_t& sender_gamertag() const { return m_senderGamertag; }
    const string_t& invite_handle_id() const { return m_inviteHandleId; }
    const string_t& invite_protocol() const { return m_inviteProtocol; }
    const utility::datetime& expiration() const { return m_expiration; }
    const multiplayer::multiplayer_session_reference session_reference() const { return m_sessionReference; }

    invite_notification_event_args() {}

    /// <summary>
    /// Internal function
    /// </summary>
    HRESULT deserialize(const rapidjson::GenericValue<rapidjson::UTF8<>, JsonAllocator>& root);

 private:
    string_t m_invitedXboxUserId;
    string_t m_senderXboxUserId;
    string_t m_senderGamertag;
    string_t m_inviteHandleId;
    string_t m_inviteProtocol;
    utility::datetime m_expiration;
    multiplayer::multiplayer_session_reference m_sessionReference;
};

class achievement_unlocked_notification_event_args
{
public:
    const string_t& name() const { return m_name; }
    const string_t id() const { return m_id; }
    const string_t& icon_url() const { return m_iconUrl; }
    const uint32_t gamerscore() const { return m_gamerscore; }
    const string_t& deeplink() const { return m_deeplink; }
    const string_t& xbox_user_id() const { return m_xboxUserId; }
    const utility::datetime& unlockTime() const { return m_unlockTime; }

    achievement_unlocked_notification_event_args() {};

    /// <summary>
    /// Internal function
    /// </summary>
    HRESULT deserialize(const rapidjson::GenericValue<rapidjson::UTF8<>, JsonAllocator>& root);

private:

    string_t m_name;
    string_t m_id;
    string_t m_iconUrl;
    uint32_t m_gamerscore{ 0 };
    string_t m_deeplink;
    string_t m_xboxUserId;
    utility::datetime m_unlockTime;
};

#endif // WIN32

enum notification_filter_source_type
{
    media_presence = 1,
    presence_online = 2,
    broadcast = 3,
    message = 4,
    party_invite_360 = 5,
    multiplayer = 6,
    achievements = 8
};

struct notification_filter
{
    notification_filter_source_type sourceType;
    uint32_t type;
};

class notification_service : public std::enable_shared_from_this<notification_service>
{
public:
    notification_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::XboxLiveContextSettings> xboxLiveContextSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    virtual ~notification_service() = default;

    virtual pplx::task<xbox_live_result<void>> subscribe_to_notifications(
#if HC_PLATFORM == HC_PLATFORM_WIN32 && !XSAPI_UNIT_TESTS
        _In_ const std::function<void(achievement_unlocked_notification_event_args&)>& achievementUnlockHandler,
        _In_ const std::function<void(invite_notification_event_args&)>& multiplayerInviteHandler
#endif
    ) = 0;

    virtual pplx::task<xbox_live_result<void>> unsubscribe_from_notifications();
    virtual pplx::task<xbox_live_result<void>> unsubscribe_from_notifications(_In_ const string_t& endpointId);

protected:
    static pplx::task<xbox_live_result<void>> subscribe_to_notifications_helper(
        _In_ std::weak_ptr<notification_service> thisWeakPtr,
        _In_ const string_t& applicationInstanceId,
        _In_ const string_t& uriData,
        _In_ const string_t& platform,
        _In_ const string_t& deviceName,
        _In_ const string_t& platformVersion,
        _In_ const std::vector<notification_filter> notificationFilterEnum
        );

    pplx::task<xbox_live_result<void>> unsubscribe_from_notifications_helper(
        _In_ const string_t& endpointId
        );

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::XboxLiveContextSettings> m_xboxLiveContextSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;
    std::unordered_map<string_t, std::shared_ptr<xbox::services::user_context>> m_userContexts;
    
    bool m_isInitialized = false;
    string_t m_endpointId;
};

// This only exists because the notification_service hasn't been converted to 
// be header only in the C++ public headers and should be removed after than happens.
std::shared_ptr<notification::notification_service> XblContextGetNotificationService(XblContext * context);

NAMESPACE_MICROSOFT_XBOX_SERVICES_NOTIFICATION_CPP_END

