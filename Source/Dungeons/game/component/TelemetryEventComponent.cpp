#include "Dungeons.h"
#include "TelemetryEventComponent.h"
#include "game/Game.h"
#include "game/util/ActorQuery.h"
#include "util/StringUtil.h"
#include "util/telemetry/AnalyticsReflection.h"
#include "util/telemetry/Analytics.h"

UTelemetryEventComponent::UTelemetryEventComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	PrimaryComponentTick.bCanEverTick = false;
	// ...
}


// Called when the game starts
void UTelemetryEventComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const auto game = actorquery::getGame(GetWorld()))
	{
		mRoundId = game->roundId();
	}
}

void UTelemetryEventComponent::ClientLogTelemetryEvent_Implementation(int32 playerId, ETelemetryEventType id, const FString& metaName)
{
	if (const auto game = actorquery::getGame(GetWorld()))
	{
		mRoundId = game->roundId();
	}

	UE_LOG(LogTelemetry, Log, TEXT("got telemetry event '%s', playerId: %d"), *metaName, playerId);
	analytics::Analytics::GetInstance().FireEmptyEvent(metaName);
}

