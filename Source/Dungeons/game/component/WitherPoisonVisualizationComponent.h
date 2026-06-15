#pragma once

#include "Components/ActorComponent.h"
#include <GameplayEffectTypes.h>
#include <GameplayPrediction.h>
#include "WitherPoisonVisualizationComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWitherFractionChanged, float, fractionRemaining, bool, newWitherApplied);

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UWitherPoisonVisualizationComponent : public UActorComponent {
	GENERATED_BODY()
public:
	UWitherPoisonVisualizationComponent();

	void BeginPlay();

	UPROPERTY(BlueprintAssignable)
	FOnWitherFractionChanged OnWitherFractionChanged;
private:
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	class ABaseCharacter* GetCharacterOwner() const;

	void OnWitherEffectTagCountChanged(const FGameplayTag tag, const int32 tagCount);

	float CachedWitherPoisonFraction = 0.f;
	float CachedWitherDuration = 0.f;

	void OnEffectDurationChanged(FActiveGameplayEffectHandle, float NewStartTime, float NewDuration);
};
