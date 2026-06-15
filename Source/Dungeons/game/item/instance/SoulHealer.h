#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "game/component/SoulComponent.h"
#include "game/Enchantments/Altruistic.h"
#include "SoulHealer.generated.h"


UCLASS()
class DUNGEONS_API USoulHealGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USoulHealGameplayEffect();
};

/**
 * 
 */
UCLASS()
class DUNGEONS_API ASoulHealer : public AItemInstance
{
	GENERATED_BODY()
public:

	ASoulHealer();

	float GetStats(EItemStats stat) const override;

	void Activate(const FPredictionKey& predictionKey) override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnSoul(ABaseCharacter* target, const FVector& spawnLocation, float healAmount, float magnitude);
protected:
	void OnOrbHealedActor(AActor* actor, float magnitude);

	UPROPERTY(EditDefaultsOnly)
	float HealAmountPerSoul = 4.0f;

	UPROPERTY(EditDefaultsOnly)
	float Radius = 1500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AHealOnOverlapActor> ActorClass;

private:
	static class ABaseCharacter* GetCharacterLowestHealth(const TArray<ABaseCharacter*>& players);
};
