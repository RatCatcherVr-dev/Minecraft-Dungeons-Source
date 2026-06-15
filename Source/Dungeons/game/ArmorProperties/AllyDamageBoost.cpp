#include "Dungeons.h"
#include "game/component/AreaBuffComponent.h"
#include "AllyDamageBoost.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

UAllyDamageBoost::UAllyDamageBoost() {
	TypeID = EArmorPropertyID::AllyDamageBoost;
}

void UAllyDamageBoost::BeginPlay() {
	Super::BeginPlay();

	if(GetOwnerRole() != ROLE_Authority) return;

	if (const auto playerOwner = Cast<APlayerCharacter>(GetOwner())) {
		OwnerAreaBuffComponent = NewObject<UAreaBuffComponent>(playerOwner, FName("Ally damage boost buff component"));
		OwnerAreaBuffComponent->Attributes.Emplace(UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute(), Multiplier);
		OwnerAreaBuffComponent->Attributes.Emplace(URangedAttributeSet::RangedAttackDamageMultiplerAttribute(), Multiplier);
		OwnerAreaBuffComponent->Effect = UAllyDamageBoostGameplayEffect::StaticClass();
		OwnerAreaBuffComponent->AreaSize = Radius;
		OwnerAreaBuffComponent->RegisterComponent();
	}
}

void UAllyDamageBoost::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	if (OwnerAreaBuffComponent.IsValid()) {
		OwnerAreaBuffComponent->DestroyComponent();
	}
}

UAllyDamageBoostGameplayEffect::UAllyDamageBoostGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.ArmorProperty.AllyDamageBoost"), 0, 1);
	effects::AddSetByCallerAttribute(*this, UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute(), EGameplayModOp::Multiplicitive);
	effects::AddSetByCallerAttribute(*this, URangedAttributeSet::RangedAttackDamageMultiplerAttribute(), EGameplayModOp::Multiplicitive);
}
