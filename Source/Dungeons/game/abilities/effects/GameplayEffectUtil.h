#pragma once
#include <AbilitySystemComponent.h>
#include <AbilitySystemInterface.h>
#include "GameplayEffectUtil.generated.h"

UCLASS()
class DUNGEONS_API UGameplayEffectUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "GameplayEffect")
	static FGameplayEffectSpec CreateEffectSpec(UAbilitySystemComponent* abilitySystem, TSubclassOf<class UGameplayEffect> effectClass, float level);
};

struct FDungeonsGameplayEffectContext;

namespace effects {
	extern const FName HealthName;	
	extern const FName DurationName;
	extern const float SECONDARY_TARGET_STUN_MULTIPLIER;
	extern const float ENCHANTMENT_STUN_MULTIPLIER;
	extern const float PROJECTILE_EXPLOSION_STUN_MULTIPLIER;
	extern const float PROJECTILE_SECONDARY_HIT_STUN_MULTIPLIER;

	void AddSetByCallerAttribute(UGameplayEffect& effect, const FGameplayAttribute& attribute, EGameplayModOp::Type modType);

	FGameplayEffectSpec CreateGameplayEffectSpecFromSubClass(const UAbilitySystemComponent* abilitySystem, TSubclassOf<UGameplayEffect> effectClass, float level = FGameplayEffectConstants::INVALID_LEVEL);

	//D11.PS
	FDungeonsGameplayEffectContext* GetDungeonsContextFromSpec(FGameplayEffectSpec& spec);

	template<typename T> FGameplayEffectSpec CreateGameplayEffectSpec(const UAbilitySystemComponent* abilitySystem, float level = FGameplayEffectConstants::INVALID_LEVEL) {
		static_assert(std::is_base_of<UGameplayEffect, T>(), "Template parameter must be a GameplayEffect.");
		return FGameplayEffectSpec(Cast<UGameplayEffect>(T::StaticClass()->GetDefaultObject()), abilitySystem->MakeEffectContext(), level);
	}

	template<typename T> FGameplayEffectSpec CreateGameplayEffectSpec(IAbilitySystemInterface* abilitySystemInterface, float level = FGameplayEffectConstants::INVALID_LEVEL) {
		return CreateGameplayEffectSpec<T>(abilitySystemInterface->GetAbilitySystemComponent(), level);
	}

	template<typename T> FGameplayEffectSpec CreateGameplayEffectSpec(UAbilitySystemComponent* abilitySystem, FName attribute, float magnitude, AActor* instigator, AActor* effectCauser, FVector origin, float level = FGameplayEffectConstants::INVALID_LEVEL) {
		FGameplayEffectSpec spec = CreateGameplayEffectSpec<T>(abilitySystem, level);		
		FDungeonsGameplayEffectContext* context = GetDungeonsContextFromSpec(spec);
		spec.SetSetByCallerMagnitude(attribute, magnitude);
		spec.GetContext().AddInstigator(instigator, effectCauser);
		spec.GetContext().AddOrigin(origin);
		return spec;
	}

	template<typename T> const UGameplayEffect* DefaultObject() {
		static_assert(std::is_base_of<UGameplayEffect, T>(), "Template parameter must be a GameplayEffect.");
		return Cast<const UGameplayEffect>(T::StaticClass()->GetDefaultObject());
	}

	const UGameplayEffect* DefaultObject(TSubclassOf<UGameplayEffect> effect);

	//This will scale the normal to accomodate the [-1..1] domain of the cue parameter normal
	void StorePushbackInNormal(FGameplayEffectSpec& spec, FVector normal);
	//Reverts scaling of StorePushbackInNormal. Pushback vectors are normally quite large so we do not need high decimal precision.
	FVector GetPushbackVectorFromParams(const FGameplayCueParameters&);
	
	void SetStunMultiplier(FGameplayEffectSpec& spec, float multiplier);

	FGameplayModifierInfo CreateModifierInfo(FGameplayAttribute attribute, FSetByCallerFloat setByCallerMagnitude, EGameplayModOp::Type type = EGameplayModOp::Type::Additive);
	
	const FDungeonsGameplayEffectContext* GetDungeonsContextFromSpec(const FGameplayEffectSpec& spec);

	FDungeonsGameplayEffectContext* GetDungeonsContextFromSpec(FGameplayEffectSpec& spec);

	FDungeonsGameplayEffectContext* GetDungeonsEffectContextFromHandle(FGameplayEffectContextHandle& handle);
	const FDungeonsGameplayEffectContext* GetDungeonsEffectContextFromHandle(const FGameplayEffectContextHandle& handle);

	FGameplayTagContainer GetDungeonsDamageTypes(const FGameplayEffectModCallbackData& data);

	FName CueFunctionNameFromEventType(EGameplayCueEvent::Type EventType);

}