#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "StaggerComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStun, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UStaggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStaggerComponent();
	
	float Endurance = 100.f;

	/** The amount of time the mob is in a stunned state. Set to 0 if stunning should be disabled; staggering still applies. */
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Stun")
	float Duration = 0.5f;
protected:
	virtual void BeginPlay() override;

private:
	FActiveGameplayEffectHandle RecoveryHandle;

	void OnAttributeMaxEnduranceChanged(const FOnAttributeChangeData& data);
	void OnAttributeEnduranceChanged(const FOnAttributeChangeData& data);

	FGameplayTagContainer IgnoredDamageTypes;

	void Recover();


	class UAbilitySystemComponent* GetAbilitySystem() const;

	/** The amount of stunning 'endurance' the mob recovers per second. Every mob starts with a maximum of 100 endurance. */
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Stun")
	float RecoverPerSecond = 20.f;

	/** Multiply 'endurance damage' with this constant. Should typically be between 0 and 1. */
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Stun")
	float Multiplier = 10.f;
};