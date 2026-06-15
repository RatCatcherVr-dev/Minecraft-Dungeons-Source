#include "Dungeons.h"
#include "AudioSFXManager.h"
#include "DungeonsGameInstance.h"
#include "TextToSpeech/Public/ITextToSpeech.h"

AAudioSFXManager::AAudioSFXManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bAllowTickBeforeBeginPlay = false;

	PrimaryActorTick.TickInterval = 0.1f;

	mDisplayName = "SFX.Manager:";
	FadeInFromSilent = 2.0f;
}

void AAudioSFXManager::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

#if !UE_BUILD_SHIPPING
	if (printDebug() && debugFrequency >= .5) {
		if (auto gi = GetGameInstance<UDungeonsGameInstance>())
			gi->GetSoundMixManager()->PrintDebugInfo();
	}
#endif
}

void AAudioSFXManager::BeginPlay()
{
	AAudioManagerBase::BeginPlay();
	//D11.PS - TTS Audio manager setup 	
	ITextToSpeech::Get()->Initialise();
}

void AAudioSFXManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AAudioManagerBase::EndPlay(EndPlayReason);
	ITextToSpeech::Get()->Deinitialise();
}
