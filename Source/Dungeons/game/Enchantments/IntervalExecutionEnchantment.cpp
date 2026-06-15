// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "IntervalExecutionEnchantment.h"

UIntervalExecutionEnchantment::UIntervalExecutionEnchantment() {}

void UIntervalExecutionEnchantment::OnStart() {
	Super::OnStart();
	bHasStarted = true;
	if(ShouldIntervalExecute()){
		ResetTimer();
		IntervalExecutionStarted();
	}
}

void UIntervalExecutionEnchantment::OnEnd() {
	Super::OnEnd();
	bHasStarted = false;
	IntervalExecutionStopped();
}

bool UIntervalExecutionEnchantment::ShouldIntervalExecute() const {
	return bHasStarted;
}

void UIntervalExecutionEnchantment::OnIntervalExecute() {
	if (ShouldIntervalExecute()) {
		Execution();
		ResetTimer();
	} else {
		IntervalExecutionStopped();
	}
}

float UIntervalExecutionEnchantment::GetExecutionInterval() const {
	return 1.0f;
}

void UIntervalExecutionEnchantment::ResetTimer() {
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UIntervalExecutionEnchantment::OnIntervalExecute, GetExecutionInterval());
}
