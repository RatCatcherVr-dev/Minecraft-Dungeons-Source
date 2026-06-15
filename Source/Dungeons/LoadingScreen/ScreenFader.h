// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "ScreenFader.generated.h"

DECLARE_DELEGATE(FOnFadeComplete)

UCLASS()
class DUNGEONS_API AScreenFader : public AInfo {
	GENERATED_BODY()
public:
	AScreenFader();

	void BeginPlay() override;

	void Tick(float deltaTime) override;

	void StartFade(bool fadeIn, float fadeTime, FOnFadeComplete onFadeComplete);

	FOnFadeComplete OnFadeComplete;

	UPROPERTY()
	UCameraComponent* CameraComp;
private:
	void SetFadeStrength(float fraction);

	UFUNCTION()
	void OnCompletedFade();

	FTimerHandle FadeTimerHandle;

	float FadeTime;

	bool bFadeIn;
};
