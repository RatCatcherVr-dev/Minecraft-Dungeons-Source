// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#if !XSAPI_NO_PPL

#include "xsapi/system.h"
#include "xsapi/multiplayer.h"
#include "xsapi/title_storage.h"
#include "xsapi/profile.h"
#include "xsapi/privacy.h"
#include "xsapi/leaderboard.h"
#include "xsapi/social.h"
#include "xsapi/presence.h"
#include "xsapi/achievements.h"
#include "xsapi/matchmaking.h"
#include "xsapi/user_statistics.h"
#include "xsapi/string_verify.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_BEGIN

/// <summary>
/// Defines pointers to objects that access Xbox Live to create features for player 
/// interactions.
///
/// Note: the XboxLiveContext is unique per instance. Changing state on one instance for a 
/// user does not affect a second instance of the context for the same user. Using multiple
/// instances can therefore result in unexpected behavior. Titles should ensure to only use 
/// one instance of the XboxLiveContext per user.
/// </summary>
class xbox_live_context
{
public:
    /// <summary>
    /// Creates an xbox_live_context from a xbox_live_user
    /// </summary>
    _XSAPIIMP inline xbox_live_context(
        _In_ XblUserHandle user
    );

    /// <summary>
    /// Returns the associated system User.
    /// </summary>
    _XSAPIIMP inline XblUserHandle user();

    /// <summary>
    /// Returns the current user's Xbox Live User ID.
    /// </summary>
    _XSAPIIMP inline string_t xbox_live_user_id();

    /// <summary>
    /// Returns an object containing settings that apply to all REST calls made such as retry and diagnostic settings.
    /// </summary>
    _XSAPIIMP inline std::shared_ptr<xbox_live_context_settings> settings();

    /// <summary>
    /// Returns an object containing Xbox Live app config such as title ID
    /// </summary>
    _XSAPIIMP inline std::shared_ptr<xbox_live_app_config> application_config();

    /// <summary>
    /// A service for storing data in the cloud.
    /// </summary>
    _XSAPIIMP inline title_storage::title_storage_service title_storage_service();

    /// <summary>
    /// A service for managing user profiles.
    /// </summary>
    _XSAPIIMP inline social::profile_service profile_service();

    /// <summary>
    /// A service for managing privacy settings.
    /// </summary>
    _XSAPIIMP inline privacy::privacy_service privacy_service();

#if !defined(XBOX_LIVE_CREATORS_SDK)
    /// <summary>
    /// A service for managing leaderboards.
    /// </summary>
    _XSAPIIMP inline leaderboard::leaderboard_service leaderboard_service();

    /// <summary>
    /// A service for managing social networking links.
    /// </summary>
    _XSAPIIMP inline social::social_service social_service();

    /// <summary>
    /// A service for managing reputation reports.
    /// </summary>
    _XSAPIIMP inline social::reputation_service reputation_service();

    /// <summary>
    /// A service for managing achievements.
    /// </summary>
    _XSAPIIMP inline achievements::achievement_service achievement_service();

    /// <summary>
    /// A service for managing user statistics.
    /// </summary>
    _XSAPIIMP inline user_statistics::user_statistics_service user_statistics_service();

    /// <summary>
    /// A service for managing multiplayer games.
    /// </summary>
    _XSAPIIMP inline multiplayer::multiplayer_service multiplayer_service();

    /// <summary>
    /// A service for managing matchmaking sessions.
    /// </summary>
    _XSAPIIMP inline matchmaking::matchmaking_service matchmaking_service();

    /// <summary>
    /// A service for managing real-time activity.
    /// </summary>
    _XSAPIIMP inline std::shared_ptr<real_time_activity::real_time_activity_service> real_time_activity_service();

    /// <summary>
    /// A service used to check for offensive strings.
    /// </summary>
    _XSAPIIMP inline system::string_service string_service();

    /// <summary>
    /// A service for managing Rich Presence.
    /// </summary>
    _XSAPIIMP inline presence::presence_service presence_service();

#if HC_PLATFORM == HC_PLATFORM_WIN32
    /// <summary>
    /// A service for receiving notifications.
    /// </summary>
    _XSAPIIMP inline std::shared_ptr<notification::notification_service> notification_service();
#endif

#if XSAPI_EVENTS_SERVICE
    /// <summary>
    /// A service used to write in game events.
    /// </summary>
    _XSAPIIMP inline events::events_service events_service();
#endif // !XDK_API && !XSAPI_UNIT_TESTS

#endif // !defined(XBOX_LIVE_CREATORS_SDK)

    /// <summary>
    /// Internal function
    /// </summary>
    inline xbox_live_context(_In_ XblContextHandle xboxLiveContextHandle);
    inline ~xbox_live_context();

private:
    XblContextHandle m_handle = nullptr;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_CPP_END

#endif

#if !XSAPI_NO_PPL
#include "impl/xbox_live_context.hpp"
#endif