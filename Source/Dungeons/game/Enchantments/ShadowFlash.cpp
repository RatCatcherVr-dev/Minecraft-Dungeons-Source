#include "ShadowFlash.h"
#include "game/util/ActorQuery.h"
#include "game/util/Pushback.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/item/instance/MeleeWeaponGearItemInstance.h"

const FGameplayTag &InvisibileTag() {
	static const FGameplayTag invisibleTag = FGameplayTag::RequestGameplayTag("StatusEffect.Invisible");
	return invisibleTag;
}

const FGameplayTag &ShadowFlashCue() {
	static const FGameplayTag shadowFlashCue = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.ShadowFlash");
	return shadowFlashCue;
}

UShadowFlashGameplayEffect::UShadowFlashGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UArmorItemPowerOnlyModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));
}

UShadowFlash::UShadowFlash()
{
	TypeId = EEnchantmentTypeID::ShadowFlash;
}

void UShadowFlash::OnInvisibilityTagCountChanged(const FGameplayTag tag, const int32 tagCount)
{
	// Ensure we exited invisibility
	if (tagCount > 0)
	{
		return;
	}

	const ABaseCharacter* ownerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (!ownerCharacter)
	{
		return;
	}
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal | EGameplayCueParametersField::GELevel);
	const auto level = GetLevel();

	FPushback push;
	push.pushbackStrength = PushbackStrengthPerLevel[level];
	push.pushbackZFactor = 1.0f;
	push.enablePushback = true;

	static constexpr float blockSize = 100.f;
	const auto ownerAbilitySystem = ownerCharacter->GetAbilitySystemComponent();
	for (auto mob : actorquery::getNearbyActors<AMobCharacter>(ownerCharacter, DamageRadiusPerLevel[level] * blockSize))
	{
		if (mob->EntityType == EntityType::Dummy || (mob && mob->IsAlive() && mob->IsTargetable() && mob->IsHostileTowardsPlayers()))
		{
			pushback::pushback(push, *ownerCharacter, *mob);
			
			FGameplayEffectSpec damageSpec = effects::CreateGameplayEffectSpec<UShadowFlashGameplayEffect>(ownerAbilitySystem, effects::HealthName, -Damage, GetOwner(), GetOwner(), GetOwner()->GetActorLocation(), level);
			ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(damageSpec, mob->GetAbilitySystemComponent());

			effects::StorePushbackInNormal(damageSpec, pushback::getLaunchVector(push, *ownerCharacter, *mob, 1.0f, 1.0f));
		}
	}

	BroadcastEnchantmentTriggeredEvent();

	FGameplayCueParameters cueParams;
	cueParams.GameplayEffectLevel = GetLevel();
	ownerAbilitySystem->ExecuteGameplayCue(ShadowFlashCue(), cueParams);
}

void UShadowFlash::BeginPlay()
{
	Super::BeginPlay();

	const ABaseCharacter* ownerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (!ownerCharacter)
	{
		UE_LOG(LogEnchantments, Error, TEXT("UShadowFlash : Invalid owning character"));
		return;
	}

	if (ownerCharacter->HasAuthority())
	{
		ownerCharacter->GetAbilitySystemComponent()->RegisterGameplayTagEvent(InvisibileTag()).AddUObject(this, &UShadowFlash::OnInvisibilityTagCountChanged);
	}
}

void UShadowFlash::PostInitProperties()
{
	Super::PostInitProperties();
	LevelMultiplier = [radiusPerLevel = DamageRadiusPerLevel](int level) { return radiusPerLevel[level]; };
}
