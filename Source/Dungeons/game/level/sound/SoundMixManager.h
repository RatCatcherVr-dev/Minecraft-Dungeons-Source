#pragma once

#include "CoreMinimal.h"
#include "game/level/sound/MusicSetTypes.h"
#include "SoundMixManager.generated.h"

class USoundMix;

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API USoundMixManager : public UObject {
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void PushSoundMix(USoundMix* soundMix);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void PopSoundMix(USoundMix* soundMix);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void PushReverb(UReverbEffect* soundMix, EDungeonsMusicPriority priority, float FadeIn = 0.f);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void PopReverb(EDungeonsMusicPriority priority);

	// returns nullptr if no reverb is in slot:
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UReverbEffect* GetReverbAt(EDungeonsMusicPriority priority) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void EnableSoundMixes();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void DisableSoundMixes();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void PopAllSoundMixes();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	TSet<USoundMix*> GlobalSoundMixes;

#if !UE_BUILD_SHIPPING
	void PrintDebugInfo();
#endif
	///// todo: add for tweaking soundMixes..

private:
	UPROPERTY()
	TSet<TWeakObjectPtr<USoundMix>> ActiveSoundMixes;
	UPROPERTY()
	TMap<EDungeonsMusicPriority, UReverbEffect*> ActiveReverbEffects;

};
