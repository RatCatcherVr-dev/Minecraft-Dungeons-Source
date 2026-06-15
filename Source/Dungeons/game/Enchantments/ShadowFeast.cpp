#include "ShadowFeast.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include "game/ArmorProperties/SoulGatheringBoost.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

UShadowFeast::UShadowFeast()
{
	TypeId = EEnchantmentTypeID::ShadowFeast;

	LevelMultiplier = [this](int level) -> int
	{
		return soulSpawnsPerLevel[level];
	};
}

void UShadowFeast::BeginPlay()
{
	Super::BeginPlay();

	const ABaseCharacter* ownerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (!ownerCharacter)
	{
		UE_LOG(LogEnchantments, Error, TEXT("UShadowFeast : Invalid owning character"));
		return;
	}

	if (!ownerCharacter->HasAuthority())
		return;

	auto soulComponent = ownerCharacter->FindComponentByClass<USoulComponent>();

	if (!soulComponent)
	{
		UE_LOG(LogEnchantments, Error, TEXT("UShadowFeast : Invalid soul component."));
		return;
	}

	soulComponent->OnAnyMobKilled.AddUniqueDynamic(this, &UShadowFeast::OnAnyMobKilled);
}

void UShadowFeast::OnAnyMobKilled(AActor* killedBy)
{
	if (GetOwner() && GetOwner()->HasAuthority() && killedBy == GetOwner()) {
		const auto invisibleTag = GetInvisibleTag();
		const auto ownerAbilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::GELevel);
		if (ownerAbilitySystem && ownerAbilitySystem->HasMatchingGameplayTag(invisibleTag)) {
			if (const auto soulComponent = GetOwner()->FindComponentByClass<USoulComponent>()) {
				soulComponent->ServerAddSouls(LevelMultiplier(Level));
				FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UShadowFeastGameplayEffect>(ownerAbilitySystem, Level);
				ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(spec, ownerAbilitySystem);
				BroadcastEnchantmentTriggeredEvent(true);
			}
		}
	}
}

FGameplayTag UShadowFeast::GetInvisibleTag()
{
	static const auto tag = FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Invisible"));
	return tag;
}

UShadowFeastGameplayEffect::UShadowFeastGameplayEffect()
{
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.ShadowFeast"), 0, 1);
}
