 #include "Dungeons.h"
#include "SoundMixManager.h"

#include "DungeonsGameInstance.h"
#include "DungeonsGameState.h"
#include "AudioCollectionComponent.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/Classes/Sound/SoundMix.h"
#include "game/util/ActorQuery.h"
#include "game/GameBP.h"

void USoundMixManager::PushSoundMix(USoundMix* soundMix) {
	if (ActiveSoundMixes.Contains(soundMix))
		return;

	ActiveSoundMixes.Add(soundMix);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), soundMix);
}

void USoundMixManager::PopSoundMix(USoundMix* soundMix) {
	if (ActiveSoundMixes.Contains(soundMix))
	{
		ActiveSoundMixes.Remove(soundMix);

	}
	if (soundMix) {
		UE_LOG(LogDungeons, Log, TEXT("Popping sound mix %s that was pushed outside of the manager"), *soundMix->GetFName().ToString())
	}
	UGameplayStatics::PopSoundMixModifier(GetWorld(), soundMix);
}

static FName asId(EDungeonsMusicPriority priority) {
	return FName(*GetEnumValueToString(priority));
}

void USoundMixManager::PushReverb(UReverbEffect* reverb, EDungeonsMusicPriority priority, float FadeIn) {
	if (!reverb)
		return;
	ActiveReverbEffects.Add(priority, reverb);
	UGameplayStatics::ActivateReverbEffect(GetWorld(), reverb, asId(priority), FadeIn);

}

void USoundMixManager::PopReverb(EDungeonsMusicPriority priority) {
	if (ActiveReverbEffects.Contains(priority))
		ActiveReverbEffects.Remove(priority);
	else
		UE_LOG(LogDungeons, Warning, TEXT("Popping Reverb %s that was not added through Manager!"), *GetEnumValueToString(priority));

	UGameplayStatics::DeactivateReverbEffect(GetWorld(), asId(priority));
}

UReverbEffect* USoundMixManager::GetReverbAt(EDungeonsMusicPriority priority) const {
	if (auto* reverbEffect = ActiveReverbEffects.Find(priority))
		return *reverbEffect;

	return nullptr;
}


void USoundMixManager::EnableSoundMixes() {
	for (auto* soundMix : GlobalSoundMixes)
		UGameplayStatics::PushSoundMixModifier(GetWorld(), soundMix);
}

void USoundMixManager::DisableSoundMixes() {
	for (auto* soundMix : GlobalSoundMixes)
		UGameplayStatics::PopSoundMixModifier(GetWorld(), soundMix);
}

void USoundMixManager::PopAllSoundMixes() {
	for (TWeakObjectPtr<USoundMix> soundMixPtr : ActiveSoundMixes) {
		UGameplayStatics::PopSoundMixModifier(GetWorld(), soundMixPtr.Get());
	}
	ActiveSoundMixes.Reset();
}

#if !UE_BUILD_SHIPPING 
	void USoundMixManager::PrintDebugInfo() {
		GEngine->AddOnScreenDebugMessage(int32(GetClass()->GetUniqueID()), 1.f, FColorList::SlateBlue, TEXT("Active Reverbs:"));
		int i = 1;
		for (const auto r : ActiveReverbEffects)
		{
			if (r.Value)
				GEngine->AddOnScreenDebugMessage(int32(GetClass()->GetUniqueID()) + ++i, 1.f, FColorList::SlateBlue, FString::Printf(TEXT(".%s (PRIO:%s)")
					, *r.Value->GetFName().ToString(), *GetEnumValueToString(r.Key)));
		}

		GEngine->AddOnScreenDebugMessage(int32(GetClass()->GetUniqueID()), 1.f, FColorList::Gold, TEXT("Active Soundmixes:"));

		for (auto mix : ActiveSoundMixes) {
			GEngine->AddOnScreenDebugMessage(int32(GetClass()->GetUniqueID()) + ++i, 1.f, FColorList::Gold, FString::Printf(TEXT(".%s")
				, *mix->GetFName().ToString()));
		}
	}
#endif
