#pragma once
#include "Dungeons.h"
#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "game/Enchantments/Enchantment.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"

#include "PushVolumeResistance.generated.h"

// ---------- GAMEPLAY EFFECTS ---------- //
UCLASS()
class DUNGEONS_API UPushVolumeImmunityGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPushVolumeImmunityGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UWindImmunityGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UWindImmunityGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UWindResistanceGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UWindResistanceGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UCurrentImmunityGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UCurrentImmunityGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UCurrentResistanceGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UCurrentResistanceGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

// ---------- ENCHANTMENTS ---------- //
UCLASS()
class DUNGEONS_API UPushVolumeImmunity : public UEnchantment
{
	GENERATED_BODY()
public:
	UPushVolumeImmunity();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

private:
	FActiveGameplayEffectHandle Handle;
};

UCLASS()
class DUNGEONS_API UWindImmunity : public UEnchantment
{
	GENERATED_BODY()
public:
	UWindImmunity();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

private:
	FActiveGameplayEffectHandle Handle;
};

UCLASS()
class DUNGEONS_API UWindResistance : public UEnchantment
{
	GENERATED_BODY()
public:
	UWindResistance();

	void BeginPlay() override;
	FText CreateDescription() const override;
	void EndPlay(const EEndPlayReason::Type) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Resistance = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float PerLevelResistance = 0.1f;

	FActiveGameplayEffectHandle Handle;
};

UCLASS()
class DUNGEONS_API UCurrentImmunity : public UEnchantment
{
	GENERATED_BODY()
public:
	UCurrentImmunity();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type) override;

private:
	FActiveGameplayEffectHandle Handle;
};

UCLASS()
class DUNGEONS_API UCurrentResistance : public UEnchantment
{
	GENERATED_BODY()
public:
	UCurrentResistance();

	void BeginPlay() override;
	FText CreateDescription() const override;
	void EndPlay(const EEndPlayReason::Type) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Resistance = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float PerLevelResistance = 0.1f;

	FActiveGameplayEffectHandle Handle;
};