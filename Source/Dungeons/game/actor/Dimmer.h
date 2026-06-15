#pragma once

#include "character/player/PlayerCharacter.h"
#include "game/Game.h"
#include "GameFramework/Actor.h"
#include "Dimmer.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnBecomingNightChanged);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnteringNightDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitingNightDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNightFallenDelegate);

USTRUCT(BlueprintType)
struct DUNGEONS_API FNightColor
{
	GENERATED_BODY()

	FNightColor();
	FNightColor(FLinearColor color);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	bool bCustomColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	FLinearColor NightColor;
};

UCLASS(NotBlueprintable)
class DUNGEONS_API ADimmer : public AActor {
	GENERATED_BODY()

public:
	ADimmer(const FObjectInitializer& objectInitializer);

	void BeginPlay() override;	

	void Tick(float deltaTime) override;

	void OnEnterNight();
	void OnEnteredNight();
	void OnExitNight();

	UFUNCTION(BlueprintCallable)
	float GetNightProgress() const;

	float GetNightIntensity() const;

	float GetLightIntensity() const;
	
	void EnterNight();

	void ExitNight();

	bool IsNight() const;

	bool IsBecomingNight() const;

	bool IsBecomingDay() const;

	bool isOceanNight();

	FNightColor GetNightModeLight();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	const game::Game* Game();
	
	FOnBecomingNightChanged OnBecomingNightChanged;

	UPROPERTY(BlueprintAssignable)
	FOnEnteringNightDelegate OnEnteringNight;

	UPROPERTY(BlueprintAssignable)
	FOnExitingNightDelegate OnExitingNight;

	UPROPERTY(BlueprintAssignable)
	FOnNightFallenDelegate OnNightFallen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundCue* NightSoundCue = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundMix* NightSoundMix = nullptr;
	bool NightSoundMixSet = false;
	
	
private:
	UPROPERTY(EditInstanceOnly)
	UMaterialParameterCollection* materialParameterCollection;

	UPROPERTY(EditInstanceOnly)
	float nightLightExponent = 3.0f;

	UPROPERTY(EditInstanceOnly)
	float nightFractionThreshold = 0.5f;

	UPROPERTY(EditInstanceOnly)
	float fadeToNightDuration = 30.0f;

	UPROPERTY(EditInstanceOnly)
	float fadeToDayDuration = 5.0f;

	void SetSign(float newSign);

	void PlayNightSound();
	void StopNightSound();

	UFUNCTION()
	void OnRep_Sign();

	UFUNCTION()
	void OnRep_DayFraction();

	UPROPERTY(ReplicatedUsing = OnRep_DayFraction)
	float dayFraction = 1.0f;

	float visualizedDayFraction = 1.0f;

	UPROPERTY(ReplicatedUsing = OnRep_Sign)
	float sign = 0.0f;
	float oldSign = 0.0f;	

	game::Game* game;

	TWeakObjectPtr<UMaterialParameterCollectionInstance> materialParameterCollectionInstance;

	UPROPERTY()
	class UAudioComponent* NightSoundAudioComponent = nullptr;

};