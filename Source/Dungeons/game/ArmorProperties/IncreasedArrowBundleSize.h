// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "game/util/ValueFormat.h"
#include "IncreasedArrowBundleSize.generated.h"


UCLASS()
class DUNGEONS_API UIncreasedArrowBundleSize : public UArmorProperty
{
	GENERATED_BODY()
public:
	UIncreasedArrowBundleSize();

	void BeginPlay() override;

protected:
	FString createFormattedValueString() const override { return valueformat::asRelativeConstant(ArrowIncreaseAmount); };

	UPROPERTY(EditDefaultsOnly)
	int ArrowIncreaseAmount = 5;

private:
	UFUNCTION()
	void OnArrowsPickedUp(class UItemSlot* slot);
};
