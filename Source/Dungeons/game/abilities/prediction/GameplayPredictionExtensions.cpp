#include "GameplayPredictionExtensions.h"
#include "UnrealTemplate.h"
#include "AbilitySystemComponent.h"

struct FSharedPredictionContextImpl {
	TWeakObjectPtr<UAbilitySystemComponent> Owner;
	FPredictionKey Key;

	FSharedPredictionContextImpl(UAbilitySystemComponent * AbilitySystemComponent, FPredictionKey InPredictionKey)
		: Key(InPredictionKey), Owner(AbilitySystemComponent) {}
	~FSharedPredictionContextImpl() {
		if (auto* owner = Owner.Get()) {
			//Since the replication map symbols are not exported, we have to do this...
			FScopedPredictionWindow(owner, Key);
		}
	}
};

FSharedPredictionContext::FSharedPredictionContext(UAbilitySystemComponent * AbilitySystemComponent, FPredictionKey InPredictionKey)
	:KeyOwner(MakeShared<FSharedPredictionContextImpl>(AbilitySystemComponent, InPredictionKey)) {
}

FSharedPredictionContext FSharedPredictionContext::WithNewKey(UAbilitySystemComponent * AbilitySystemComponent, FPredictionKey DependentKey)
{
	// We should never generate prediction keys on the authority
	if (AbilitySystemComponent->GetOwnerRole() != ROLE_Authority)
	{
		DependentKey.GenerateDependentPredictionKey();
	}

	return FSharedPredictionContext(AbilitySystemComponent, DependentKey);
}

FPredictionKey FSharedPredictionContext::GetKey() const
{
	return KeyOwner ? KeyOwner->Key : FPredictionKey();
}

FSharedPredictionContext::~FSharedPredictionContext() {}

FUseSpecfiedKeyScopedPredictionWindow::FUseSpecfiedKeyScopedPredictionWindow(FSharedPredictionContext& context)
{
	if (auto* keyOwner = context.KeyOwner.Get()) {
		ScopedPredictionKey = RestoreKey = keyOwner->Key;
		Owner = keyOwner->Owner;
		Swap(RestoreKey, Owner->ScopedPredictionKey);
	} 
}

FUseSpecfiedKeyScopedPredictionWindow::FUseSpecfiedKeyScopedPredictionWindow() {
}

FUseSpecfiedKeyScopedPredictionWindow::~FUseSpecfiedKeyScopedPredictionWindow()
{
	if (auto* owner = Owner.Get()) {
		Swap(RestoreKey, owner->ScopedPredictionKey);
	}
}