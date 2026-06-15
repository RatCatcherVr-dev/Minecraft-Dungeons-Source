#pragma once

#include "CoreMinimal.h"
#include "DungeonsWidgets/DungeonsUserWidget.h"
#include <functional>
#include "CosmeticsDisclaimerWidget.generated.h"

class APlayerCharacter;
class UDungeonsGameInstance;

namespace cosmetics {

struct MessageContext {
	APlayerCharacter* player;
	UDungeonsGameInstance* gameInstance;
};

using DisplayPredicate = std::function<bool(const MessageContext&)>;
using TextGen = std::function<FText()>;

struct DisplayMessage {
	DisplayPredicate Pred;
	TextGen DisplayText;
};

DisplayPredicate CheckCoopVisibility();
DisplayPredicate CheckConnectedWithCacheVisibility();
DisplayPredicate CheckConnectedNoCacheVisibility();
TextGen CoopDisclaimerText();
TextGen DisconnectedWithCacheDisclaimerText();

}


UCLASS(Abstract)
class DUNGEONS_API UCosmeticsDisclaimerWidget : public UDungeonsUserWidget {
	GENERATED_BODY()
public:
	void NativePreConstruct() override;

	UFUNCTION(BlueprintCallable)
	void OnCosmeticFilterOpened();
protected:
	/** Shows the disclaimer text for the window */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* DisclaimerText = nullptr;
private:
	TArray<cosmetics::DisplayMessage> Messages;
};
