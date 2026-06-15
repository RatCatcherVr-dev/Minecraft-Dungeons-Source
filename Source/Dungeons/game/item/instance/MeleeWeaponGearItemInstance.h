// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/GearItemInstance.h"
#include "game/component/MeleeAttackComponent.h"
#include "GameplayEffect.h"
#include "game/util/ValueFormat.h"
#include "game/ImpactSoundCueProvider.h"
#include "MeleeWeaponGearItemInstance.generated.h"

UCLASS()
class DUNGEONS_API AMeleeWeaponGearItemInstance : public AGearItemInstance, public IImpactSoundProvider
{
	GENERATED_BODY()
public:
	AMeleeWeaponGearItemInstance();

	UFUNCTION()
	TArray<FMeleeAttackComponentAttackVariant>& GetEditableAttackVariants();

	UFUNCTION()
	const TArray<FMeleeAttackComponentAttackVariant>& GetAttackVariants() const;
	
	float GetStats(EItemStats stat) const override;

	FORCEINLINE float GetAttackVariantResetSeconds() const { return AttackVariantResetSeconds; }
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float AttackVariantResetSeconds = 3.0;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|AttackVariants")
	TArray<FMeleeAttackComponentAttackVariant> ConfiguredAttackVariants;

	USoundCue* GetImpactSound_Implementation(int index) const override;
};

