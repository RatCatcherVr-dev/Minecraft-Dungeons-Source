#pragma once

#include "Components/ActorComponent.h"
#include "DungeonsGameState.h"
#include "DungeonsGameInstance.h"
#include "game/GamplayEffects/Drowning.h"
#include "OxygenComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnOxygenChanged, float, oldValue, float, newValue, float, maxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOxygenFractionChanged, float, oldFraction, float, newFraction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsUnderwaterChanged, bool, underwater);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotifyLowOxygen, float, fraction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInstantFillOxygen);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), DisplayName = "OxygenComponent")
class DUNGEONS_API UOxygenComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UOxygenComponent();

	UFUNCTION()
	void BeginPlay() override;
	UFUNCTION()
	void OnUnderwaterTagChange(const FGameplayTag Tag, int32 Count);
	UFUNCTION()
	void OnWaterBreathingTagChange(const FGameplayTag Tag, int32 Count);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetIsUnderwater(bool Underwater);
	UFUNCTION(NetMulticast, reliable, Category = "Dungeons")
	void BroadcastUnderwaterDelegate(bool Underwater);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool GetIsUnderwater() { return IsUnderwater; };
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void FillOxygen();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void EnableWaterBreathing(bool enabled);

	void OnDownOrDeath();
	void OnReviveOrRespawn();

	UPROPERTY(Category = "Dungeons", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0))
	float Oxygen = 30;
	UPROPERTY(Category = "Dungeons", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0))
	float MaxOxygen = 30;
	UPROPERTY(Category = "Dungeons", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0))
	float LowOxygenFractionThreshold = 0.3f;
	UPROPERTY(Category = "Dungeons", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, UIMin = 0))
	FGameplayTag LowOxygenNotifyEffect;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetCurrentOxygen();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetMaximumOxygen();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetCurrentOxygenPercentage();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool GetIsOxygenLow();

	bool ShowDebugMessage = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons|GameplayEffects")
	TSubclassOf<UDrowningUIGameplayEffect> DrowningUIEffect;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnOxygenChanged OnOxygenChanged;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnOxygenFractionChanged OnOxygenFractionChanged;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnIsUnderwaterChanged OnIsUnderwaterChanged;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnNotifyLowOxygen OnNotifyLowOxygen;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnInstantFillOxygen OnInstantFillOxygen;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

	void OnAttributeOxygenChange(const FOnAttributeChangeData& data);
	void FillOxygenWithoutEffects();
	void ApplyOxygenEffects();
	void RemoveOxygenEffects();
	UAbilitySystemComponent* GetAbilitySystem();
	UFUNCTION()
	void OnNotifyOxygenLow();

	UPROPERTY(Replicated)
	bool IsUnderwater = false;

	FActiveGameplayEffectHandle WaterBreathingHandle;
	FActiveGameplayEffectHandle OxygenLowEffectHandle;
	FActiveGameplayEffectHandle DrowningUIEffectHandle;

	TOptional<float> PreviouslySeenOxygen;
	TOptional<float> CachedOxygenFraction;

	ADungeonsGameState* GameState;
	UDungeonsGameInstance* GameInstance;

	FTimerHandle OxygenLowNotifyTimerHandle;

	UFUNCTION()
	void OnDrowning(bool isDrowning);
	UFUNCTION(Client, Reliable)
	void Client_OnDrowning(bool isDrowning);
};