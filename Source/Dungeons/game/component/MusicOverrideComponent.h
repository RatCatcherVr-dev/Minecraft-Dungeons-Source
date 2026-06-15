#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/level/sound/MusicSetTypes.h"
#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "MusicOverrideComponent.generated.h"

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UMusicOverrideComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UMusicOverrideComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	const TMap<EMusicSequenceState, USoundCue*>& GetEventMobMusicTracks() const {
		return MusicTracks.MusicSet;
	}

	USoundCue* GetMusicOverrideTrack(EMusicSequenceState state) const;
	class USoundMix* GetSoundMix() const { return EventMobMix; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class USoundMix* EventMobMix;


private:
	UPROPERTY(EditDefaultsOnly, Category="Dungeons")
	FDungeonsMusicSet MusicTracks;

	float lastTime = 0.f;
};
