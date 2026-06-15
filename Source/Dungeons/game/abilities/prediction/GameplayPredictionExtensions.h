#pragma once
#include "CoreMinimal.h"
#include "GameplayPrediction.h"
#include "GameplayPredictionExtensions.generated.h"

struct FSharedPredictionContextImpl;

//A shared prediction key, that will not be repped back to client until all users have finished using it for predictions. On clients, it simply contains the prediction key
USTRUCT()
struct DUNGEONS_API FSharedPredictionContext {
	GENERATED_USTRUCT_BODY()
	//Creates a shared prediction context for the specified key.
	FSharedPredictionContext(UAbilitySystemComponent* AbilitySystemComponent, FPredictionKey InPredictionKey);
	//Empty context, same as passing empty key (useful for default arguments)
	FSharedPredictionContext() {};
	~FSharedPredictionContext();

	//Utility function that will generate a new (possibly dependent) key and return a context wrapping that key.
	static FSharedPredictionContext WithNewKey(UAbilitySystemComponent*, FPredictionKey DependentKey = FPredictionKey());

	FPredictionKey GetKey() const;

private:
	TSharedPtr<FSharedPredictionContextImpl> KeyOwner;

	friend struct FUseSpecfiedKeyScopedPredictionWindow;
};


//A prediction window that simply sets the specified key as the current scoped prediction key on the ability system component passed in.
USTRUCT()
struct DUNGEONS_API FUseSpecfiedKeyScopedPredictionWindow {
	GENERATED_USTRUCT_BODY()
	FUseSpecfiedKeyScopedPredictionWindow();
	FUseSpecfiedKeyScopedPredictionWindow(FSharedPredictionContext&);
	~FUseSpecfiedKeyScopedPredictionWindow();

	FPredictionKey	ScopedPredictionKey;
private:
	TWeakObjectPtr<UAbilitySystemComponent> Owner;
	FPredictionKey RestoreKey;
};