#pragma once

#include "Components/ActorComponent.h"
#include "GlobalHealthBarComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueChanged, float, fraction);

UCLASS(Blueprintable, ClassGroup = (Custom))
class DUNGEONS_API UGlobalHealthBarComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UGlobalHealthBarComponent();

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnValueChanged OnValueChanged;

	float Value = 1.0f;
	FDelegateHandle OnDeathHandle;
	FDelegateHandle OnHealthFractionChangedHandle;

	UFUNCTION(BlueprintImplementableEvent)
	void OnAlive();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();
private:
	UFUNCTION()
	void OnDeath_Internal();

	void OnHealtFractionChanged(float newFraction, float oldFraction);
	TWeakObjectPtr<class UHealthComponent> HealthComponent;
};
