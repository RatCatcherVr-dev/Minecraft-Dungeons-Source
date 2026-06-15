#include "Dungeons.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/merchant/ui/MerchantWidgetBase.h"
#include "game/actor/character/merchant/MerchantActor.h"
#include "ShopperComponent.h"

UShopperComponent::UShopperComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UShopperComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	if (!actorquery::getGame(GetWorld())) { // @attn @todo @nogame @fredstefanaron @dlc4hotfix
		return;
	}
	if (mMerchantWidgetCreationQueue.Num() > 0) {
		auto merchantWeakPtr = mMerchantWidgetCreationQueue.Pop();
		if(merchantWeakPtr.IsValid()){
			EnsureMerchantWidgetFor(merchantWeakPtr.Get());
		}
	} else {
		SetComponentTickEnabled(false);
	}
}

void UShopperComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	for (auto pair : mMerchantWidgetCache) {
		pair.Value->RemoveFromParent();
	}
	mMerchantWidgetCache.Reset();
	mMerchantWidgetCreationQueue.Reset();
}

void UShopperComponent::OpenMissionSelector(ELevelNames mission) const {
	OnOpenMissionSelector.Broadcast(mission);
}

void UShopperComponent::EnqueueMerchantWidgetCreationFor(AMerchantBase* merchant) {
	if (!mMerchantWidgetCache.Contains(merchant)) {
		mMerchantWidgetCreationQueue.AddUnique(merchant);
		SetComponentTickEnabled(true);
	}
}

void UShopperComponent::EnsureMerchantWidgetFor(AMerchantBase* merchant) {
	if (!mMerchantWidgetCache.Contains(merchant)) {
		auto widget = CreateWidgetOfClass(merchant->GetMerchantActorOwner()->GetMerchantWidgetClass());
		check(widget && "failed to create widget");
		mMerchantWidgetCache.Add(merchant, widget);	
		widget->BindTo(merchant);
	}
}

UMerchantWidgetBase* UShopperComponent::GetOrCreateMerchantWidgetFor(AMerchantBase* merchant) {
	EnsureMerchantWidgetFor(merchant);
	return *mMerchantWidgetCache.Find(merchant);
}

void UShopperComponent::OpenMerchantWidgetFor(AMerchantBase* merchant) {
	auto mMerchantWidget = GetOrCreateMerchantWidgetFor(merchant);
	if (!mMerchantWidget) { return; }
	merchant->OnOpeningUi();
	OnShowWidget(mMerchantWidget);
}
