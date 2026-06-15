#pragma once
#include <GameplayEffect.h>
#include <SubclassOf.h>
#include <Object.h>
#include "ItemPowerEffect.generated.h"

class AItemInstance;
class ABaseCharacter;

UCLASS(Abstract)
class DUNGEONS_API UItemPowerEffect : public UObject {
	GENERATED_BODY()
public:
	typedef std::function<float(float)> ItemPowerToMultiplierFunction;
	typedef std::function<FString(float)> PowerMultiplierFormattedValueFunction;
	typedef std::function<FString(const ABaseCharacter& owner, const AItemInstance*, float)> PowerMultiplierAbsoluteFormattedValueFunction;
	
	float GetMultiplier(float power) const;
	FText CreateText(float power) const;
	FText CreateAbsoluteText(const ABaseCharacter& owner, const AItemInstance* instance, float power) const;
	bool IsHiddenFromPlayer() const;	

	TSubclassOf<UGameplayEffect> GameplayEffect;
protected:	
	ItemPowerToMultiplierFunction ItemPowerFunc;	
	FText PowerEffectTemplate;
	FText PowerEffectAbsoluteTemplate;
	PowerMultiplierFormattedValueFunction PowerMultiplierFormatter;
	PowerMultiplierAbsoluteFormattedValueFunction PowerMultiplierAbsoluteFormatter;
	bool HiddenFromPlayer = false;
};
