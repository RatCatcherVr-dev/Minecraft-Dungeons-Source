// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "GameplayEffect.h"
#include "game/util/ValueFormat.h"
#include "game/Enchantments/Enchantment.h"
#include "MultiDodge.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChargeCounterDecreased);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChargeCounterIncreased);

UCLASS()
class DUNGEONS_API UMultiDodgeMaxDodgesGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMultiDodgeMaxDodgesGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UMultiDodgeIncreamentDodgeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMultiDodgeIncreamentDodgeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UMultiDodge : public UEnchantment
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type reason) override;

	UMultiDodge();

	UFUNCTION()
	void AfterDodgeCooldown();

	void CreateAndApplyMaxDodgeEffect();
	void RemoveDodgeEffect();
	void CreateAndApplyIncrementDodgeEffect();

	bool HasMaxDodgeCharges();

	UFUNCTION()
	void OnCooldownChanged(bool nowOnCooldown);

	UFUNCTION(BlueprintImplementableEvent)
		void OnDodgeCounterIncreased();
	UFUNCTION(BlueprintImplementableEvent)
		void OnDodgeCounterDecreased();


	FText CreateDescription() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MultiDodge)
	class USoundBase* OnDodgeCounterIncreasedSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MultiDodge)
	class USoundBase* OnDodgeCounterDecreasedSound;

	void OnStart() override;

protected:

	FActiveGameplayEffectHandle Handle;
};
