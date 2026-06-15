// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "DungeonsGameViewportClient.generated.h"

/**
 * 
 */

 //D11.PS
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInputKeyPressed, int/* ControllerId*/, FKey /*Key*/);
typedef FOnInputKeyPressed::FDelegate FOnInputKeyPressedDelegate;
//D11.SSN
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInputKeyReleased, int/* ControllerId*/, FKey /*Key*/);
typedef FOnInputKeyReleased::FDelegate FOnInputKeyReleasedDelegate;

UCLASS()
class DUNGEONS_API UDungeonsGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()
	
public:
	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;

	virtual void PostRender(UCanvas* Canvas) override;

	void SetColorFilterStrength(float filterStrength);

	void SetFilterColor(FLinearColor color);

	//D11.PS - functions added to get the controller id from the key inputs
	virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;

	void RawInputKey(const bool released, const FPointerEvent& InMouseEvent);
	void RawInputKey(const bool released, const FKeyEvent& InKeyEvent);
	void RawInputKey(const bool released, int controllerId, FKey key, bool isRepeat);
	void RawInputKey(const bool released, int controllerId, FKey key) { RawInputKey(released, controllerId, key, false); };

	//D11.SSN - added delegates to register key/button up
	FDelegateHandle AddOnInputKeyPressedDelegate_Handle(const FOnInputKeyPressedDelegate &InputKeyDelegate);
	FDelegateHandle AddOnInputKeyReleasedDelegate_Handle(const FOnInputKeyReleasedDelegate &InputKeyDelegate);

	FOnInputKeyPressed InputKeyPressedDelegate;
	FOnInputKeyReleased InputKeyReleasedDelegate;

private:
	FLinearColor FadeColor = FLinearColor::Black;

	float ColorFilterStrength = 0.0f;

	void DrawColorFilter(UCanvas* Canvas);
};
