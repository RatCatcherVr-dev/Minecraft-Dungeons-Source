// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnCounterWidgetBase.h"
#include "EnchantmentPointsCounterWidget.generated.h"

class UItemStashComponent;

UCLASS()
class DUNGEONS_API UEnchantmentPointsCounterWidget : public UPawnCounterWidgetBase
{
	GENERATED_BODY()
protected:
	TOptional<int> FetchBoundValue(const AActor&) const override;
	void BindTo(AActor&) override;
	void UnbindFrom(AActor&) override;	

	UPROPERTY()
	UItemStashComponent* mitemStash;
};
