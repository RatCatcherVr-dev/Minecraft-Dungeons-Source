#include "Dungeons.h"

#include "DungeonsAbilitySystemComponent.h"
#include "util/Algo.h"

void UDungeonsAbilitySystemComponent::InitializeComponent() {
	// Check ADO ticket #319280.
	// UAbilitySystemComponent::InitializeComponent() will load all UAttributeSet subobjects serialized in an actor.
	// Serializing them is not something we want and it's causing issues now since they have been serialized into the repository.

	UGameplayTasksComponent::InitializeComponent();

	AActor *Owner = GetOwner();
	InitAbilityActorInfo(Owner, Owner);	// Default init to our outer owner
}

TArray<FHandleUIDataInfo> UDungeonsAbilitySystemComponent::GetActiveEffectsWithUiData() {
	FGameplayEffectQuery query;
	query.CustomMatchDelegate = FActiveGameplayEffectQueryCustomMatch::CreateLambda([](const FActiveGameplayEffect& effect) -> bool {
		return effect.Spec.Def != nullptr && effect.Spec.Def->UIData != nullptr;
	});

	const auto effects = GetActiveEffects(query);

	return algo::map_as<TArray<FHandleUIDataInfo>>(GetActiveEffects(query), [&](const FActiveGameplayEffectHandle& h) -> FHandleUIDataInfo { return { h, GetGameplayEffectDefForHandle(h)->UIData }; });
}
