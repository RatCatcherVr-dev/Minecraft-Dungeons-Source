// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "game/GameBP.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "DungeonsAbilitySystemGlobals.generated.h"


USTRUCT()
struct DUNGEONS_API FDungeonsGameplayEffectContext : public FGameplayEffectContext {
	GENERATED_USTRUCT_BODY()
	
	TSet<FGameplayTag> AdditionalCues;

	mutable TOptional<FVector> Normal;

	float StunMultiplier = 1.f;

	TOptional<float> Period;
	
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
	FGameplayEffectContext* Duplicate() const override;

	game::Game* Game;
	ETeamName InstigatorTeam = ETeamName::World;

	void AddInstigator(class AActor *InInstigator, class AActor *InEffectCauser) override;
};

enum class EGameplayCueParametersField : uint16 {
	EMPTY = 0,
	NormalizedMagnitude = 1,
	RawMagnitude = (1 << 1),
	EffectContext = (1 << 2),
	Location = (1 << 3) ,
	Normal = (1 << 4),
	Instigator = (1 << 5),
	EffectCauser = (1 << 6),
	SourceObject = (1 << 7),
	TargetAttachComponent = (1 << 8),
	PhysMaterial = (1 << 9),
	GELevel = (1 << 10),
	AbilityLevel = (1 << 11),
	AggregatedSourceTags = (1 << 12),
	AggregatedTargetTags = (1 << 13),
};

//underlying type made Unreal Headertool Crash...
using FGameplayCueParametersFilter = uint16;

FORCEINLINE FGameplayCueParametersFilter operator&(FGameplayCueParametersFilter a, EGameplayCueParametersField b) { return a & static_cast<FGameplayCueParametersFilter>(b); }
FORCEINLINE FGameplayCueParametersFilter operator|(FGameplayCueParametersFilter a, EGameplayCueParametersField b) { return a | static_cast<FGameplayCueParametersFilter>(b); }
FORCEINLINE FGameplayCueParametersFilter operator|(EGameplayCueParametersField a, EGameplayCueParametersField b) { return static_cast<FGameplayCueParametersFilter>(a) | static_cast<FGameplayCueParametersFilter>(b); }

/**
 * 
 */
UCLASS()
class DUNGEONS_API UDungeonsAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()


	void GlobalPreGameplayEffectSpecApply(FGameplayEffectSpec& Spec, UAbilitySystemComponent* AbilitySystemComponent) override;
	
	void InitGameplayCueParameters(FGameplayCueParameters& CueParameters, const FGameplayEffectSpecForRPC &Spec) override;
	void InitGameplayCueParameters_GESpec(FGameplayCueParameters& CueParameters, const FGameplayEffectSpec &Spec) override;
	void InitGameplayCueParameters(FGameplayCueParameters& CueParameters, const FGameplayEffectContextHandle& EffectContext) override;
	
	FGameplayEffectContext* AllocGameplayEffectContext() const override;


	UFUNCTION(BlueprintCallable)
	static float GetPeriod(const FGameplayEffectContextHandle& handle);

public:
	void PushParameterFilterContext(FGameplayCueParametersFilter);
	void PopParameterFilterContext();
	
private:
	TArray<FGameplayCueParametersFilter> ParamFilterStack;
	
	TWeakObjectPtr<AGameBP> CachedGameBP;
};



struct FParameterFilterContextWindow {
	FParameterFilterContextWindow();
	FParameterFilterContextWindow(FGameplayCueParametersFilter filter);
	FParameterFilterContextWindow(EGameplayCueParametersField filter);
	~FParameterFilterContextWindow();
	
private:
	UDungeonsAbilitySystemGlobals& Globals;
};