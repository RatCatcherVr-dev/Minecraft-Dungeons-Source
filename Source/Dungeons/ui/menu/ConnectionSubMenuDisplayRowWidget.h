#pragma once 

#include "DungeonsWidgets/DungeonsUserWidget.h"

#include "ConnectionSubMenuDisplayRowWidget.generated.h"

enum class EMinecraftAPIConnectionStatus : uint8;

UCLASS(Abstract)
class UConnectionSubMenuDisplayRowWidget : public UDungeonsUserWidget
{
	GENERATED_BODY()

public:
	FText GetTextToSpeech() const;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UConnectionDisplayBaseWidget* Status = nullptr;
};
