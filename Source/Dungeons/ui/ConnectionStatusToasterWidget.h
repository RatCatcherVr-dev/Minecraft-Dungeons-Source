#pragma once

#include "CoreMinimal.h"
#include "DungeonsWidgets/DungeonsUserWidget.h"
#include "ConnectionStatusToasterWidget.generated.h"


enum class EMinecraftAPIConnectionStatus : uint8;

UCLASS(Abstract)
class DUNGEONS_API UConnectionStatusToasterWidget : public UDungeonsUserWidget {
	GENERATED_BODY()
public:
	void NativePreConstruct() override;
	void NativeConstruct() override;
	void NativeDestruct() override;
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UConnectionIconWidget* ConnectionStatusIcon = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ConnectionStatusNotification = nullptr;

	UPROPERTY(BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* PopIn = nullptr;
private:
	void OnConnectionStatusChanged(EMinecraftAPIConnectionStatus status);

	FText GetStatusText(EMinecraftAPIConnectionStatus status);

	bool bConnectionWasLost = false;

	FDelegateHandle StatusChangeHandle;

	UPROPERTY()
	UGameInstance* GameInstance = nullptr;
};
