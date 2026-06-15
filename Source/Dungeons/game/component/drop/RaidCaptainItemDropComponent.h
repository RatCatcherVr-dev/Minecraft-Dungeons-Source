// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "game/component/drop/ItemDropComponent.h"
#include "RaidCaptainItemDropComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class DUNGEONS_API URaidCaptainItemDropComponent : public UItemDropComponent
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly)
	EDropSpawnType SpawnType = EDropSpawnType::Shared;

	UPROPERTY(EditDefaultsOnly)
	float Probability = 1.f;

	UPROPERTY(EditDefaultsOnly)
	int32 MinCount = 1;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxCount = 1;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxDifficultyTokensSpawned = 3;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxMysteryTokensSpawned = 3;

	static URaidCaptainItemDropComponent* Create(UObject* outer);
private:
	TArray<FNetworkedItemDropData> GatherItemDropData(const FItemDropSource& dropSource) override;
protected:
	void OnMobKilled(const FOnAttributeChangeData& data) override;

};
