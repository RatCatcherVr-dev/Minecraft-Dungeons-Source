// Fill out your copyright notice in the Description page of Project Settings.

#include "game/component/MapPinComponent.h"
#include "ObjectiveInterestPoint.h"

AObjectiveInterestPoint::AObjectiveInterestPoint() {
	MapPinComponent = CreateDefaultSubobject<UMapPinComponent>("MapPinComponent");
	InitialLifeSpan = 10.f;
}
