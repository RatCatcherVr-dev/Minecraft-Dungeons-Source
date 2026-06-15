// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include <mutex>
#include "xsapi-c/real_time_activity_c.h"

namespace xbox { namespace services {
    class xbox_live_context;
}}

namespace xbox { namespace services { 
    /// <summary>
    /// Contains classes and enumerations that let you subscribe
    /// to real time activity information for a player statistic
    /// on Xbox Live.
    /// </summary>
    namespace real_time_activity {

/// <summary>
/// Enumeration for the message types for the Xbox Live service.
/// The value of each enum should not be changed.
/// </summary>
enum class real_time_activity_message_type
{
    /// <summary>
    /// Indicates that this is a Subscribe message.
    /// </summary>
    subscribe = 1,

    /// <summary>
    /// Indicates that this is a Unsubscribe message.
    /// </summary>
    unsubscribe = 2,

    /// <summary>
    /// Indicates that this is a ChangeEvent message.
    /// </summary>
    change_event = 3,

    /// <summary>
    /// Indicates that this is a Resync message.
    /// </summary>
    resync = 4
};

/// <summary>
/// Enumeration for the possible states of a statistic subscription request
/// to the real-time activity service.
/// </summary>
enum real_time_activity_subscription_state
{
    /// <summary>
    /// The subscription state is unknown.
    /// </summary>
    unknown,

    /// <summary>
    /// Waiting for the server to respond to the subscription request.
    /// </summary>
    pending_subscribe,

    /// <summary>
    /// Subscription confirmed.
    /// </summary>
    subscribed,

    /// <summary>
    /// Waiting for the server to respond to the unsubscribe request.
    /// </summary>
    pending_unsubscribe,

    /// <summary>
    /// Unsubscribe confirmed.
    /// </summary>
    closed
};

/// <summary>
/// Enumeration for the possible connection states of the connection
/// to the real-time activity service.
/// </summary>
enum real_time_activity_connection_state
{
    /// <summary>
    /// Currently connected to the real-time activity service.
    /// </summary>
    connected,
    
    /// <summary>
    /// Currently connecting to the real-time activity service.
    /// </summary>
    connecting,

    /// <summary>
    /// Currently disconnected from the real-time activity service.
    /// </summary>
    disconnected
};

/// <summary>
/// The base class for real time activity subscriptions.
/// </summary>
class real_time_activity_subscription
{
public:
    /// <summary>The state of the subscription request.</summary>
    inline real_time_activity_subscription_state state() const;

    /// <summary>The resource uri for the request.</summary>
    inline const string_t& resource_uri() const;

    /// <summary>The unique subscription id for the request.</summary>
    inline uint32_t subscription_id() const;
    
    inline real_time_activity_subscription(XblRealTimeActivitySubscriptionHandle handle);

protected:
    string_t m_resourceUri;
    XblRealTimeActivitySubscriptionHandle m_handle;
};

class real_time_activity_subscription_error_event_args
{
public:
    /// <summary>
    /// The subscription this refers to
    /// </summary>
    inline const real_time_activity_subscription& subscription();

    /// <summary>
    /// The error returned by the operation.
    /// </summary>
    inline std::error_code err() const;

    /// <summary>
    /// The error message
    /// </summary>
    _XSAPIIMP_DEPRECATED inline std::string err_message() const;

    /// <summary>
    /// Internal function
    /// </summary>
    inline real_time_activity_subscription_error_event_args(
        XblRealTimeActivitySubscriptionHandle subscriptionHandle,
        HRESULT subscriptionError
    );

private:
    real_time_activity_subscription m_subscription;
    std::error_code m_err;
};

/// <summary>
/// Represents a client side service that handles connections and communications with
/// the Xbox Live real-time activity service.
/// </summary>
class real_time_activity_service : public std::enable_shared_from_this<real_time_activity_service>
{
public:
    /// <summary>
    /// Starts a background task that creates and initializes a websocket connection to the Xbox Live real-time activity service.
    /// Its recommended that titles do not activate more than MAXIMUM_WEBSOCKETS_ACTIVATIONS_ALLOWED_PER_USER (5) per user per title instance.
    /// Upon reaching the limit, titles will hit an assert during development that can be temporarily disabled via
    /// xbox_live_context_settings::disable_asserts_for_maximum_number_of_websockets_activated().
    /// </summary>
    inline void activate();

    /// <summary>
    /// Cancels all existing subscriptions to the Xbox Live real-time activity service,
    /// unhooks from the websocket connection, and stops the background task.
    /// </summary>
    inline void deactivate();

    /// <summary>
    /// Registers a handler function to receive a notification that is sent when the client service
    /// loses or gains connectivity to the real time activity service.
    /// Event handlers receive a real_time_activity_connection_state object.
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    /// <returns>
    /// A function_context object that can be used to unregister the event handler.
    /// </returns>
    inline function_context add_connection_state_change_handler(_In_ std::function<void(real_time_activity_connection_state)> handler);

    /// <summary>
    /// Unregisters an event handler for real time activity connectivity state changes.
    /// </summary>
    /// <param name="remove">The function_context object that was returned when the event handler was registered. </param>
    inline void remove_connection_state_change_handler(_In_ function_context remove);

    /// <summary>
    /// Registers a handler function to receive a notification that is sent when there is an
    /// error in the real time activity service.
    /// Event handlers receive a real_time_activity_subscription_error_event_args&amp; object.
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    /// <returns>
    /// A function_context object that can be used to unregister the event handler.
    /// </returns>
    inline function_context add_subscription_error_handler(_In_ std::function<void(const real_time_activity_subscription_error_event_args&)> handler);

    /// <summary>
    /// Unregisters an event handler for real time activity error notifications.
    /// </summary>
    /// <param name="remove">The function_context object that was returned when the event handler was registered. </param>
    inline void remove_subscription_error_handler(_In_ function_context remove);

    /// <summary>
    /// Registers a handler function to receive a notification that is sent when there is a
    /// resync message from the real time activity service.
    /// This message indicates that data may have been lost and to resync all data by calling
    /// corresponding REST API's
    /// </summary>
    /// <param name="handler">The callback function that receives notifications.</param>
    /// <returns>
    /// A function_context object that can be used to unregister the event handler.
    /// </returns>
    inline function_context add_resync_handler(_In_ std::function<void()> handler);

    /// <summary>
    /// Unregisters an event handler for real time activity resync notifications.
    /// </summary>
    /// <param name="remove">The function_context object that was returned when the event handler was registered. </param>
    inline void remove_resync_handler(_In_ function_context remove);

    inline real_time_activity_service(const real_time_activity_service& other);
    inline real_time_activity_service& operator=(real_time_activity_service other);
    inline ~real_time_activity_service();

private:
    inline real_time_activity_service(_In_ XblContextHandle contextHandle);

    XblContextHandle m_xblContext;
    struct HandlerContext;

    friend xbox_live_context;
};

}}}

#include "impl/real_time_activity.hpp"