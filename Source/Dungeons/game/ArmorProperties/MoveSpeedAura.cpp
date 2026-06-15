#include "Dungeons.h"
#include "MoveSpeedAura.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

UMoveSpeedAura::UMoveSpeedAura() {
	TypeID = EArmorPropertyID::MoveSpeedAura;
}

void UMoveSpeedAura::BeginPlay() {
	Super::BeginPlay();

	if(GetOwnerRole() != ROLE_Authority) return;

	if (const auto playerOwner = Cast<APlayerCharacter>(GetOwner())) {
		OwnerAreaBuffComponent = NewObject<UAreaBuffComponent>(playerOwner, FName("Move speed aura buff component"));
		OwnerAreaBuffComponent->Attributes.Emplace(UMovementAttributeSet::SpeedMultiplierAttribute(), Multiplier);
		OwnerAreaBuffComponent->Effect = UMoveSpeedAuraGameplayEffect::StaticClass();
		OwnerAreaBuffComponent->AreaSize = Radius;
		OwnerAreaBuffComponent->RegisterComponent();
	}
}

void UMoveSpeedAura::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	if (OwnerAreaBuffComponent.IsValid()) {
		OwnerAreaBuffComponent->DestroyComponent();
	}
}

UMoveSpeedAuraGameplayEffect::UMoveSpeedAuraGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.ArmorProperty.MoveSpeedAura"), 0, 1);
	effects::AddSetByCallerAttribute(*this, UMovementAttributeSet::SpeedMultiplierAttribute(), EGameplayModOp::Multiplicitive);
}
