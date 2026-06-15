#pragma once
#include "CoreMinimal.h"
#include "AudioManagerBase.h"
#include "AudioSFXManager.generated.h"

enum class EEventType : uint8;
class UBufferPlayerSynthComponent;


UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API AAudioSFXManager : public AAudioManagerBase {
	GENERATED_BODY()
	AAudioSFXManager(const FObjectInitializer& ObjectInitializer);
	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	UPROPERTY()
	UBufferPlayerSynthComponent *TtsBufferSynth;
};
