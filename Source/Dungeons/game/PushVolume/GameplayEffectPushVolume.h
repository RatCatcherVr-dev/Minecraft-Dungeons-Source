#pragma once
#include "BasePushVolume.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffectPushVolume.generated.h"

UCLASS()
class DUNGEONS_API UWindPushVolumeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UWindPushVolumeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UCurrentPushVolumeGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UCurrentPushVolumeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API AGameplayEffectPushVolume : public ABasePushVolume {
	GENERATED_BODY()
public:
	AGameplayEffectPushVolume();

	UPROPERTY(Category = "Dungeons", EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> EffectToApply;

	UPROPERTY(Category = "Dungeons", EditDefaultsOnly, BlueprintReadWrite)
	EPushVolumeType PushVolumeType;

protected:
	void AddInfluence(UPushVolumeReactiveComponent* receiverToAdd) override;
	void OnInfluenceSuccessfullyAddedToActor(UPushVolumeReactiveComponent* reciever) override;
	void OnInfluenceSuccessfullyRemovedFromActor(UPushVolumeReactiveComponent* reciever) override;
};