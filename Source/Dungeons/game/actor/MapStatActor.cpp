#include "Dungeons.h"
#include "MapStatActor.h"
#include "DungeonsGameMode.h"


AMapStatActor::AMapStatActor(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer) {

	bReplicates = true;
	bAlwaysRelevant = true;
	NetUpdateFrequency = 1.0f;
}

void AMapStatActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMapStatActor, TrackedCounts);
}

void AMapStatActor::TrackScalar(EMapTrackingTypes TrackingType, int Count)
{
	SetTrackingTypeCount(TrackingType, GetTrackingTypeCount(TrackingType) + Count);
}

int AMapStatActor::GetTrackingTypeCount(EMapTrackingTypes TrackingType) const
{
	for (auto&& TrackedCount : TrackedCounts) {
		if (TrackedCount.TrackingType == TrackingType) {
			return TrackedCount.Count;
		}
	}
	return 0;
}

void AMapStatActor::SetTrackingTypeCount(EMapTrackingTypes TrackingType, int Count)
{
	for (auto&& TrackedCount : TrackedCounts) {
		if (TrackedCount.TrackingType == TrackingType) {
			TrackedCount.Count = Count;
			OnTrackingTypeCountChanged(TrackingType); //Server does not get the OnRep replication callback and must call this hook itself.
			return;
		}
	}
	TrackedCounts.Add({ TrackingType, Count });
	OnTrackingTypeCountChanged(TrackingType); //Server does not get the OnRep replication callback and must call this hook itself.
}

void AMapStatActor::OnRep_TrackedCounts()
{
	//Check which tracked types were changed.
	for (auto&& TrackedCount : TrackedCounts) {
		if (!LastTrackedCount.Contains(TrackedCount.TrackingType) || LastTrackedCount[TrackedCount.TrackingType] != TrackedCount.Count) {
			//TrackingType has changed
			OnTrackingTypeCountChanged(TrackedCount.TrackingType);
			LastTrackedCount.Add(TrackedCount.TrackingType, TrackedCount.Count);
		}
	}
}

void AMapStatActor::OnTrackingTypeCountChanged(EMapTrackingTypes TrackingType) const
{
	OnMapTrackingTypeCountChanged.Broadcast(TrackingType);
}

