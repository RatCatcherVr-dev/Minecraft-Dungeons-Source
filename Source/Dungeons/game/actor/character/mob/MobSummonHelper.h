// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "game/mobspawn/MobSpawnTypes.h"
#include <GameplayTagContainer.h>
#include "MobSummonHelper.generated.h"

class UGameplayEffect;
DECLARE_DELEGATE_OneParam(FOnMobSummonedDelegate, const TArray<AMobCharacter*>&);

/**
 * 
 */
UCLASS(DefaultToInstanced)
class DUNGEONS_API UMobSummonHelper : public UObject
{
	GENERATED_BODY()
public:
	UMobSummonHelper();

	void TrySummonMobs(game::mobspawn::TransformProvider, const game::mobspawn::Config&, float EffectLevel, int mobsToSpawn, FOnMobSummonedDelegate delegate = {}) const;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	EntityType MobToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FGameplayTag SpawnCue;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool ManuallyTriggerSpawnCue;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UGameplayEffect> Effect;

	//Spawning of mobs is deferred, so we might end up spawning a mob after this object has expired. Set this to false to destroy the mob if source has expired.
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool bSummonEvenIfExpired = true;
	
	int CurrentNumberOfMobs() const;
	TArray<AMobCharacter*> GetSummonedMobs() const;

	void GrantSummons(const TArray<AMobCharacter*> summons);
protected:

	void OnMobSummoned(AMobCharacter* mob);
	void OnMobDeath(const AMobCharacter* mob);

	class IAbilitySystemInterface* AbilitySystemProvider = nullptr;

	class UAbilitySystemComponent* GetAbilitySystem() const;
	void PostRename(UObject* OldOuter, const FName OldName) override;
	void PostInitProperties() override;
private:
	mutable TArray<TWeakObjectPtr<AMobCharacter>> SummonedMobs;


	AActor* OuterActor = nullptr;
};
