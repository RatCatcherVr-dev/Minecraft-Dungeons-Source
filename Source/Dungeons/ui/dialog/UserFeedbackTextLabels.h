#pragma once

#include "Internationalization.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ui/dialog/BlockingMessage.h"
#include "StringTableCoreFwd.h"
#include "UserFeedbackTextLabels.generated.h"


//Todo: create interface to hook into Global messages system (It needs GetTitle() + GetBody() ) 
// + reparent all interface BP classes!
USTRUCT(BlueprintType) 
struct FDialogData {
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FText Title;

	UPROPERTY(BlueprintReadOnly)
	FText Body;

};

UCLASS()
class DUNGEONS_API UUserFeedbackTextLabels : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FDialogData GetControllerDisconnectedMessage();


};
