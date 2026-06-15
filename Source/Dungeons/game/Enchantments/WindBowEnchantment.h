#pragma once

#include "CoreMinimal.h"
#include "Enchantment.h"
#include "GameplayEffect.h"

#include "WindBowEnchantment.generated.h"

UCLASS()
class DUNGEONS_API UWindBowGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
	public:
		UWindBowGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UWindBowEnchantment : public UEnchantment {
	GENERATED_BODY()
public:
	UWindBowEnchantment();

	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

	UPROPERTY(EditDefaultsOnly)
	float KnockbackRadius = 300;

	UPROPERTY(EditDefaultsOnly, meta=(ToolTip = "Get Over Here!!"))
	float KnockbackStrength = 5;

	UPROPERTY(EditDefaultsOnly)
	float KnockbackZFactor = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float StunTime = 1.3f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UWindBowGameplayEffect> Effect;

	UFUNCTION(NetMulticast, Reliable)
	void ApplyRagdollImpulse(const TArray<ABaseCharacter*>& targets, FPushback push, FVector launchVec);
};