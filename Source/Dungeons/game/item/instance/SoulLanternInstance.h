// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/MobSummonItem.h"
#include "SoulLanternInstance.generated.h"

UCLASS()
class DUNGEONS_API USoulLanterenTeleportGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	static const FName DurationName;

	USoulLanterenTeleportGameplayEffect();
};

UCLASS()
class DUNGEONS_API ASoulLanternInstance : public AMobSummonItem
{
	GENERATED_BODY()
	ASoulLanternInstance();

	//Time the mob will be rooted after a teleport
	UPROPERTY(EditDefaultsOnly)
	float TeleportRootTime = 1.f;

	TOptional<FGameplayEffectSpec> GenerateTeleportEffectSpec() const override;
};
