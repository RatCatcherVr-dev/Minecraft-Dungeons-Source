#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/OnLandingExecutionEnchantment.h"
#include <GameplayEffectTypes.h>
#include "world/level/BlockPos.h"
#include <GameplayEffect.h>
#include "VesselTrail.generated.h"

UCLASS()
class DUNGEONS_API UVesselTrailDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UVesselTrailDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API UVesselTrailSpeedGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UVesselTrailSpeedGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS(BlueprintType)
class DUNGEONS_API AVesselBlock : public AActor {
	GENERATED_BODY()
public:
	AVesselBlock();

	void SetDamagePerSecond(float dps);
	void SetDamageDuration(float duration);

	void BeginPlay() override;
	
	void Kill(float multiplier = 1.f);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnKill(float delay);

private:

	float DamagePerSecond = 0.0f;
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FActiveGameplayEffectHandle ApplyDamageEffect(ABaseCharacter* character) const;
	bool RemoveDamageEffect(ABaseCharacter* character, FActiveGameplayEffectHandle handle) const;
	void RemoveAllDamagedCharacters();

	bool CanDamage(ABaseCharacter* character) const;

	void OnDisableFireDamage();

	void DisableOverlaps() const;

	/*TArray<AActor*> GetFilteredTargets();
	TArray<AActor*> GetBoxOverlappingActors() const;

	FGameplayEffectSpec CreateFireTrailDamageEffectSpec(ABaseCharacter* characterOwner, const UAbilitySystemComponent* ownerAbilitySystem) const;
*/
	FGameplayEffectSpec EffectSpec;

	TMap<ABaseCharacter*, FActiveGameplayEffectHandle> CharactersInFire;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float killDelay = .5f;
};

UCLASS()
class DUNGEONS_API UVesselTrail : public UEnchantment {
	GENERATED_BODY()
	
public:
	UVesselTrail();

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
	void RemoveAll();


protected:
	UPROPERTY(EditAnywhere, Category = "Dungeons")	
	TSubclassOf<AVesselBlock> VesselBlockClass;

	void OnStart() override;
	void OnEnd() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BaseDamagePerSecond = 20.0f;
	
private:
	void OnExitedBlock(BlockPos blockPos);
	
	void StartFireSpawning();

	void StopFireSpawning();

	AVesselBlock* SpawnFireBlock(BlockPos blockPos) const;

	void ApplySpeedEffect();

	void RemoveSpeedEffect() const;

	FActiveGameplayEffectHandle SpeedEffectHandle;
	
	FTimerHandle TrailTimerHandle;

	bool bShouldSpawnFire = false;

	BlockPos lastBlockPos;

	TArray<AVesselBlock*> blocks;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int queueLengthMax = 5;
};
