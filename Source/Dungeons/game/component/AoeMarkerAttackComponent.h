#pragma once

#include "game/component/AttackComponent.h"
#include "game/util/Pushback.h"
#include "GameplayTagContainer.h"
#include "HealthComponent.h"
#include "AoeAttackComponent.h"
#include "game/actor/DungeonsTargetMarker.h"
#include "AoeMarkerComponent.h"
#include "AoeMarkerAttackComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UAoeMarkerAttackComponent : public UAoeMarkerComponent
{
	GENERATED_BODY()
	
public:
	UAoeMarkerAttackComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aoe Marker Attack Component")
	int markersToSpawn = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aoe Marker Attack Component")
	float markersRadiusToSpawn = 1500;

	virtual bool OnAttackBegan(AActor* Target) override;
};
