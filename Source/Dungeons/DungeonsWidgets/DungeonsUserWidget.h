#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DungeonsUserWidget.generated.h"

// D11.SSN - custom user widget class
UCLASS()
class DUNGEONS_API UDungeonsUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Dungeons")
	void OnSetOwner();

	virtual void OnOwnerWasChanged();
	virtual void OnOwnerIsChanging();
};
