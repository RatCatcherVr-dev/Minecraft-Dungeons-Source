#include "Dungeons.h"
#include "LifeStealAura.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include <GameplayTagContainer.h>
#include "game/abilities/effects/GameplayEffectUtil.h"

ULifeStealAura::ULifeStealAura() {
	TypeID = EArmorPropertyID::LifeStealAura;
}

void ULifeStealAura::BeginPlay() {
	Super::BeginPlay();

	if(GetOwnerRole() != ROLE_Authority) return;

	if (const auto playerOwner = Cast<APlayerCharacter>(GetOwner())) {
		OwnerAreaBuffComponent = NewObject<UAreaBuffComponent>(playerOwner, FName("Life steal aura buff component"));
		OwnerAreaBuffComponent->Attributes.Emplace(UMeleeAttributeSet::MeleeAttackLifeStealAmountAttribute(), Amount);
		OwnerAreaBuffComponent->Attributes.Emplace(URangedAttributeSet::RangedAttackLifeStealAmountAttribute(), Amount);
		OwnerAreaBuffComponent->Attributes.Emplace(UItemAttributeSet::ItemLifeStealAmountAttribute(), Amount);
		OwnerAreaBuffComponent->Effect = ULifeStealAuraGameplayEffect::StaticClass();
		OwnerAreaBuffComponent->AreaSize = Radius;
		OwnerAreaBuffComponent->RegisterComponent();
	}
}

void ULifeStealAura::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	if (OwnerAreaBuffComponent.IsValid()) {
		OwnerAreaBuffComponent->DestroyComponent();
	}
}

ULifeStealAuraGameplayEffect::ULifeStealAuraGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.ArmorProperty.LifeStealAura"), 0, 1);
	effects::AddSetByCallerAttribute(*this, UMeleeAttributeSet::MeleeAttackLifeStealAmountAttribute(), EGameplayModOp::Additive);
	effects::AddSetByCallerAttribute(*this, URangedAttributeSet::RangedAttackLifeStealAmountAttribute(), EGameplayModOp::Additive);
	effects::AddSetByCallerAttribute(*this, UItemAttributeSet::ItemLifeStealAmountAttribute(), EGameplayModOp::Additive);
}
