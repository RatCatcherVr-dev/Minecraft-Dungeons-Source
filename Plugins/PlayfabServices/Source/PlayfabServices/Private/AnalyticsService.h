#pragma once

#include "AnalyticsEvent.h"
#include "CommonProperties.h"
#include "PlayfabPlayerCache.h"
#include "PlayfabClient.h"

#include <deque>
#include <mutex>
#include <memory>
#include <UnrealString.h>

struct EventGroup
{
	EventGroup() = default;
	EventGroup(AnalyticsEvent initialEvent)
	{
		events.push_back(initialEvent);
	}
	std::deque<AnalyticsEvent> events{};
	mutable std::mutex mutex{};

	EventGroup(const EventGroup&) = delete;
	EventGroup& operator=(const EventGroup&) = delete;
};
using EventList = TMap<FString, std::unique_ptr<EventGroup>>;

class AnalyticsEvent;

class AnalyticsService 
{
public:
   AnalyticsService(FString cachFile, FString titleId);

   void recordTelemetry(AnalyticsEvent& event);
   void tick();
   void shutDownService();
   PlayFabClient *GetPlayFabClient() const { return mPlayFabClient.get(); }
   PlayFabPlayer *GetFirstPlayFabPlayer() { return mPlayerCache.GetFirst(); }
   void AddPlayerToCache(PlayFabPlayer&&);
   void RemovePlayerFromCache(const PlayFabPlayer& PlayerId);
private:
   void _handleAggregation(EventGroup& events, const AnalyticsEvent& event);
   void _sendNextEvent(EventList& queueToSend, bool discardOnFailure);
   void _sendAllEvents(EventList& queueToSend, bool discardOnFailure);
   bool _sendEvent(AnalyticsEvent& evnt);
   void _sendAggregatedEvents();
   void _forceSendEvents(bool discardOnFailure);

   std::unique_ptr<CommonProperties> mCommonProperties;
   std::unique_ptr<PlayFabClient> mPlayFabClient;
   uint32_t mSequenceId;
   EventList mNonAggregatedEventQueue;
   std::mutex mNonAggQueueMutex;
   EventList mAggregatedEventQueue;
   std::mutex mAggQueueMutex;
   PlayFabPlayerCache mPlayerCache;
   
};
