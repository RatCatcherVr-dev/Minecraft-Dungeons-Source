#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.h"
#include "game/Enchantments/AncientGuardianThorns.h"
#include "util/Random.h"
#include "game/actor/BiomineSpawnPoint.h"
#include "AncientGuardianComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMeleeDamaged, const FGameplayTag&);


UENUM(BlueprintType)
enum class EAncientGuardianState : uint8
{
	Disabled = 0,
	Normal = 1,
	NotSpiking = 2,
	Retreating = 3,
	Retreated = 4,
	Return = 5
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAncientGuardianStateChanged, EAncientGuardianState, newState);

USTRUCT(BlueprintType)
struct FFormationPositions
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ancient Guardian")
	TArray<ABiomineSpawnPoint*> spawnPoints;
	TArray<FVector> positions;
};

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UAncientGuardianComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAncientGuardianComponent();

	void BeginPlay() override;

	UPROPERTY(BlueprintAssignable)
	FOnAncientGuardianStateChanged OnAncientGuardianStateChanged;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ancient Guardian")
	EAncientGuardianState GetAncientGuardianState() { return ancientGuardianState; };
	UFUNCTION(BlueprintCallable, Category = "Ancient Guardian")
	void SetAncientGuardianState(EAncientGuardianState newState);
	void HandleSpikeEnchantment();

	UPROPERTY(EditAnywhere, Category = "Ancient Guardian")
	TArray<FFormationPositions> mineFormations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ancient Guardian")
	TArray<AActor*> alcoves;
	TArray<FVector> alcoveLocations;
	FVector retreatLocation;

	FVector GetNextReatreatLocation();
	void SetNextReatreatLocation();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_StateChange();

private:
	UPROPERTY(ReplicatedUsing = OnRep_StateChange)
	EAncientGuardianState ancientGuardianState = EAncientGuardianState::Disabled;

	UAncientGuardianThorns* thorns;

	Random rnd;

	UPROPERTY(EditDefaultsOnly, Category = "Ancient Guardian")
	float retreatTime = 25.0f;

	FTimerHandle returnTimerHandle;

	UFUNCTION()
	void SetReturnState();
};
