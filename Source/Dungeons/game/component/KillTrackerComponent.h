#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DungeonsGameInstance.h"
#include "KillTrackerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UKillTrackerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UKillTrackerComponent();

	UFUNCTION()	
	void HandleMobKill(AActor* item, AActor* player, AActor* mob);

	int32 GetNumKillsInCategory(const FString& mobEntityTypeName);

	// D11.SSN
	UFUNCTION(Client, Reliable)
	void Client_RegisterKill(EntityType mobEntityType, bool enchanted);

private:
	void RegisterKill(EntityType mobEntityType, bool enchanted);
};
