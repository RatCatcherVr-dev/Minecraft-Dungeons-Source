#pragma once

#include "game/actor/character/mob/MobCharacter.h"

#include "AutoTarget.generated.h"

class ABasePlayerController;

UCLASS(config = Game, defaultconfig)
class UAutoTargetDefaultIdle : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere)
	float Angle;

	UPROPERTY(Config, EditAnywhere)
	float Radius;

	UPROPERTY(Config, EditAnywhere)
	float Weight;
};

UCLASS(config = Game, defaultconfig)
class UAutoTargetDefaultActive : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere)
	float Angle;

	UPROPERTY(Config, EditAnywhere)
	float Radius;

	UPROPERTY(Config, EditAnywhere)
	float Weight;
};

UCLASS(config = Game, defaultconfig)
class UAutoTargetRangedIdle : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere)
	float Angle;

	UPROPERTY(Config, EditAnywhere)
	float Radius;

	UPROPERTY(Config, EditAnywhere)
	float Weight;
};

UCLASS(config = Game, defaultconfig)
class UAutoTargetRangedActive : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere)
	float Angle;

	UPROPERTY(Config, EditAnywhere)
	float Radius;

	UPROPERTY(Config, EditAnywhere)
	float Weight;
};

UCLASS(config = Game, defaultconfig)
class UAutoTargetShootingIdle : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere)
	float Angle;

	UPROPERTY(Config, EditAnywhere)
	float Radius;

	UPROPERTY(Config, EditAnywhere)
	float Weight;
};

UCLASS(config = Game, defaultconfig)
class UAutoTargetShootingActive : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere)
	float Angle;

	UPROPERTY(Config, EditAnywhere)
	float Radius;

	UPROPERTY(Config, EditAnywhere)
	float Weight;
};

// D11.DB - Autotargeting system. Can generate a list of targetable AMobCharacter
//			actors that satisfy specific targeting criteria.
UCLASS(config = Game, defaultconfig)
class AAutoTarget : public AActor
{
	GENERATED_BODY()

public:
	struct MobTarget {
		TWeakObjectPtr<AMobCharacter> Mob;
		float Order = 0;
	};

	enum class Mode {
		Default, Shooting, Ranged
	};

	void BeginPlay();

	// D11.DB - Update the list of targetable Mobs.	
	int Update( ABasePlayerController& controller );

	// D11.DB - Update the outline highlight surrounding the target
	void UpdateHighlight( ABasePlayerController& controller );

	// D11.DB - Removes all current autotarget highlights.
	void ClearHighlight( ABasePlayerController& controller );

	// D11.DB - Fully reset the autotarget system.
	void Clear( ABasePlayerController& controller );

	// D11.DB - Toggle ranged targeting mode.
	void ToggleRangedTarget();

	// D11.DB - Set whether the player is currently shooting/readying their ranged weapon.
	void SetShooting(bool shooting) { bIsShooting = shooting; }

	// D11.DB - Return whether or not we are currently in ranged targeting mode.
	bool GetRangedTargetMode() {
		return IsSuccessfullyRangedTargeting;
	}

	// D11.DB - Get the current value of RangeBoost.
	float GetRangeBoost() {
		return RangeBoost;
	}

	// D11.DB - Get the current targeting mode.
	Mode GetMode();

	// D11.DB - Get a TArray of all the currently targetable mobs.
	const TArray<MobTarget>& GetTargets() { return Targets; }

	// D11.DB - Get the current highest priority targetable mob.
	TWeakObjectPtr< AMobCharacter > GetTarget() { return Targets.Num() > 0 ? Targets[0].Mob : nullptr; }

	// D11.DB - Has any valid targets.
	bool HasTarget() { return Targets.Num() > 0; }

	// D11.DB - Forcibly override the primary target.
	void ForceTarget(AMobCharacter* mob);
	
	// D11.DB - DEBUG - Draw some handy overlays for debugging and otherwise visualising
	//					the autotargeting functionality.
	void DebugDraw( const ABasePlayerController& controller );

	// D11.DJB - Allow user to disable/enable the auto-target system
	void SetEnabled(bool enable) { bEnabled = enable; }

	float GetTimeInPrevState() {
		return TimeInPrevState;
	}
	float GetTimeInCurrentState() {
		return TimeInCurrentState;
	}
	Mode GetPrevMode() {
		return mPrevModeCache;
	}

	// D11.KS 
	void UpdateOnScreenMobs();

	bool IsTargetingStickActive(const ABasePlayerController& controller) const;

private:
	// D11.DB - Disable the line part of the autotargeting cone. It will fallback
	//			to using the wide cone. This is required in order for the ConeWeight
	//			feature to function as expected.
	UPROPERTY(Config, EditAnywhere)
	bool bLineEnabled;

	// D11.DB - Enable debug drawing functionality.
	UPROPERTY(Config, EditAnywhere)
	bool bDebugDraw;

	// D11.DB - Enable debug drawing functionality.
	UPROPERTY(Config, EditAnywhere)
	bool bDebugDrawVerbose;

	// D11.DB - Enable target highlighting.
	UPROPERTY(Config, EditAnywhere)
	bool bHighlightEnabled;

	// D11.DB - Enable the toggleable ranged target mode 
	UPROPERTY(Config, EditAnywhere)
	bool bRangedTargetModeEnabled;

	// D11.DB - Force ranged targetting mode to use the radial method.
	UPROPERTY(Config, EditAnywhere)
	bool bRangedTargetModeForceRadial;

	// D11.DB - Enable the Autotesting system.
	UPROPERTY(Config, EditAnywhere)
	bool bEnabled;

	// D11.DB - Some attack ranges are increased slightly if the player is moving at the same time.
	UPROPERTY(Config, EditAnywhere)
	float RangeBoost;

	// D11.DB - Find all potential targets within the specified cone.
	void UpdateCone( ABasePlayerController& controller, float radius, float angle, float weight ); 

	// D11.DB - Is the provided mob valid for targeting.
	bool IsMobValid(AMobCharacter* mob, ABasePlayerController& controller);

	// D11.DB - Determines which targeting method to use.
	void GetTargetingData(const ABasePlayerController& controller, float& outAngle, float& outRadius, float& outWeight);

	// D11.DB - DEBUG - Debug draw an autotargeting radius.
	void DebugDrawCircle( const ABasePlayerController& controller, float radius );

	// D11.DB - DEBUG - Debug draw an autotargeting cone.
	void DebugDrawCone( const ABasePlayerController& controller, float radius, float angle );

	// D11.DB - The internal cache of targets.
	TArray<MobTarget> Targets;

	// D11.DB - The last mob successfully autotargeted.
	TWeakObjectPtr<AMobCharacter> PrevTarget;

	// D11.DB - The last mob successfully autotargeted.
	TWeakObjectPtr<AMobCharacter> LastHighlightedTarget;

	// D11.DB - Flags for ranged targeting mode.
	bool IsRangedTargeting = false;
	bool IsSuccessfullyRangedTargeting = false;

	// D11.DB - Flags for whether the player is trying to shoot
	bool bIsShooting = false;

	// D11.DB - Track mode changes.
	Mode mPrevMode;
	Mode mPrevModeCache;
	int modeTimer;
	float TimeOfCurrentModeTransition;
	float TimeOfPreviousModeTransition;
	float TimeInCurrentState;
	float TimeInPrevState;
	
	// D11.DB - Autotarget configuration.
	UPROPERTY()
	UAutoTargetDefaultIdle* ConfigDefaultIdle;
	UPROPERTY()
	UAutoTargetDefaultActive* ConfigDefaultActive;
	UPROPERTY()
	UAutoTargetRangedIdle* ConfigRangedIdle;
	UPROPERTY()
	UAutoTargetRangedActive* ConfigRangedActive;
	UPROPERTY()
	UAutoTargetShootingIdle* ConfigShootingIdle;
	UPROPERTY()
	UAutoTargetShootingActive* ConfigShootingActive;
};
