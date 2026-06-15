#pragma once

#include <CoreMinimal.h>
#include "Engine/LocalPlayer.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "DungeonsLocalPlayer.generated.h"

UCLASS()
class DUNGEONS_API UDungeonsLocalPlayer : public ULocalPlayer {
	GENERATED_UCLASS_BODY()
public:
	FString GetGameLoginOptions() const override;

	void RefreshDeadzones(ABasePlayerController* BasePlayerController);

	void SetMouseSensitivityIngame(float value) { MouseSensitivityIngame = value; }
	void SetMouseSensitivityMenu(float value) { MouseSensitivityMenu = value; }
	void SetDeadzoneLStick(float value) { DeadzoneIngameLStick = value; }
	void SetDeadzoneRStick(float value) { DeadzoneIngameRStick = value; }

	int32 GetSystemUserId();
	void SetUserSystemId(int32 SystemId);
private:
	float MouseSensitivityIngame = 0.5f;
	float MouseSensitivityMenu = 0.5f;
	float DeadzoneIngameLStick = 0.2f;
	float DeadzoneIngameRStick = 0.2f;
	const float DeadzoneUI = 0.5f;
	const float DeadzoneTriggers = 0.2f;

	int32 UserSystemId = 0;
};