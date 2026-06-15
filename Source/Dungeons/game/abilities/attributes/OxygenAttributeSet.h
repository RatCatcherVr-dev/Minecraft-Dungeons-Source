#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeUtil.h"
#include "UnrealNetwork.h"
#include "OxygenAttributeSet.generated.h"

UCLASS()
class DUNGEONS_API UOxygenAttributeSet : public UAttributeSet {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Oxygen, Category = "Dungeons")
	float Oxygen = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_MaxOxygen, Category = "Dungeons")
	float MaxOxygen = 30;

	DECLARE_ATTRIBUTE_FUNCTION(Oxygen);
	DECLARE_ATTRIBUTE_FUNCTION(MaxOxygen);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	*	Called just before any modification happens to an attribute. This is lower level than PreAttributeModify/PostAttribute modify.
	*	There is no additional context provided here since anything can trigger this. Executed effects, duration based effects, effects being removed, immunity being applied, stacking rules changing, etc.
	*	This function is meant to enforce things like "Health = Clamp(Health, 0, MaxHealth)" and NOT things like "trigger this extra thing if damage is applied, etc".
	*
	*	NewValue is a mutable reference so you are able to clamp the newly applied value as well.
	*/
	void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/**
	*	This is called just before any modification happens to an attribute's base value when an attribute aggregator exists.
	*	This function should enforce clamping (presuming you wish to clamp the base value along with the final value in PreAttributeChange)
	*	This function should NOT invoke gameplay related events or callbacks. Do those in PreAttributeChange() which will be called prior to the
	*	final value of the attribute actually changing.
	*/
	void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

protected:

	UFUNCTION()
	void OnRep_Oxygen();

	UFUNCTION()
	void OnRep_MaxOxygen();
};