#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "game/actor/character/player/BasePlayerController.h"
#include "PlayerIdleEnchantment.generated.h"

UCLASS()
class DUNGEONS_API UPlayerIdleGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPlayerIdleGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UBardGarbIdleGameplayEffect : public UPlayerIdleGameplayEffect {
	GENERATED_BODY()
public:
	UBardGarbIdleGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UBardGarbUnique1IdleGameplayEffect : public UPlayerIdleGameplayEffect {
	GENERATED_BODY()
public:
	UBardGarbUnique1IdleGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UPlayerIdleEnchantment : public UEnchantment
{
	GENERATED_BODY()
public:
	UPlayerIdleEnchantment();

	void EndPlay(EEndPlayReason::Type endPlayReason) override;

	void RemoveIdleEffect();

	void OnPlayerIdleChange(const ABasePlayerController* player, EPlayerIdleState idle) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UPlayerIdleGameplayEffect> Effect;

	FActiveGameplayEffectHandle Handle;
};

UCLASS()
class DUNGEONS_API UBardGarbIdle : public UPlayerIdleEnchantment
{
	GENERATED_BODY()
public:
	UBardGarbIdle();
};

UCLASS()
class DUNGEONS_API UBardGarbUnique1Idle : public UPlayerIdleEnchantment
{
	GENERATED_BODY()
public:
	UBardGarbUnique1Idle();
};