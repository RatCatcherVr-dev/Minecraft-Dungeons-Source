#pragma once

#include "ImpactSoundCueProvider.generated.h"

UINTERFACE(BlueprintType)
class DUNGEONS_API UImpactSoundProvider : public UInterface 
{ 
	GENERATED_BODY()
};

class DUNGEONS_API IImpactSoundProvider {
	GENERATED_BODY()
public:
	
	/** Reacts to impact of a weapon, passes the variant as level. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Dungeons | Impact")
	class USoundCue* GetImpactSound(int variant) const;
};