#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/level/sound/MusicSetTypes.h"

#include "Runtime/Engine/Classes/Sound/SoundCue.h"
#include "AudioCollectionComponent.generated.h"


enum class ELevelNames : uint8;


UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UAudioCollectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FLevelAudio GetMusicTracksForLevel(ELevelNames level) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FMusicSet GetEncounterMusic(ELevelNames level) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FMusicSet GetBossMusic(ELevelNames level) const;

	FLevelAudio GetLobbyAudio() const;
	USoundWave* GetLoadingScreenSound() const { return LoadingScreenSound; }
	float LoadingScreenFadeIn() const { return mLoadingScreenFadeIn; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Dungeons")
	FMusicSet DefaultEncounterMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Dungeons")
	FMusicSet DefaultBossMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FLevelAudio LobbyBackgroundAudio;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	TMap<ELevelNames, FLevelAudio> LevelAudioTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	USoundWave* LoadingScreenSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	float mLoadingScreenFadeIn = 0.f;

};
