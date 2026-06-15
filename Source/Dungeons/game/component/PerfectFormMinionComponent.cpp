#include "PerfectFormMinionComponent.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/PerfectFormComponent.h"

#include "Engine/Engine.h"

UPerfectFormMinionComponent::UPerfectFormMinionComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	bCanFeast = false;
	PerfectFormComponent = nullptr;
	OnEndermiteFeastDelegateHangle = FDelegateHandle{};
}

void UPerfectFormMinionComponent::BindPerfectFormDelegate(AMobCharacter* mob) {
	if (!PerfectFormComponent.IsValid()) {
		PerfectFormComponent = mob->FindComponentByClass<UPerfectFormComponent>();
	}
	if (PerfectFormComponent.IsValid()) {
		OnEndermiteFeastDelegateHangle = PerfectFormComponent->OnEndermiteFeastDelegate.AddUObject(this, &UPerfectFormMinionComponent::OnEndermiteFeast);
	}
}

void UPerfectFormMinionComponent::UnbindPerfectFormDelegate() {
	if (PerfectFormComponent.IsValid()) {
		if (OnEndermiteFeastDelegateHangle.IsValid()) {
			PerfectFormComponent->OnEndermiteFeastDelegate.Remove(OnEndermiteFeastDelegateHangle);
			OnEndermiteFeastDelegateHangle.Reset();
		}
	}
}

void UPerfectFormMinionComponent::OnEndermiteFeast(bool isActive)
{
	bCanFeast = isActive;
}

