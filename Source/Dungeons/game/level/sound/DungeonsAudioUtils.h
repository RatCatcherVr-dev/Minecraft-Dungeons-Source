#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "game/level/sound/MusicSetTypes.h"

#include "DungeonsAudioUtils.generated.h"

class USoundCue;
enum class EMusicSequenceState : uint8;
enum class EEventType : uint8;
enum class ELevelNames : uint8;

UCLASS()
class DUNGEONS_API UDungeonsAudioUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Audio")
	static ELevelNames GetLevelName(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Audio")
	static FLevelAudio GetCurrentLevelAudioCollection(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Audio")
	static bool CinematicPlaying(const UObject* WorldContextObject);

};

namespace audioutils {
	float GetSoundCueDuration(USoundCue* Cue);

	USoundCue* GetEncounterMusicTrack(const UWorld* world, EMusicSequenceState sequenceState, EEventType eventType, ELevelNames level);
	ELevelNames CurrentLevelContext(UWorld* world);
	AActor* CameraViewTargetPosition(UWorld* world);

	namespace musicset {
		USoundCue* GetMusicTrack(EMusicSequenceState sequenceState, const FMusicSet& musicSet);
	}
}
