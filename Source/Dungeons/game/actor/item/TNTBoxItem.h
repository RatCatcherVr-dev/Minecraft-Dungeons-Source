#pragma once

#include "GameplayEffect.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "game/util/ActorQuery.h"
#include "game/util/Pushback.h"
#include "game/abilities/effects/WorldDamageGameplayEffect.h"
#include "game/actor/PropActor.h"
#include "ThrowablePropActor.h"
#include "TNTBoxItem.generated.h"

class ItemType;
class ABasePlayerController;
class APlayerCharacter;

UCLASS()
class DUNGEONS_API ATNTBoxItem : public AThrowablePropActor {
	GENERATED_BODY()
public:
	ATNTBoxItem(const class FObjectInitializer& OI);

	void ThrowInMouseDir() override;

	void Activate(const FVector& impulse) override;

	void ThrowServer_Implementation(const FVector& dir) override;

protected:
	void Tick(float DeltaSeconds) override;
	uint16 DealDamageWithinRadius(const int radius, AActor* owner) override;

	FGameplayEffectSpec GetGameplayEffectSpec(UAbilitySystemComponent* AbilitySystemComponent, AActor* owner, const float scaledDamage) override;

};

UCLASS()
class DUNGEONS_API UTNTBoxDamageGameplayEffect : public UWorldDamageGameplayEffect {
	GENERATED_BODY()
public:
	UTNTBoxDamageGameplayEffect();

};
