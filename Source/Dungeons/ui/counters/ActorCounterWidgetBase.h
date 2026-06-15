// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CounterWidgetBase.h"
#include "ActorCounterWidgetBase.generated.h"

class AActor;

UCLASS(Abstract)
class DUNGEONS_API UActorCounterWidgetBase : public UCounterWidgetBase
{
	GENERATED_BODY()

protected:	
	virtual TOptional<int> FetchBoundValue(const AActor&) const;

	void Refresh();

	void Bind(AActor* actor);

	virtual void UnbindFrom(AActor&) {};
	virtual void BindTo(AActor&) {};	

	UPROPERTY()
	AActor* mBoundActor;
};
