#pragma once 

#include "DungeonsWidgets/DungeonsUserWidget.h"

#include "ConnectionSubMenuTextRowWidget.generated.h"

UCLASS(Abstract)
class UConnectionSubMenuTextRowWidget : public UDungeonsUserWidget
{
    GENERATED_BODY()

public:
	void Update(const FText& service, const FText& status);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Service = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* Status = nullptr;
};
