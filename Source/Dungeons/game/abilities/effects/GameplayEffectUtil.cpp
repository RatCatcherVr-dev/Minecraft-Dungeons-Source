#include "Dungeons.h"
#include "GameplayEffectUtil.h"
#include <GameplayEffect.h>
#include "../DungeonsAbilitySystemGlobals.h"
#include "GameplayEffectExtension.h"
#include "game/component/HealthComponent.h"

#pragma warning( push )
#pragma warning( disable : 4946)

FGameplayEffectSpec UGameplayEffectUtil::CreateEffectSpec(UAbilitySystemComponent* abilitySystem, TSubclassOf<class UGameplayEffect> effectClass, float level) {
	return effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, effectClass, level);
}

namespace effects {
	const FName HealthName("Health");
	const FName DurationName("Duration");
	const float SECONDARY_TARGET_STUN_MULTIPLIER = 0.2f;
	const float ENCHANTMENT_STUN_MULTIPLIER = 0.2f;
	const float PROJECTILE_EXPLOSION_STUN_MULTIPLIER = 0.2f;
	const float PROJECTILE_SECONDARY_HIT_STUN_MULTIPLIER = 0.2f;

	FGameplayEffectSpec CreateGameplayEffectSpecFromSubClass(const UAbilitySystemComponent* abilitySystem, TSubclassOf<UGameplayEffect> effectClass, float level) {
		return FGameplayEffectSpec(Cast<UGameplayEffect>(effectClass->GetDefaultObject()), abilitySystem->MakeEffectContext(), level);
	}

	FDungeonsGameplayEffectContext* GetDungeonsContextFromSpec(FGameplayEffectSpec& spec) {
		FGameplayEffectContextHandle handle = spec.GetContext();
		return GetDungeonsEffectContextFromHandle(handle);
	}

	const FDungeonsGameplayEffectContext* GetDungeonsContextFromSpec(const FGameplayEffectSpec& spec) {
		return GetDungeonsEffectContextFromHandle(spec.GetEffectContext());
	}

	FDungeonsGameplayEffectContext* GetDungeonsEffectContextFromHandle(FGameplayEffectContextHandle& handle) {
		return static_cast<FDungeonsGameplayEffectContext*>(handle.Get());
	}

	const FDungeonsGameplayEffectContext* GetDungeonsEffectContextFromHandle(const FGameplayEffectContextHandle& handle) {
		return static_cast<const FDungeonsGameplayEffectContext*>(handle.Get());
	}

	FGameplayModifierInfo CreateModifierInfo(FGameplayAttribute attribute, FSetByCallerFloat setByCallerMagnitude, EGameplayModOp::Type type /* = EGameplayModOp::Type::Additive */) {
		FGameplayModifierInfo modInfo;
		modInfo.Attribute = attribute;
		modInfo.ModifierMagnitude = setByCallerMagnitude;
		modInfo.ModifierOp = type;
		return modInfo;
	}

	void StorePushbackInNormal(FGameplayEffectSpec& spec, FVector normal) {
		FDungeonsGameplayEffectContext* context = GetDungeonsContextFromSpec(spec);
		context->Normal = normal / (1 << 12);
	}

	FVector GetPushbackVectorFromParams(const FGameplayCueParameters& params) {
		return params.Normal * (1 << 12);
	}

	void SetStunMultiplier(FGameplayEffectSpec& spec, float multiplier) {
		FDungeonsGameplayEffectContext* context = GetDungeonsContextFromSpec(spec);
		context->StunMultiplier = multiplier;
	}

	FGameplayTagContainer GetDungeonsDamageTypes(const FGameplayEffectModCallbackData& data) {
		FGameplayTagContainer assetTags;
		data.EffectSpec.GetAllAssetTags(assetTags);

		return assetTags.Filter(FGameplayTagContainer(damageTag::def()));
	}

	const UGameplayEffect* DefaultObject(TSubclassOf<UGameplayEffect> effect) {
		return Cast<const UGameplayEffect>(effect->GetDefaultObject());
	}

	void AddSetByCallerAttribute(UGameplayEffect& effect, const FGameplayAttribute& attribute, EGameplayModOp::Type modType) {
		FGameplayModifierInfo info;
		info.Attribute = attribute;
		FSetByCallerFloat data;
		data.DataName = FName(*info.Attribute.AttributeName);
		info.ModifierOp = modType;
		info.ModifierMagnitude = data;
		effect.Modifiers.Add(info);
	}

	FName CueFunctionNameFromEventType(EGameplayCueEvent::Type EventType) {
	switch (EventType) { case EGameplayCueEvent::OnActive:
		return FName(TEXT("OnActive"));
	case EGameplayCueEvent::WhileActive:
		return FName(TEXT("WhileActive"));
	case EGameplayCueEvent::Executed:
		return FName(TEXT("OnExecute"));
	case EGameplayCueEvent::Removed:
		return FName(TEXT("OnRemove"));
	}

	return FName();
}
}


#pragma warning (pop)
