// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "ScreenFader.h"
#include "game/util/ActorQuery.h"
#include <Runtime/Engine/Classes/Camera/CameraActor.h>
#include <Runtime/Engine/Classes/Camera/CameraComponent.h>
#include "DungeonsGameViewportClient.h"
#include <Engine.h>

AScreenFader::AScreenFader() {
	PrimaryActorTick.bCanEverTick = true;
}

void AScreenFader::BeginPlay() {
}

void AScreenFader::Tick(float deltaTime) {
	Super::Tick(deltaTime);
	auto fadeTimeRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(FadeTimerHandle);
	if (fadeTimeRemaining > 0) {
		auto fadeFraction = fadeTimeRemaining / FadeTime;

		SetFadeStrength(fadeFraction);
	}
}

void AScreenFader::StartFade(bool fadeIn, float fadeTime, FOnFadeComplete onFadeComplete) {
	GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, this, &AScreenFader::OnCompletedFade, fadeTime);

	FadeTime = fadeTime;

	bFadeIn = fadeIn;

	OnFadeComplete = onFadeComplete;
}

void AScreenFader::SetFadeStrength(float fraction) {
	if (auto dungeonsViewport = Cast<UDungeonsGameViewportClient>(GEngine->GameViewport)) {
		dungeonsViewport->SetColorFilterStrength(bFadeIn ? fraction : (1.0f - fraction));
	}
}

void AScreenFader::OnCompletedFade() {
	SetFadeStrength(0.0f);

	FOnFadeComplete ExecHandle = OnFadeComplete;
	OnFadeComplete = nullptr;

	ExecHandle.Execute();
	
}