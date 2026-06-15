#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/Canvas.h"
#include <GameplayEffectUIData.h>
#include <UserWidget.h>
#include "game/Enchantments/Enchantment.h"
#include "DungeonsGameplayEffectUIData.generated.h"

UENUM(BlueprintType)
enum class EStackCountDisplayPreference : uint8 {
	// 5/10 displays as 5/10
	AsCounter,
	// Displays magnitude multiplied by stack count
	AsMagnitude,
	// 5/10 displays as 50%
	AsPercentage
};

UCLASS(BlueprintType)
class DUNGEONS_API UDungeonsGameplayEffectUIData : public UGameplayEffectUIData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	FText name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	TSubclassOf<UUserWidget> iconClass;

	UPROPERTY(BlueprintReadOnly)
	TSoftClassPtr<UUserWidget> softIconClassRef;

	//Only works if effect has a Max Stack count. If set to true, will display as percentage instead of current/max
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	EStackCountDisplayPreference displayPreference = EStackCountDisplayPreference::AsCounter;

	//Used for stacking effects. Added to the actual stack count before displaying.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	int32 displayStackCountOffset = 0;
	
	//Used for stacking effects. Added to the max stack count before displaying.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	int32 displayMaxStackCountOffset = 0;

	//Uses the attribute to acquire the magnitude of the gameplay effect.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	FGameplayAttribute displayMagnitudeAttribute;

	//Adds to the magnitude multiplied by the stack count before displaying.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	float displayMagnitudeOffset = 0.0f;

	//Determines how many fractional digits to show when displaying the magnitude.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	int32 displayMagnitudeFractionalDigitsCount = 3;

	//Determines how frequent to update the duration if changed.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	float displayDurationUpdateFrequency = 0.f;
	
};
