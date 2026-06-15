#include "Dungeons.h"
#include "Shielding.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

UShielding::UShielding() {
	TypeId = EEnchantmentTypeID::Shielding;

	LevelMultiplier = [this](int level) -> float {
		return FMath::Pow(ShieldMultiplier, level);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void UShielding::OnStart() {
	Super::OnStart();

	if(GetOwnerRole() != ROLE_Authority) return;

	OwnerAreaBuffComponent = NewObject<UAreaBuffComponent>(GetOwner(), FName("Shielding buff component"));
	OwnerAreaBuffComponent->Attributes.Emplace(UHealthAttributeSet::TakeDamageMultiplierAttribute(), LevelMultiplier(Level));
	OwnerAreaBuffComponent->Effect = UShieldingGameplayEffect::StaticClass();
	OwnerAreaBuffComponent->AreaSize = ShieldRadius;
	OwnerAreaBuffComponent->RegisterComponent();
}

void UShielding::OnEnd() {
	Super::OnEnd();
	if (OwnerAreaBuffComponent.IsValid()) {
		OwnerAreaBuffComponent->DestroyComponent();
	}
}

UShieldingGameplayEffect::UShieldingGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Shielding"), 0, 1);
	effects::AddSetByCallerAttribute(*this, UHealthAttributeSet::TakeDamageMultiplierAttribute(), EGameplayModOp::Multiplicitive);
}
