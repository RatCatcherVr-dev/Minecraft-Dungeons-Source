#include "AnalyticsService.h"
#include "AnalyticsEvent.h"
#include "CommonProperties.h"

#include <iomanip>
#include <sstream>

constexpr auto MAX_EVENT_QUEUE = 500;

AnalyticsService::AnalyticsService(FString cachFile, FString titleId) :
	mCommonProperties(std::make_unique<CommonProperties>(cachFile)),
	mPlayFabClient(std::make_unique<PlayFabClient>(titleId)),
	mSequenceId(0)
{
	mCommonProperties->InitializeCommonProperties();

}

void AnalyticsService::recordTelemetry(AnalyticsEvent& event) {
	event.stampWithRecord(mSequenceId++);

	if (event.getShouldAggregate())
	{
		std::lock_guard<std::mutex> lock(mAggQueueMutex);
		if (auto itr = mAggregatedEventQueue.Find(event.getName())) {
			_handleAggregation(*itr->get(), event);
		}
		else {
			if (mAggregatedEventQueue.Num() < MAX_EVENT_QUEUE) {
				mAggregatedEventQueue.Add(event.getName(), std::make_unique<EventGroup>(event));
			}
		}
	}
	else
	{
		std::lock_guard<std::mutex> lock(mNonAggQueueMutex);
		
		if (auto itr = mNonAggregatedEventQueue.Find(event.getName())) {
			std::lock_guard<std::mutex> innerLock(itr->get()->mutex);
			itr->get()->events.push_back(event);
		}
		else {
			if (mNonAggregatedEventQueue.Num() < MAX_EVENT_QUEUE) {
				mNonAggregatedEventQueue.Add(event.getName(), std::make_unique<EventGroup>(event));
			}
		}
	}
}

void AnalyticsService::tick() {
	{
		std::lock_guard<std::mutex> lock(mNonAggQueueMutex);
		_sendNextEvent(mNonAggregatedEventQueue, false);
	}
	_sendAggregatedEvents();
	mPlayFabClient->Tick();
}

void AnalyticsService::shutDownService()
{
	// If the game is shutting down, don't keep retrying event sends. 
	_forceSendEvents(true);
	mPlayFabClient->OnShutdown();
}

void AnalyticsService::AddPlayerToCache(PlayFabPlayer&& PlayerId)
{
	mPlayerCache.AddPlayer(std::move(PlayerId));
}

void AnalyticsService::RemovePlayerFromCache(const PlayFabPlayer& Player) {
	mPlayerCache.RemovePlayer(Player.GetPlayerId());
}

void AnalyticsService::_forceSendEvents(bool discardOnFailure)
{
	{
		std::lock_guard<std::mutex> lock(mNonAggQueueMutex);
		_sendAllEvents(mNonAggregatedEventQueue, discardOnFailure);
	}
	{
		std::lock_guard<std::mutex> lock(mAggQueueMutex);
		_sendAllEvents(mAggregatedEventQueue, discardOnFailure);
	}
}

void AnalyticsService::_sendNextEvent(EventList& queueToSend, bool discardOnFailure) {
	for (auto& eventlist : queueToSend) {
		auto& eventGroup = eventlist.Value;
		if (!eventGroup->events.empty()) {
			std::lock_guard<std::mutex> lock(eventGroup->mutex);
			if (_sendEvent(eventGroup->events.front()) || discardOnFailure)
				eventGroup->events.pop_front();
			return;
		}
	}
	queueToSend.Empty();
}

void AnalyticsService::_sendAllEvents(EventList& queueToSend, bool discardOnFailure) {
	while (queueToSend.Num() != 0) {
		_sendNextEvent(queueToSend, discardOnFailure);
	}
}

bool AnalyticsService::_sendEvent(AnalyticsEvent& evnt)
{
	for (auto commonProp : mCommonProperties->GetProperties())
	{
		evnt.addProperty(commonProp.Key, commonProp.Value.getValue());
	}

	if (!mPlayerCache.HasPlayer(evnt.getUserId()))
	{
		return false;
	}
	auto player = mPlayerCache.GetPlayFabPlayer(evnt.getUserId());
	auto result = mPlayFabClient->SendAnalyticsEvent(player, evnt);

	return result != SendAnalyticEventResult::AuthenticationInProcess;
}

void AnalyticsService::_sendAggregatedEvents()
{
	auto now = std::chrono::system_clock::now();
	std::lock_guard<std::mutex> lock(mAggQueueMutex);
	for (auto qIter = mAggregatedEventQueue.CreateIterator(); qIter; ++qIter) {
		auto& eventGroup = qIter->Value;
		if (!eventGroup->events.empty() && now > eventGroup->events.front().getAggregationCompleteTime()) {
			{
				std::lock_guard<std::mutex> innerLock(eventGroup->mutex);
				for (auto& event : eventGroup->events) {
					_sendEvent(event);
				}
			}
			qIter.RemoveCurrent();
		}
	}
}

void AnalyticsService::_handleAggregation(EventGroup& events, const AnalyticsEvent& event) {
	for (auto& eventInQueue : events.events) {
		if (event == eventInQueue) {
			eventInQueue.updateMeasurements(event);
			return;
		}
	}
	{
		std::lock_guard<std::mutex> lock(events.mutex);
		events.events.push_back(event);
	}
}
