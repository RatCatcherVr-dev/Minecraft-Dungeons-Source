// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonsWidgets/DungeonsUserWidget.h"
#include "CounterWidgetBase.generated.h"


UENUM(BlueprintType)
enum class EUICounterEmphasis : uint8 {
	Regular,
	Diminished,
	Emphasized,	
};
ENUM_NAME(EUICounterEmphasis);


UCLASS(Abstract)
class DUNGEONS_API UCounterWidgetBase : public UDungeonsUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnValueChanged(int newValue, int previousValue);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetValue() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetEmphasis(EUICounterEmphasis emphasis);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnEmphasisChanged(EUICounterEmphasis emphasis);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EUICounterEmphasis GetEmphasis() const;

protected:	
	void SetValue(int);

	void ResetValue();
	
	TOptional<int> mValue;

	EUICounterEmphasis mEmphasis = EUICounterEmphasis::Regular;
};
