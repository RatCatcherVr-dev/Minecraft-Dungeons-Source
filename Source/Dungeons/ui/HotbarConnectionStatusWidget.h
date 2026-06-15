#pragma once

#include "CoreMinimal.h"
#include "menu/ConnectionDisplayBaseWidget.h"

#include "HotbarConnectionStatusWidget.generated.h"

UCLASS(Abstract)
class DUNGEONS_API UHotbarConnectionStatusWidget : public UConnectionDisplayBaseWidget {
	GENERATED_BODY()
protected:
	void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* TextFadeout = nullptr;

	virtual void UpdateWidgets() override;
	virtual void OnStateChanged(EMinecraftAPIConnectionStatus status) override;
};
