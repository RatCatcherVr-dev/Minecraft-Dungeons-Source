#pragma once

#include <Kismet/BlueprintFunctionLibrary.h>

#include "Emotes.h"
#include "EmoteDef.h"
#include "EmoteDefs.generated.h"

namespace emotes {
	const EmoteDef& get(EEmote);
	const EmoteDef* getChecked(EEmote);	
}

UCLASS(BlueprintType)
class UEmoteDefs : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const FText& GetEmoteText(EEmote emote);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static TSoftObjectPtr<UTexture2D> GetEmoteIconSoftTexture(EEmote emote);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static UTexture2D* GetEmoteIconTexture(EEmote emote);
};
