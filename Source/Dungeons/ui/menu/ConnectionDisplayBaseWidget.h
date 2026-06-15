#pragma once 

#include "UserWidget.h"
#include "DungeonsWidgets/DungeonsUserWidget.h"

#include "ConnectionDisplayBaseWidget.generated.h"

enum class EMinecraftAPIConnectionStatus : uint8;

struct ConnectionDisplayBaseState {
	FText statusLabel;
	EMinecraftAPIConnectionStatus status;
};

/**
 * Base class for widgets to keep a connection status up to date, with text and icon.
 *
 * * No Children
 */
UCLASS(Abstract)
class UConnectionDisplayBaseWidget : public UDungeonsUserWidget {
    GENERATED_BODY()

public:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void BeginDestroy() override;
    FText GetTextToSpeech() const;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* ConnectionDisplayLabel = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UConnectionIconWidget* ConnectionDisplayIcon = nullptr;

	virtual void UpdateWidgets();
	virtual void OnStateChanged(EMinecraftAPIConnectionStatus status) {};
private:
	void SetState(EMinecraftAPIConnectionStatus status);

	ConnectionDisplayBaseState state;

	UPROPERTY()
	UGameInstance* GameInstance = nullptr;
	FDelegateHandle OnStatusChangeHandle;
};
