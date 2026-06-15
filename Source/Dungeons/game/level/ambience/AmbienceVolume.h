#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "Ambience.h"
#include "AmbienceVolume.generated.h"

UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API AAmbienceVolume : public AVolume {
	GENERATED_BODY()
	
public:
	AAmbienceVolume();

	TOptional<EAmbienceID> GetAmbience() const;
	TOptional<EAmbienceAudioID> GetAmbienceAudio() const;
protected:
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	bool EnableSetAmbience = false;
	UPROPERTY(EditAnywhere, meta=(DisplayAfter = "EnableSetAmbience", EditCondition="EnableSetAmbience"), Category = "Dungeons")
	EAmbienceID ambience;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	bool EnableSetAudioAmbience = true;
	UPROPERTY(EditAnywhere, meta=(DisplayAfter = "EnableSetAudioAmbience", EditCondition = "EnableSetAudioAmbience"), Category = "Dungeons")
	EAmbienceAudioID ambienceAudio;

};
