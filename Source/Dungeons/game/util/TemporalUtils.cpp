// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "TemporalUtils.h"

ATemporalDilationNormalizer* UTemporalUtils::DilateTime(const UObject* worldContextObject, const float dilation, TArray<AActor*> targets, float seconds, bool dilateSound) {

	float previousDilation = UGameplayStatics::GetGlobalTimeDilation(worldContextObject);

	UGameplayStatics::SetGlobalTimeDilation(worldContextObject, dilation);

	TArray<AActor*> actorsInWord;
	UGameplayStatics::GetAllActorsOfClass(worldContextObject, AActor::StaticClass(), actorsInWord);
	for (AActor* actor : actorsInWord) {	
		actor->CustomTimeDilation = targets.Contains(actor) ? 1.f : 1.f / dilation;
	}

	if (dilateSound) {
		UGameplayStatics::SetGlobalPitchModulation(worldContextObject, 0, 0);
		UGameplayStatics::SetGlobalPitchModulation(worldContextObject, 1, seconds);
	}

	auto normalizer = NewObject<ATemporalDilationNormalizer>(worldContextObject->GetWorld(), ATemporalDilationNormalizer::StaticClass());
	normalizer->Start(worldContextObject, targets, dilation, seconds);
	return normalizer;
}

void ATemporalDilationNormalizer::Start(const UObject* worldContextObject, TArray<AActor*> targets, float dilation, float seconds) {
	WorldContextObject = worldContextObject->GetWorld();
	Seconds = seconds;
	Targets = targets;
	Dilation = dilation;
	StartTimerstamp = this->WorldContextObject->GetWorld()->GetRealTimeSeconds();
	this->WorldContextObject->GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ATemporalDilationNormalizer::normalize, 0.01f, true);
}

void ATemporalDilationNormalizer::Affect(AActor* actor) {
	actor->CustomTimeDilation = 1.f;
}

void ATemporalDilationNormalizer::Unaffect(AActor* actor) {
	float globalDilation = UGameplayStatics::GetGlobalTimeDilation(WorldContextObject);
	actor->CustomTimeDilation = 1.f / globalDilation;
}

void ATemporalDilationNormalizer::normalize() {

	float time = this->WorldContextObject->GetWorld()->GetRealTimeSeconds();

	float delta = time - StartTimerstamp;

	float weight = FMath::Clamp(delta / Seconds, 0.f, 1.f);

	float newDilation = weight * (1.f / Dilation);

	if (weight < 1.f) {
		for (auto target : Targets) {
			if (target->IsValidLowLevel()) {
				target->CustomTimeDilation = newDilation;
			}
		}
		return;
	}

	UGameplayStatics::SetGlobalTimeDilation(WorldContextObject, 1.f);
	TArray<AActor*> actorsInWord;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AActor::StaticClass(), actorsInWord);
	for (AActor* actor : actorsInWord) {
		actor->CustomTimeDilation = 1.f;
	}
	WorldContextObject->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	this->Destroy();
}