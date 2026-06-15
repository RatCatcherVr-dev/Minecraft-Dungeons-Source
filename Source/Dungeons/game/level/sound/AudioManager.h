#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "AudioManager.generated.h"

enum class EntityType : unsigned int;

UCLASS()
class DUNGEONS_API AAudioManager : public AActor {
	GENERATED_BODY()
	
public:	
	AAudioManager();

	UFUNCTION(BlueprintCallable, Category = "Audio Management")
	float GetSoundCueFirstSoundWaveDuration(USoundCue* cue);

};
