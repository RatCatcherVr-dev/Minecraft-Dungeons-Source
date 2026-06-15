#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/actor/Dimmer.h"
#include "DownedNightModeComponent.generated.h"

class ABasePlayerState;
class ADungeonsGameState;
class APlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UDownedNightModeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void BeginPlay() override;
	void EnterPermaNight();
private:

	void OnPlayerCharacterAdded(APlayerCharacter* player);
	void OnPlayerAliveStateChanged(APlayerCharacter* player);	

	//Decides if night mode should be active.
	void RefreshNightMode();

	void EnterNight();

	UPROPERTY()
	TWeakObjectPtr<ADimmer> mDimmer;

	ADungeonsGameState* GetGameState() const;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float nightModeDelay = 2.0f;
	
	FTimerHandle mBecomeNightDelayHandle;
	bool mPermanent = false;
};
