#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/HarvesterInstance.h"
#include "Harvester_Unique1.generated.h"


UCLASS()
class DUNGEONS_API AHarvester_Unique1 : public AHarvesterInstance
{
	GENERATED_BODY()
public:
	AHarvester_Unique1();
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class APoisonCloud> PoisonCloudClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UPoisonCloudDamageGameplayEffect> PoisonCloudDamageEffect;

	UPROPERTY(EditDefaultsOnly)
	float DamagePerSecond = 50.0f;

	void OnPostMobDeath(AMobCharacter* mob) override;
};
