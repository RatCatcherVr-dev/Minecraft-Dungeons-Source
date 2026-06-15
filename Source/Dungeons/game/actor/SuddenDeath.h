#pragma once

#include "character/player/PlayerCharacter.h"
#include "game/actor/Dimmer.h"
#include "GameplayEffect.h"
#include "SuddenDeath.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSuddenDeathCountStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSuddenDeathCountStopped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSuddenDeathStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSuddenDeathEnded);

UENUM()
enum class ESuddenDeathMode : uint8 {
	Always, //Starts whenever night mode is active, this is the default
	OnAnyPlayerDowned, //Will only start counting whenever a player is down and we
};

UCLASS(NotBlueprintable)
class DUNGEONS_API ASuddenDeath : public AActor {
	GENERATED_BODY()

public:
	ASuddenDeath(const FObjectInitializer& objectInitializer);
	
	void BeginPlay() override;

	FActiveGameplayEffectHandle ApplyEffect(APlayerCharacter* player, float damage);
	
	void Tick(float DeltaTime) override;

	UPROPERTY(EditInstanceOnly, Category = "SuddenDeath")
	float timeLimit = 25.f;
	
	UPROPERTY(EditInstanceOnly, Category = "SuddenDeath")
	float damageFractionStart = 0.25f;
	
	UPROPERTY(EditInstanceOnly, Category = "SuddenDeath")
	float damageFractionEnd = 1.0f;
	
	UPROPERTY(EditInstanceOnly, Category = "SuddenDeath")
	float damagePeriodStart = 4.f;
	
	UPROPERTY(EditInstanceOnly, Category = "SuddenDeath")
	float damagePeriodEnd = 2.f;
	
	UPROPERTY(EditInstanceOnly, Category = "SuddenDeath")
	float startToEndDuration = 15.f;
		
	void OnSuddenDeathCountStarted();
	void OnSuddenDeathCountStopped();
	void OnSuddenDeathEnded();
	void OnSuddenDeathStarted();

	UFUNCTION(BlueprintCallable, Category = "SuddenDeath")
	bool IsSuddenDeathCountingDown() const;

	UFUNCTION(BlueprintCallable, Category = "SuddenDeath")
	float GetTimeSecondsUntilSuddenDeath() const;
	
	UFUNCTION(BlueprintCallable, Category = "SuddenDeath")
	bool IsSuddenDeathActive() const;

	UFUNCTION(BlueprintCallable, Category = "SuddenDeath")
	void ToggleSuddenDeathInterruption();

	UPROPERTY(BlueprintAssignable, Category = "SuddenDeath")
	FOnSuddenDeathCountStarted SuddenDeathCountStarted;

	UPROPERTY(BlueprintAssignable, Category = "SuddenDeath")
	FOnSuddenDeathCountStopped SuddenDeathCountStopped;

	UPROPERTY(BlueprintAssignable, Category = "SuddenDeath")
	FOnSuddenDeathStarted SuddenDeathStarted;

	UPROPERTY(BlueprintAssignable, Category = "SuddenDeath")
	FOnSuddenDeathEnded SuddenDeathEnded;

	void SetMode(ESuddenDeathMode);

protected:
	UPROPERTY(EditInstanceOnly, Category = "SuddenDeath")
	ESuddenDeathMode Mode = ESuddenDeathMode::Always;

private:
	void ExitSuddenDeath();
	void ExitNight();
	bool ShouldEnterNightMode() const;
	void ApplyEffectWithinRadius(AActor* center, float radius, const FGameplayEffectSpec spec);

	bool nightModeActive = false;	
	bool suddenDeathModeActive = false;	
	bool suddenDeathInterrupted = false;

	float nightModeStartStamp = 0.f;
	float suddenDeathDamageApplyDelay = 0.f;

	TWeakObjectPtr<ADimmer> mDimmer;

	float getTimeSpentInSuddenDeathMode() const;
};

UCLASS()
class DUNGEONS_API USuddenDeathDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USuddenDeathDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API USuddenDeathOceansDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USuddenDeathOceansDamageGameplayEffect();
};