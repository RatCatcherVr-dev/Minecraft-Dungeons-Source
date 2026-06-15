#include "Dungeons.h"
#include "ContinousDamageActor.h"
#include "character/BaseCharacter.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "util/CharacterQuery.h"
#include <GameplayEffectTypes.h>
#include "game/component/ContinousDamageComponent.h"
#include <GameplayEffect.h>


AContinousDamageActor::AContinousDamageActor() {
	bReplicates = true;
	DamageComponent = CreateDefaultSubobject<UContinousDamageComponent>("ContinuousDamage");
}

void AContinousDamageActor::BeginPlay() {
	Super::BeginPlay();
	SetLifeSpan(TotalLifeSpan);
	DamageComponent->SetInstigator(GetInstigator());
	GetWorld()->GetTimerManager().SetTimer(DamageExpireHandle, this, &AContinousDamageActor::OnDisableDamage, DamageDuration);
}

void AContinousDamageActor::OnDisableDamage() {
	if (HasAuthority()) {
		DamageComponent->DisableDamage();
	}
	OnDamageDisabled();
}

void AContinousDamageActor::SetGameplayEffectSpec(const FGameplayEffectSpecHandle& spec) {
	DamageComponent->SetGameplayEffectSpec(spec);
}