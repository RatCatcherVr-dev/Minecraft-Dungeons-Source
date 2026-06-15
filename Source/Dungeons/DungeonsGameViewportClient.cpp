// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "DungeonsGameViewportClient.h"
#include <Runtime/Engine/Classes/Engine/Canvas.h>
#include "DungeonsGameInstance.h"

void UDungeonsGameViewportClient::Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice /*= true*/) {
	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);
	MouseEnter(Viewport, 0, 0); //@todo: hack: Attempted / hacky fix for hardware cursors not being properly initialized
}

void UDungeonsGameViewportClient::PostRender(UCanvas* Canvas) {
	Super::PostRender(Canvas);
	if(ColorFilterStrength > 0.0f){
		DrawColorFilter(Canvas);
	}
}

void UDungeonsGameViewportClient::SetColorFilterStrength(float filterStrength) {
	ColorFilterStrength = filterStrength;
}

void UDungeonsGameViewportClient::SetFilterColor(FLinearColor color) {
	FadeColor = std::move(color);
}

void UDungeonsGameViewportClient::DrawColorFilter(UCanvas* Canvas) {
	FColor OldColor = Canvas->DrawColor;
	FadeColor.A = ColorFilterStrength;	
	Canvas->DrawColor = FadeColor.ToFColor(true);
	Canvas->DrawTile(Canvas->DefaultTexture, Canvas->OrgX, Canvas->OrgY, Canvas->ClipX, Canvas->ClipY, 0, 0, Canvas->DefaultTexture->GetSizeX(), Canvas->DefaultTexture->GetSizeY());
	Canvas->DrawColor = OldColor;
}

//D11.PS - override and send the key value with the controller to whoever subscribes
bool UDungeonsGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs) {
	
	auto gameInstance = Cast<UDungeonsGameInstance>(GetGameInstance());

	if (gameInstance && gameInstance->IsLoadingScreenActive())
		return true;
	
	bool Val = UGameViewportClient::InputKey(EventArgs);
	//D11.KS - Ignore mouse press temporarily
	// D11.DB - Reenabled mouse press as it's required for mouse/gamepad input mode switching.

#if !defined(HAS_RAW_INPUT_LISTENER)
	if (EventArgs.Event == EInputEvent::IE_Released)
	{
		InputKeyReleasedDelegate.Broadcast(EventArgs.ControllerId, EventArgs.Key);
	}
	else
	{
		InputKeyPressedDelegate.Broadcast(EventArgs.ControllerId, EventArgs.Key);
	}
#endif

	return Val;
}

void UDungeonsGameViewportClient::RawInputKey(const bool released, const FPointerEvent& InMouseEvent)
{
	RawInputKey(released, InMouseEvent.GetUserIndex(), InMouseEvent.GetEffectingButton(), InMouseEvent.IsRepeat());
}

void UDungeonsGameViewportClient::RawInputKey(const bool released, const FKeyEvent& InKeyEvent)
{
	RawInputKey(released, InKeyEvent.GetUserIndex(), InKeyEvent.GetKey(), InKeyEvent.IsRepeat());
}

void UDungeonsGameViewportClient::RawInputKey(const bool released, int controllerId, FKey key, bool isRepeat)
{
	auto gameInstance = Cast<UDungeonsGameInstance>(GetGameInstance());

	if (gameInstance && gameInstance->IsLoadingScreenActive())
		return;

	if (released)
	{
		InputKeyReleasedDelegate.Broadcast(controllerId, key);
		gameInstance->OnAnyKeyReleasedControllerIdDelegate.Broadcast(controllerId, key, isRepeat);
	}
	else
	{
		InputKeyPressedDelegate.Broadcast(controllerId, key);
		gameInstance->OnAnyKeyPressedControllerIdDelegate.Broadcast(controllerId, key, isRepeat);
	}
}

//D11.PS
FDelegateHandle UDungeonsGameViewportClient::AddOnInputKeyPressedDelegate_Handle(const FOnInputKeyPressedDelegate &InputKeyDelegate)
{
	return InputKeyPressedDelegate.Add(InputKeyDelegate);
}


FDelegateHandle UDungeonsGameViewportClient::AddOnInputKeyReleasedDelegate_Handle(const FOnInputKeyPressedDelegate &InputKeyDelegate)
{
	return InputKeyReleasedDelegate.Add(InputKeyDelegate);
}
