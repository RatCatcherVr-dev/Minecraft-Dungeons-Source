#pragma once

#include "game/component/AttackComponent.h"
#include "game/util/Pushback.h"
#include "GameplayTagContainer.h"
#include "HealthComponent.h"
#include "AoeAttackComponent.h"
#include "game/actor/DungeonsTargetMarker.h"
#include "AoeMarkerComponent.h"
#include "AoeFormationAttackComponent.generated.h"

USTRUCT(BlueprintType)
struct FAttackFormationPositions
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> positions;
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UAoeFormationAttackComponent : public UAoeMarkerComponent
{
	GENERATED_BODY()
	
public:
	UAoeFormationAttackComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aoe Formation Attack Component")
	TArray<FAttackFormationPositions> AttackFormations;

	virtual bool OnAttackBegan(AActor* Target) override;
};
