#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.h"
#include "world/level/block/Block.h"
#include "GameplayTagContainer.h"
#include "game/util/Pushback.h"
#include "EnchantmentComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "SplitComponent.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FTagMagnitude
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TagName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Magnitude;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FEffectTagMagnitude
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> GameplayEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTagMagnitude> TagMagnitude;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API USplitComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USplitComponent();

	void OnAttributeHealthChange(const FOnAttributeChangeData& data);

	void Split(const FGameplayTagContainer&);

	void NormalSlimeSplit(const AMobCharacter* ownerCharacter, const TArray<FEnchantmentData>& enchantments);

	void Summon(EntityType type, const FVector& offset, const TArray<FEnchantmentData>& enchantments);

	void ApplyGameEffectsOnDeath();

private:
	UPROPERTY(EditAnywhere, Category = "Dungeons|SplitComponent")
	int level = 3;

	UPROPERTY(EditAnywhere, Category = "Dungeons|SplitComponent")
	float offsetDegrees = 90.f;

	UPROPERTY(EditAnywhere, Category = "Dungeons|SplitComponent")
	float distance = 80.f;

	UPROPERTY(EditAnywhere, Category = "Dungeons|SplitComponent")
	FPushback mPushback;

	UPROPERTY(EditAnywhere, Category = "Dungeons|SplitComponent")
	TMap<uint8, EntityType> EntitiesToSpawnLevels;

	UPROPERTY(EditAnywhere, Category = "Dungeons|SplitComponent | On Death")
	TArray<FEffectTagMagnitude> EffectsToApplyOnDeath;

	UPROPERTY(EditAnywhere, Category = "Dungeons|SplitComponent | On Death")
	float OnDeathRadiusToApplyEffects;

	bool HasSplit = false;
};
