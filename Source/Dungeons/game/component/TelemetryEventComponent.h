
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "util/telemetry/AnalyticsReflection.h"

#include "TelemetryEventComponent.generated.h"

namespace analytics
{
	class Analytics;
}

UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UTelemetryEventComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTelemetryEventComponent();

protected:
	virtual void BeginPlay() override;

	
public:	
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Telemetry")
	void ClientLogTelemetryEvent(int32 playerId, ETelemetryEventType id, const FString& metaName);
		
private:
	std::string mRoundId;
};
