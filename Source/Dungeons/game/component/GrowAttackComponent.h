#pragma once

#include "game/component/AttackComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "game/Enchantments/EnchantmentType.h"
#include "GrowAttackComponent.generated.h"

UCLASS()
class DUNGEONS_API UGrowAttackBuffGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UGrowAttackBuffGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UGrowAttackComponent : public UAttackComponent
{
	GENERATED_BODY()
	
public:
	UGrowAttackComponent();

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons|EnchantComponent")
	class UAnimSequenceBase* EnchantSequence = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons|EnchantComponent")
	FName Slot = FName(TEXT("UpperBody"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|EnchantComponent")
	float enchantDelay = .8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|EnchantComponent")
	float totalTime = 2.0f;
	
	bool CanGrow(AActor* attackTarget) const;
	
	bool CanGrow(const TArray<AActor*>& attackTargets) const;

	void Grow(const TArray<AActor*>& attackTargets);
	
	void CompleteAttack();

	void Cancel();

	UFUNCTION()
	void Disenchant();

	UFUNCTION()
	void HandleDamage(float damage);

	void BeginPlay();

protected:
	void ApplyEnchant();
	void ApplyEnchant(ABaseCharacter* targetCharacter);

	void AttackCpp(AActor* attackTarget, int32 seed = 0, FSharedPredictionContext predictionContext = FSharedPredictionContext()) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|EnchantComponent")
	TSubclassOf<UGrowAttackBuffGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|EnchantComponent")
	float BuffTakeDamageMultiplier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|EnchantComponent")
	float BuffDealDamageMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|EnchantComponent")
	float BuffSpeedMultiplier = 3.0f;
private:
	TArray<TWeakObjectPtr<AActor>> AttackTargets;

	mutable TArray<TWeakObjectPtr<ABaseCharacter>> EnchantedTargets;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|EnchantComponent")
	int MaxEnchantedTargets = 2;
	
	FTimerHandle DelayTimerHandle;

	FTimerHandle CompleteTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	float distanceMax;

	UPROPERTY(EditDefaultsOnly)
	bool deflateOnDamage = false;

	void DisenchantTarget(TWeakObjectPtr<ABaseCharacter> target);

	EEnchantmentTypeID EnchantToGrant;
};
