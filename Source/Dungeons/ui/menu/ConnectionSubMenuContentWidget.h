#pragma once

#include "DungeonsWidgets/DungeonsUserWidget.h"

#include "ConnectionSubMenuContentWidget.generated.h"

class UConnectionSubMenuTextRowWidget;
enum class EMinecraftAPIConnectionStatus : uint8;

struct ConnectionSubMenuContentRowState {
	FText name;
	FText status;
};

struct ConnectionSubMenuContentState {
	TArray<ConnectionSubMenuContentRowState> connectionStatuses;
};

UCLASS(Abstract)
class UConnectionSubMenuContentWidget : public UDungeonsUserWidget {
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;

protected:
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FText GetOpenedText() const;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UConnectionSubMenuDisplayRowWidget* Summary = nullptr;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UVerticalBox* ConnectionStatuses = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UConnectionSubMenuTextRowWidget> ConnectionSubMenuTextRowClass = nullptr;

private:
	void SetState(bool EditorPreview);
	void UpdateWidgets();

	ConnectionSubMenuContentState state;

	UPROPERTY()
	UGameInstance* GameInstance = nullptr;
	FDelegateHandle OnStatusChangeHandle;
};
