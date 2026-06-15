#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DarkConduitInteraction.generated.h"

UCLASS()
class DUNGEONS_API ADarkConduitInteraction : public AActor
{
	GENERATED_BODY()
	
public:	
	ADarkConduitInteraction();
	//ADarkConduitInteraction(const class FObjectInitializer& OI);

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void TriggerOnInConduitRangeChanged(float ConduitDistSq);
	void TriggerOnInConduitRangeChanged(bool NewValue);

	UFUNCTION(BlueprintImplementableEvent)
	void OnConduitInRange();

	UFUNCTION(BlueprintImplementableEvent)
	void OnConduitOutOfRange();

	bool InteractionActive;

	UPROPERTY(BlueprintReadOnly)
	bool IsDarkConduitComplete;

	UPROPERTY(BlueprintReadWrite)
	float InteractionRange;
};

