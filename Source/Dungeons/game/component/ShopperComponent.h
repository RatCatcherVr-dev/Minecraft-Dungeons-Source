#pragma once

#include "Components/ActorComponent.h"
#include "game/merchant/ui/MerchantWidgetBase.h"
#include "game/mission/MissionEditor.h"
#include "game/levels.h"
#include <Engine/EngineTypes.h>
#include "ShopperComponent.generated.h"

class UUserWidget;
class AMerchantBase;
class ABasePlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpenMissionSelector, ELevelNames, mission);
DECLARE_MULTICAST_DELEGATE(FOnItemReserved);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class DUNGEONS_API UShopperComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UShopperComponent();

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FOnItemReserved OnItemReserved;

	UPROPERTY(BlueprintAssignable)
	FOnOpenMissionSelector OnOpenMissionSelector;

	void OpenMissionSelector(ELevelNames) const;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnShowWidget(UMerchantWidgetBase* widget);

	UFUNCTION(BlueprintImplementableEvent)
	UMerchantWidgetBase* CreateWidgetOfClass(const TSoftClassPtr<UMerchantWidgetBase> &  MerchantWidgetClass);

private:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AMerchantBase>> mMerchantWidgetCreationQueue;

	UPROPERTY(Transient)
	TMap<AMerchantBase*,UMerchantWidgetBase*> mMerchantWidgetCache;

	UMerchantWidgetBase* GetOrCreateMerchantWidgetFor(AMerchantBase* merchant);

public:
	void EnqueueMerchantWidgetCreationFor(AMerchantBase* merchant);
	void EnsureMerchantWidgetFor(AMerchantBase* merchant);
	void OpenMerchantWidgetFor(AMerchantBase* merchant);
};
