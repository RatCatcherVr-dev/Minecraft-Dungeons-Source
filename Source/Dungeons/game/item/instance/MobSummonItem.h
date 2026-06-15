#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include <GameplayEffect.h>
#include "world/entity/EntityTypes.h"
#include "MobSummonItem.generated.h"

class AMobCharacter;

UCLASS()
class DUNGEONS_API UMobSummonItemGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMobSummonItemGameplayEffect(const FObjectInitializer& ObjectInitializer);

	static const FName MobDealDamageEffectMagnitude;
	static const FName MobIncreasedMaxHealthMagnitude;
};


UCLASS()
class DUNGEONS_API AMobSummonItem : public AItemInstance
{
	GENERATED_BODY()
public:

	AMobSummonItem();

	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type reason) override;

	int GetDisplayCount() const override;
	void Activate(const FPredictionKey& predictionKey) override;

	bool IsBusy() const override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	float GetStats(EItemStats stat) const;

	static float GetStatsForEntityType(EItemStats, EntityType);
protected:
	void OnMobSummoned(AMobCharacter*, const FVector&, const FRotator&);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UMobSummonItemGameplayEffect> Effect;

	//Applied to mobs when teleporting them.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> TeleportEffect;

	std::string mobType;

	virtual EntityType GetMobType() const;
	virtual FGameplayTag GetSummonCueForType(EntityType type) const;
	virtual FGameplayTag GetTeleportCueForType(EntityType type) const;
	virtual TOptional<FGameplayEffectSpec> GenerateTeleportEffectSpec() const;

	virtual bool WillSummon() const;

	float GetSoulActivationCost() const override;
	int GetAlivePetCount() const;
private:
	UFUNCTION()
	void OnPlayerDeath();
	UFUNCTION()
	void OnPlayerTeleported();
	UFUNCTION()
	void OnPetDied();

	void AddPetMob(AMobCharacter& petMob);
	void CleanupDeadPetMobs();
	
	void SummonMob(const FPredictionKey& predictionKey, const FVector& location, const FRotator& rotation);

	TWeakObjectPtr<AMobCharacter> FindWeakestMob();

	void KillMob(TWeakObjectPtr<AMobCharacter> mobToKill);

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int PetAmountCap = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float IncreasedMaxHealthMultiplier = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DealDamageMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MobSummonOffset = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float TeleportCooldownFactor = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool CoolDownForEachPetDeath = true;

	UPROPERTY(Replicated)
	TArray<TWeakObjectPtr<AMobCharacter>> PetMobs;

	UPROPERTY()
	class UTeleportToOwnerComponent* TeleportToOwner;
};
