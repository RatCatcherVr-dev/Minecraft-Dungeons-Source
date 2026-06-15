

/**
 * Dungeons LevelStreamingDynamic
 *
 * Dynamically controlled streaming implementation. game specific
 *
 */

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/LevelStreaming.h"
#include "Engine/LevelStreamingDynamic.h"
#include "DungeonsLevelStreamingDynamic.generated.h"



UCLASS(BlueprintType)
class DUNGEONS_API UDungeonsLevelStreamingDynamic : public ULevelStreamingDynamic
{
	GENERATED_UCLASS_BODY()
	
	
	virtual void PostLoad() override;

	void ConvertInstancedMeshes(bool ConvertTo, TArray<AActor*>& ActorsToDestroy);

private:
	UFUNCTION()
	void OnLevelLoadedCall();


};
