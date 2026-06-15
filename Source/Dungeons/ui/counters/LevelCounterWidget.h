// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnCounterWidgetBase.h"
#include "LevelCounterWidget.generated.h"

class UPlayerExperienceComponent;

UCLASS()
class DUNGEONS_API ULevelCounterWidget : public UPawnCounterWidgetBase
{
	GENERATED_BODY()
protected:
	TOptional<int> FetchBoundValue(const AActor&) const override;
	void BindTo(AActor&) override;
	void UnbindFrom(AActor&) override;

	UPROPERTY()
	UPlayerExperienceComponent* mExperience;
};
