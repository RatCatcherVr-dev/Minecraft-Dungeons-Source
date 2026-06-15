#include "AnalyticsEvent.h"

AnalyticsEvent::AnalyticsEvent(const FString& userId, const FString& eventName)
    : mUserId(userId)
    , mName(eventName)
    , mShouldAggregate(false)
    , mAggregationTimeSeconds(60)
    , mEventCreationTime(std::chrono::system_clock::now())
    , mRecordStamped(false)
    , mProcessedRealtime(false)
{
}

void AnalyticsEvent::stampWithRecord(uint32_t recordSequence)
{
    if (mRecordStamped) {
        return;
    }

    mRecordStamped = true;

    if (getShouldAggregate()) {
        addMeasurement("SeqMn", AnalyticsMeasurement::AggregationType::Min, recordSequence);
        addMeasurement("SeqMx", AnalyticsMeasurement::AggregationType::Max, recordSequence);
        addMeasurement("RecCt", AnalyticsMeasurement::AggregationType::Increment, 1);
    }
    else {
        addProperty("Seq", recordSequence);
    }
}

bool AnalyticsEvent::operator==(const AnalyticsEvent & other) const
{
    if (mName != other.mName ||
        mProperties.Num() != other.mProperties.Num() ||
        mMeasurements.Num() != other.mMeasurements.Num())
    {
        return false;
    }

    for (auto&& propEntry : mProperties) {
        auto* itr = other.mProperties.Find(propEntry.Key);
        if (itr == nullptr) {
            return false;
        }

        auto& prop = propEntry.Value.getValue();
        if (!(prop == itr->getValue())) {
            return false;
        }

    }

    return true;
}

void AnalyticsEvent::updateMeasurements(const AnalyticsEvent& evnt)
{
    for (auto& measure : mMeasurements) {
        auto itr = evnt.mMeasurements.Find(measure.Key);

        if (itr != nullptr) {
            measure.Value.updateMeasurement(*itr);
        }
    }
}

const FString& AnalyticsEvent::getUserId() const
{
    return mUserId;
}
