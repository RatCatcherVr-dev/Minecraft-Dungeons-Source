// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTypes.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.h"
#include <AbilitySystemComponent.h>
#include "HealthBarComponent.generated.h"



UCLASS(ClassGroup = (Custom), BlueprintType)
class DUNGEONS_API UHealthBarComponent : public UActorComponent
{

	class SmoothHealthState {

	private:
		static const float INTERPOLATION_DURATION;
		static const float INTERPOLATION_EXPONENT;

		static const float FADE_OUT_DELAY;
		static const float FADE_OUT_DURATION;

		static const float HEALTHBAR_CHANGED_PERCENTAGE_THRESHOLD;

		float startPercentage;
		float targetPercentage;
		float startTime = -FLT_MAX;

	public:
		SmoothHealthState() {
			startPercentage = 1.0f;
			targetPercentage = 1.0f;
			startTime = -FLT_MAX;
		}

		SmoothHealthState(float currentHealthPercentage, float atTime) {
			startPercentage = currentHealthPercentage;
			targetPercentage = currentHealthPercentage;
			startTime = atTime;
		}

		void SetTargetPercentage(float currentHealthPercentage, float atTime) {
			const auto clampedPercentage = FMath::Clamp(currentHealthPercentage, 0.f, 1.f);
			if (!FMath::IsNearlyEqual(clampedPercentage,targetPercentage, HEALTHBAR_CHANGED_PERCENTAGE_THRESHOLD)) {
				startPercentage = GetSmoothPercentage(atTime);
				targetPercentage = clampedPercentage;
				startTime = atTime;
			}
		}

		float GetSmoothPercentage(float atTime) const {
			auto alpha = FMath::Pow(FMath::Min(1.0f, (atTime - startTime) / INTERPOLATION_DURATION), INTERPOLATION_EXPONENT);
			return FMath::LerpStable(startPercentage, targetPercentage, alpha);
		}

		float GetTargetPercentage() const {
			return targetPercentage;
		}

		float GetOpacity(float atTime) const {
			const float increasingFadeTime = FMath::Clamp( (atTime - (startTime + INTERPOLATION_DURATION + FADE_OUT_DELAY)) / FADE_OUT_DURATION, 0.0f, 1.0f);
			return 1.0f - increasingFadeTime;
		}

		bool IsDoneInterpolating(float atTime) const {
			return GetSmoothPercentage(atTime) == targetPercentage;
		}

		bool IsDoneFading(float atTime) const {
			return GetOpacity(atTime) <= 0.0f;
		}
	};


	GENERATED_BODY()
	static const float SHOW_HEALTHBAR_HEALTHPERCENTAGE_THRESHOLD;

protected:
	void BeginPlay() override;	

public:
	UHealthBarComponent();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetHealthPercentage() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetSmoothHealthPercentage() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetScale() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetOpacity() const;

	void RefreshHidden();

	void SetDisplayedGlobally(bool globally);	

private:
	UAbilitySystemComponent* GetAbilitySystem() const;

	void OnMaxHealthAttributeChanged(const FOnAttributeChangeData& data);

	void RefreshScale();

	void OnInvisibilityChanged(FGameplayTag, int32 tagCount);

	TWeakObjectPtr<UHealthComponent> healthComponent;

	void HealthChanged(const FOnAttributeChangeData& data);
	void HealthChanged(float amount);

	void UpdateHealth();

	Unique<SmoothHealthState> SmoothState;

	inline float Now() const {
		return GetOwner()->GetWorld()->GetTimeSeconds();
	}

	float Scale = 1.0f;

	float MinOpacity = 0.0f;

	bool bDisplayedGlobally = false;
	bool bHidden = false;	
};
