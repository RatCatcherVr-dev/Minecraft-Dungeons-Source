#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "TeleportToOwnerComponent.generated.h"

namespace teleport {
	FVector AdjustToGround(const UWorld& world, const FVector& location, const AActor& actor);
	void TeleportCharacter(ABaseCharacter& characterOwner, ABaseCharacter& character, const FVector& location, const FRotator& rotation, const FGameplayTag& teleportOutCue, const FGameplayTag& teleportInCue, TOptional<FGameplayEffectSpec> TeleportEffectSpec);
	TOptional<FVector> TryFindSpawnLocation(ABaseCharacter& owner, float offset, const FVector& dir);
}

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class DUNGEONS_API UTeleportToOwnerComponent : public UActorComponent {
GENERATED_BODY()
public:
	UTeleportToOwnerComponent();

	std::function<TOptional<FGameplayEffectSpec>()> TeleportSpec;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag TeleportOutCue;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag TeleportInCue;

	void AddCharacter(ABaseCharacter& character);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UFUNCTION()
	void TeleportIfFarAway();

	UFUNCTION()
	void OnCharacterDied();

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float AutoTeleportDistanceThreshold = 2250.f;

	UPROPERTY(Replicated)
	TArray<TWeakObjectPtr<ABaseCharacter>> Characters;

	UPROPERTY()
	FTimerHandle DistanceCheckTimerHandle;
};
