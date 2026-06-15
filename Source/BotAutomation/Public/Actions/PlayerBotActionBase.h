#pragma once

class APlayerAutomator;
class APlayerControllerBase;
class ABasePlayerController;
class APlayerCharacter;

enum class EPlayerBotActionResult
{
	Stopped,
	Success,
	InProgress,

	// All further values indicate errors.
	Error,
	ErrorTimedOut
};

class BOTAUTOMATION_API PlayerBotActionBase
{
public:
	PlayerBotActionBase();
	PlayerBotActionBase(float ActionRuntime);
	PlayerBotActionBase(float ActionRuntime, FString ActionName);
	virtual ~PlayerBotActionBase();

	void SetPlayerAutomator(APlayerAutomator* Automator);
	void RemovePlayerAutomator();

	virtual void StartAction();
	void UpdateAction(float DeltaSeconds);
	virtual void StopAction();
	void ResetAction();

	bool IsRunning() const { return bLogicRunning; };
	bool IsFinished() const 
	{
		return bLogicStarted && !bLogicRunning && (Status == EPlayerBotActionResult::Stopped || Status == EPlayerBotActionResult::Success || Status >= EPlayerBotActionResult::Error);
	}

	EPlayerBotActionResult GetStatus() const { return Status; }
	FString GetStatusText() const;

	FString UserFriendlyName();
	virtual void DebugDraw();

protected:
	virtual bool ExecuteAction(float DeltaSeconds);
	void SetStatus(EPlayerBotActionResult status);
	void EndAction(EPlayerBotActionResult status);

	APlayerAutomator* PlayerAutomator;
	APlayerControllerBase* GetPlayerControllerBase();
	ABasePlayerController* GetBasePlayerController();
	APlayerCharacter* GetPlayerCharacter(class APlayerControllerBase* Controller = nullptr);

private:
	bool bLogicStarted;
	bool bLogicRunning;

	FString BotActionName;

	EPlayerBotActionResult Status;
	float RuntimeRemaining;
	float OrginalRuntime;
};
