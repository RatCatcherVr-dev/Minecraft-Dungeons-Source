#include "Dungeons.h"
#include "AutoTarget.h"

#include "game/actor/character/player/BasePlayerController.h"
#include "util/CharacterQuery.h"
#include "world/entity/MobTags.h"
#include "GameSettingsFunctionLibrary.h"
#include "DungeonsGameInstance.h"
#include "DungeonsUserManagement.h"
#include "game/team/TeamQuery.h"

DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_Update"),STAT_AAutoTarget_Update, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_Update_GetTargetingData"),STAT_AAutoTarget_Update_GetTargetingData, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_Update_UpdateCone"),STAT_AAutoTarget_Update_UpdateCone, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_Update_OnHighlightTargetChanged"),STAT_AAutoTarget_Update_OnHighlightTargetChanged, STATGROUP_PlayerController);

DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_UpdateCone_MobList"), STAT_AAutoTarget_UpdateCone_MobList, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_UpdateCone_SortList"), STAT_AAutoTarget_UpdateCone_SortList, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_UpdateCone_ComputeHeuristics"), STAT_AAutoTarget_UpdateCone_ComputeHeuristics, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_UpdateCone_"), STAT_AAutoTarget_UpdateCone_, STATGROUP_PlayerController);

DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_UpdateCone_coneCheck"), STAT_AAutoTarget_UpdateCone_coneCheck, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_UpdateCone_IsMobValid"), STAT_AAutoTarget_UpdateCone_IsMobValid, STATGROUP_PlayerController);

DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_UpdateMobsOnScreen"), STAT_AAutoTarget_UpdateMobsOnScreen, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_UpdateMobsOnScreen_"), STAT_AAutoTarget_UpdateMobsOnScreen_, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_UpdateMobsOnScreen_ValidityCheck"), STAT_AAutoTarget_UpdateMobsOnScreen_ValidityCheck, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_AAutoTarget_UpdateMobsOnScreen_ProjectWorld"), STAT_AAutoTarget_UpdateMobsOnScreen_ProjectWorld, STATGROUP_PlayerController);


// D11.DB - Refactored config variables.
static TAutoConsoleVariable<float> CVarATDefaultIdleAngle(TEXT("Dungeons.AutoTarget.Mode.DefaultIdle.Angle"), 60.0f, TEXT(""), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarATDefaultIdleRadius(TEXT("Dungeons.AutoTarget.Mode.DefaultIdle.Radius"), 60.0f, TEXT(""), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarATDefaultIdleWeight(TEXT("Dungeons.AutoTarget.Mode.DefaultIdle.Weight"), 60.0f, TEXT(""), ECVF_Cheat);

static TAutoConsoleVariable<float> CVarATDefaultActiveAngle(TEXT("Dungeons.AutoTarget.Mode.DefaultActive.Angle"), 60.0f, TEXT(""), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarATDefaultActiveRadius(TEXT("Dungeons.AutoTarget.Mode.DefaultActive.Radius"), 60.0f, TEXT(""), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarATDefaultActiveWeight(TEXT("Dungeons.AutoTarget.Mode.DefaultActive.Weight"), 60.0f, TEXT(""), ECVF_Cheat);

static TAutoConsoleVariable<float> CVarATRangedIdleAngle(TEXT("Dungeons.AutoTarget.Mode.RangedIdle.Angle"), 60.0f, TEXT(""), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarATRangedIdleRadius(TEXT("Dungeons.AutoTarget.Mode.RangedIdle.Radius"), 60.0f, TEXT(""), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarATRangedIdleWeight(TEXT("Dungeons.AutoTarget.Mode.RangedIdle.Weight"), 60.0f, TEXT(""), ECVF_Cheat);

static TAutoConsoleVariable<float> CVarATRangedActiveAngle(TEXT("Dungeons.AutoTarget.Mode.RangedActive.Angle"), 60.0f, TEXT(""), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarATRangedActiveRadius(TEXT("Dungeons.AutoTarget.Mode.RangedActive.Radius"), 60.0f, TEXT(""), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarATRangedActiveWeight(TEXT("Dungeons.AutoTarget.Mode.RangedActive.Weight"), 60.0f, TEXT(""), ECVF_Cheat);

static TAutoConsoleVariable<float> CVarATShootingIdleAngle(TEXT("Dungeons.AutoTarget.Mode.ShootingIdle.Angle"), 60.0f, TEXT(""), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarATShootingIdleRadius(TEXT("Dungeons.AutoTarget.Mode.ShootingIdle.Radius"), 60.0f, TEXT(""), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarATShootingIdleWeight(TEXT("Dungeons.AutoTarget.Mode.ShootingIdle.Weight"), 60.0f, TEXT(""), ECVF_Cheat);

static TAutoConsoleVariable<float> CVarATShootingActiveAngle(TEXT("Dungeons.AutoTarget.Mode.ShootingActive.Angle"), 60.0f, TEXT(""), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarATShootingActiveRadius(TEXT("Dungeons.AutoTarget.Mode.ShootingActive.Radius"), 60.0f, TEXT(""), ECVF_Cheat);
static TAutoConsoleVariable<float> CVarATShootingActiveWeight(TEXT("Dungeons.AutoTarget.Mode.ShootingActive.Weight"), 60.0f, TEXT(""), ECVF_Cheat);

// D11.DB - START - Console Variables
static TAutoConsoleVariable<float> CVarDebugAutoTestRangeBoost(
	TEXT("Dungeons.AutoTarget.RangeBoost"),
	100.0f,
	TEXT("How long to maintain the last highlighted target after losing it.\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<int32> CVarDebugAutoTestToggleDebugDraw(
	TEXT("Dungeons.AutoTarget.DebugDraw"),
	0,
	TEXT("Enables debug drawing of the autotargeting.\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<int32> CVarDebugAutoTestToggleDebugDrawVerbose(
	TEXT("Dungeons.AutoTarget.DebugDrawVerbose"),
	0,
	TEXT("Enable verbose debug text"),
	ECVF_Cheat);

static TAutoConsoleVariable<int32> CVarDebugAutoTestLineEnable(
	TEXT("Dungeons.AutoTarget.LineEnable"),
	0,
	TEXT("Enables the line part of the autotesting cone.\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<int32> CVarDebugAutoTestHighlightEnable(
	TEXT("Dungeons.AutoTarget.HighlightEnable"),
	0,
	TEXT("Enable/Disable highlighting of autotargets.\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<int32> CVarDebugAutoTestRangedTargetMode(
	TEXT("Dungeons.AutoTarget.RangedTargetMode"),
	0,
	TEXT("Enable/Disable the ranged target mode functionality.\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<int32> CVarDebugAutoTestRangedTargetModeForceRadial(
	TEXT("Dungeons.AutoTarget.RangedTargetModeForceRadial"),
	0,
	TEXT("Enable/Disable the ranged target mode's force radial functionality.\n"),
	ECVF_Cheat);

static TAutoConsoleVariable<int32> CVarDebugAutoTestEnable(
	TEXT("Dungeons.AutoTarget.Enable"),
	0,
	TEXT("Enable/Disable the entire autotargeting system.\n"),
	ECVF_Cheat);

namespace {
// D11.DB - Compute vector rejection
float VectorRejection(const FVector& a, const FVector& b)
{
	const float adb = FVector2D::DotProduct(FVector2D(a.X, a.Y), FVector2D(b.X, b.Y));
	const float bdb = FVector2D::DotProduct(FVector2D(b.X, b.Y), FVector2D(b.X, b.Y));
	const auto a2 = a - ((adb / bdb) * b);
	return a2.Size();
}

void ComputeHeuristics(const FVector& mobOffset, const float playerForwardAngle, const float coneAngle, const float coneRadius, float& distanceValue, float& angleValue)
{
	SCOPE_CYCLE_COUNTER(STAT_AAutoTarget_UpdateCone_ComputeHeuristics);

	//D11.SC I think this should be ok as this is used a weighted value rather than an actual distance, if we see targetting anomalies, its probably this :/
	distanceValue = (mobOffset.SizeSquared() / (coneRadius * coneRadius));
	
	auto mobOffsetAngle = FMath::Atan2(mobOffset.Y, mobOffset.X);

	auto angleDifference = FMath::FindDeltaAngleRadians(mobOffsetAngle, playerForwardAngle);
	angleValue = FMath::Abs(angleDifference / FMath::DegreesToRadians(coneAngle * 0.5f));
};

AAutoTarget::MobTarget CalculateTargetOrder(AMobCharacter& mob, const FVector& mobOffset, const float playerForwardAngle, float coneAngle, float coneRadius, float weight) {
	AAutoTarget::MobTarget target;
	target.Mob = &mob;

	float distanceValue, angleValue;
	ComputeHeuristics(mobOffset, playerForwardAngle, coneAngle, coneRadius, distanceValue, angleValue);
	target.Order = FMath::Lerp(distanceValue, angleValue, weight);

	if(!teamquery::is::hostile(ETeamName::Heroes, mob.GetCurrentTeam())) {
		target.Order += 100000;
	}


	if (hasMobTag(mob.EntityType, MobTags::HashTag_Passive)) {
		// Animals should always be targeted at a lower priority
		target.Order += 1000000;
	}

	if (hasMobTag(mob.EntityType, MobTags::HashTag_LowPriority)) {
		// D11.DB - Added a special tag allowing specific mob types to
		//			be assigned a reduced autotargetting priority.
		target.Order += 10000;
	}

	return target;
}
}

void AAutoTarget::BeginPlay()
{
	Super::BeginPlay();

	ConfigDefaultIdle = NewObject<UAutoTargetDefaultIdle>(this);
	ConfigDefaultActive = NewObject<UAutoTargetDefaultActive>(this);
	ConfigRangedIdle = NewObject<UAutoTargetRangedIdle>(this);
	ConfigRangedActive = NewObject<UAutoTargetRangedActive>(this);
	ConfigShootingIdle = NewObject<UAutoTargetShootingIdle>(this);
	ConfigShootingActive = NewObject<UAutoTargetShootingActive>(this);

	CVarATDefaultIdleAngle.AsVariable()->Set(ConfigDefaultIdle->Angle);
	CVarATDefaultIdleRadius.AsVariable()->Set(ConfigDefaultIdle->Radius);
	CVarATDefaultIdleWeight.AsVariable()->Set(ConfigDefaultIdle->Weight);

	CVarATDefaultActiveAngle.AsVariable()->Set(ConfigDefaultActive->Angle);
	CVarATDefaultActiveRadius.AsVariable()->Set(ConfigDefaultActive->Radius);
	CVarATDefaultActiveWeight.AsVariable()->Set(ConfigDefaultActive->Weight);

	CVarATRangedIdleAngle.AsVariable()->Set(ConfigRangedIdle->Angle);
	CVarATRangedIdleRadius.AsVariable()->Set(ConfigRangedIdle->Radius);
	CVarATRangedIdleWeight.AsVariable()->Set(ConfigRangedIdle->Weight);

	CVarATRangedActiveAngle.AsVariable()->Set(ConfigRangedActive->Angle);
	CVarATRangedActiveRadius.AsVariable()->Set(ConfigRangedActive->Radius);
	CVarATRangedActiveWeight.AsVariable()->Set(ConfigRangedActive->Weight);

	CVarATShootingIdleAngle.AsVariable()->Set(ConfigShootingIdle->Angle);
	CVarATShootingIdleRadius.AsVariable()->Set(ConfigShootingIdle->Radius);
	CVarATShootingIdleWeight.AsVariable()->Set(ConfigShootingIdle->Weight);

	CVarATShootingActiveAngle.AsVariable()->Set(ConfigShootingActive->Angle);
	CVarATShootingActiveRadius.AsVariable()->Set(ConfigShootingActive->Radius);
	CVarATShootingActiveWeight.AsVariable()->Set(ConfigShootingActive->Weight);

	CVarDebugAutoTestRangeBoost.AsVariable()->Set(RangeBoost);
	CVarDebugAutoTestToggleDebugDraw.AsVariable()->Set(bDebugDraw);
	CVarDebugAutoTestToggleDebugDrawVerbose.AsVariable()->Set(bDebugDrawVerbose);
	CVarDebugAutoTestLineEnable.AsVariable()->Set(bLineEnabled);
	CVarDebugAutoTestHighlightEnable.AsVariable()->Set(bHighlightEnabled);
	CVarDebugAutoTestRangedTargetMode.AsVariable()->Set(bRangedTargetModeEnabled);
	CVarDebugAutoTestRangedTargetModeForceRadial.AsVariable()->Set(bRangedTargetModeForceRadial);
	CVarDebugAutoTestEnable.AsVariable()->Set(bEnabled);

	mPrevMode = Mode::Default;
	mPrevModeCache = Mode::Default;
}

// D11.DB - END - Console Variables

int AAutoTarget::Update(ABasePlayerController& controller)
{

	SCOPE_CYCLE_COUNTER(STAT_AAutoTarget_Update);

	// D11.DB - Lazy init. Set the initial value of the console variables to
	//			match the config file. We only ever want to do this once.

#if !( UE_BUILD_SHIPPING || UE_BUILD_TEST)
	// D11.DB - Make sure we use the latest values from the debug console.
	ConfigDefaultIdle->Angle = CVarATDefaultIdleAngle.GetValueOnGameThread();
	ConfigDefaultIdle->Radius = CVarATDefaultIdleRadius.GetValueOnGameThread();
	ConfigDefaultIdle->Weight = CVarATDefaultIdleWeight.GetValueOnGameThread();
	ConfigDefaultActive->Angle = CVarATDefaultActiveAngle.GetValueOnGameThread();
	ConfigDefaultActive->Radius = CVarATDefaultActiveRadius.GetValueOnGameThread();
	ConfigDefaultActive->Weight = CVarATDefaultActiveWeight.GetValueOnGameThread();
	ConfigRangedIdle->Angle = CVarATRangedIdleAngle.GetValueOnGameThread();
	ConfigRangedIdle->Radius = CVarATRangedIdleRadius.GetValueOnGameThread();
	ConfigRangedIdle->Weight = CVarATRangedIdleWeight.GetValueOnGameThread();
	ConfigRangedActive->Angle = CVarATRangedActiveAngle.GetValueOnGameThread();
	ConfigRangedActive->Radius = CVarATRangedActiveRadius.GetValueOnGameThread();
	ConfigRangedActive->Weight = CVarATRangedActiveWeight.GetValueOnGameThread();
	ConfigShootingIdle->Angle = CVarATShootingIdleAngle.GetValueOnGameThread();
	ConfigShootingIdle->Radius = CVarATShootingIdleRadius.GetValueOnGameThread();
	ConfigShootingIdle->Weight = CVarATShootingIdleWeight.GetValueOnGameThread();
	ConfigShootingActive->Angle = CVarATShootingActiveAngle.GetValueOnGameThread();
	ConfigShootingActive->Radius = CVarATShootingActiveRadius.GetValueOnGameThread();
	ConfigShootingActive->Weight = CVarATShootingActiveWeight.GetValueOnGameThread();

	RangeBoost = CVarDebugAutoTestRangeBoost.GetValueOnGameThread();
	bDebugDraw = CVarDebugAutoTestToggleDebugDraw.GetValueOnGameThread() != 0;
	bDebugDrawVerbose = CVarDebugAutoTestToggleDebugDrawVerbose.GetValueOnGameThread() != 0;
	bLineEnabled = CVarDebugAutoTestLineEnable.GetValueOnGameThread() != 0;
	bHighlightEnabled = CVarDebugAutoTestHighlightEnable.GetValueOnGameThread() != 0;
	bRangedTargetModeEnabled = CVarDebugAutoTestRangedTargetMode.GetValueOnGameThread() != 0;
	bRangedTargetModeForceRadial = CVarDebugAutoTestRangedTargetModeForceRadial.GetValueOnGameThread() != 0;
	bEnabled = CVarDebugAutoTestEnable.GetValueOnGameThread() != 0;
#endif

	// D11.DB - If the autotest system has been disabled then do nothing.
	// D11.SSN - only disable auto targeting when shooting.
	if (!bEnabled && bIsShooting)
	{
		if( Targets.Num() > 0 )
		{
			Targets.Reset();
		}

		return 0;
	}
	if (mPrevMode != GetMode())
	{
		TimeOfPreviousModeTransition = TimeOfCurrentModeTransition;
		TimeOfCurrentModeTransition = controller.GetWorld()->GetTimeSeconds();
		mPrevModeCache = mPrevMode;
	}
	
	auto ThisWorld = controller.GetWorld();


	TimeInPrevState = ThisWorld->GetTimeSeconds() - TimeOfPreviousModeTransition;
	TimeInCurrentState = ThisWorld->GetTimeSeconds() - TimeOfCurrentModeTransition;

	TWeakObjectPtr<AMobCharacter> prevTargetCache = PrevTarget;

	float angle = 0.0f;
	float radius = 0.0f;
	float weight = 0.0f;

	{
		SCOPE_CYCLE_COUNTER(STAT_AAutoTarget_Update_GetTargetingData);
		GetTargetingData(controller, angle, radius, weight);
	}

	// D11.DB - We're going to rebuild this so clear it out.
	Targets.Reset();

	SCOPE_CYCLE_COUNTER(STAT_AAutoTarget_Update_UpdateCone);
	UpdateCone(controller, radius, angle, weight);


	if (Targets.Num() == 0)
	{
		PrevTarget.Reset();
	}
	else
	{
		PrevTarget = Targets[0].Mob;
	}

	// D11.DB - If we still have no targets, disable ranged target mode.
	if (Targets.Num() == 0)
	{
		//controller.GetControlledPlayerCharacter().getar
		IsRangedTargeting = false;
		IsSuccessfullyRangedTargeting = false;
	}

	TWeakObjectPtr< AActor > newTarget = GetTarget();
	AActor* oldTarget = nullptr;
	if (prevTargetCache.IsValid())
	{
		oldTarget = prevTargetCache.Get();
	}
	if (newTarget.Get() != oldTarget)
	{
		SCOPE_CYCLE_COUNTER(STAT_AAutoTarget_Update_OnHighlightTargetChanged);
		controller.OnHighlightTargetChanged(newTarget.Get(), oldTarget);
	}
	
	IsSuccessfullyRangedTargeting = IsRangedTargeting;

	mPrevMode = GetMode();

	DebugDraw(controller);

	// D11.DB - This could be handy.
	return Targets.Num();
}

void AAutoTarget::UpdateHighlight(ABasePlayerController& controller)
{
	if (!bHighlightEnabled || !controller.GetGamepadActive()) {
		return;
	}

	auto prev = LastHighlightedTarget.IsValid() ? LastHighlightedTarget.Get() : nullptr;
	auto curr = Targets.Num() == 0 ? nullptr : Targets[0].Mob.Get();

	if (AHighlightController* highlightController = AHighlightController::CheckHighlightControllerExists(controller.GetWorld())){
		// D11.DB - Make sure we clear the highlight from dead enemies.
		if (!IsMobValid(prev, controller)) {
			highlightController->EndOutlineActor(prev);
			prev = nullptr;
		}

		// D11.DB - Change highlighted target if required.
		if (prev != curr) {
			if (prev) {
				prev->cachedTargetees.Remove(controller.GetControlledPlayerCharacter());
				highlightController->EndOutlineActor(prev);

				if (prev->cachedTargetees.Num() > 0)
				{
					for (auto& targeter : prev->cachedTargetees)
					{
						if (targeter.IsValid() && !targeter.IsStale())
						{
							if (auto targeterController = Cast<ABasePlayerController>(targeter->GetPlayerController()))
							{
								//Quick reset on the outline.
								highlightController->EndOutlineActor(prev);
								highlightController->BeginOutlineActor(targeter.Get(), prev);
								break;
							}
						}
					}
				}

				LastHighlightedTarget.Reset();
			}
			if (curr) {
				curr->cachedTargetees.Add(controller.GetControlledPlayerCharacter());
				highlightController->BeginOutlineActor(controller.GetControlledPlayerCharacter(), curr);
				LastHighlightedTarget = curr;
			}
		}
	}
}

void AAutoTarget::ClearHighlight(ABasePlayerController& controller)
{
	if (LastHighlightedTarget.IsValid())
	{
		if (AHighlightController* highlightController = AHighlightController::CheckHighlightControllerExists(controller.GetWorld())){
			highlightController->EndOutlineActor(LastHighlightedTarget.Get());
		}
		LastHighlightedTarget->cachedTargetees.Remove(controller.GetControlledPlayerCharacter());
		LastHighlightedTarget.Reset();
	}
}

void AAutoTarget::Clear(ABasePlayerController& controller)
{
	ClearHighlight(controller);
	Targets.Reset();
	LastHighlightedTarget.Reset();
}

void AAutoTarget::ToggleRangedTarget()
{
	if (bRangedTargetModeEnabled)
	{
		IsRangedTargeting = !IsRangedTargeting;
	}
	else
	{
		IsRangedTargeting = false;
	}
}

AAutoTarget::Mode AAutoTarget::GetMode()
{
	if (IsRangedTargeting) {
		return Mode::Ranged;
	}
	else if (bIsShooting) {
		return Mode::Shooting;
	}
	else {
		return Mode::Default;
	}
}

void AAutoTarget::ForceTarget(AMobCharacter* mob)
{
	if (Targets.Num() == 0)
	{
		Targets.Add({ mob, 0 });
	}
	else
	{
		Targets[0] = { mob, 0 };
	}
}

void AAutoTarget::UpdateOnScreenMobs()
{
	// D11.DB - Find all viable Mobs.
	auto& MobList = InstanceTracker< AMobCharacter >::GetList(GetWorld());

	UDungeonsGameInstance* gi = Cast<UDungeonsGameInstance>(GetGameInstance());

	ABasePlayerController* initialController = Cast<ABasePlayerController>(gi->GetUserManager()->GetInitialPlayerController());

	SCOPE_CYCLE_COUNTER(STAT_AAutoTarget_UpdateMobsOnScreen);
	{
		gi->OnScreenMobs.Reset();
		gi->OnScreenMobs.Reserve(MobList.Num());
		gi->MobsInOuterScreenPadding.Reset();
		gi->MobsInOuterScreenPadding.Reserve(MobList.Num());

		if (initialController->GetControlledPlayerCharacter() == nullptr) {
			return;
		}

		FVector2D mobPosOnScreen;

		int viewportX, viewportY;
		initialController->GetViewportSize(viewportX, viewportY);

		const auto CurrMode = GetMode();

		const FVector2D extraBoundary = FVector2D(viewportX, viewportY) * 0.1f;

		const FBox2D screenRect(FVector2D::ZeroVector, FVector2D(viewportX, viewportY));
		const FBox2D outerScreenPaddingRect(-extraBoundary, FVector2D(viewportX, viewportY) + extraBoundary);
		
		int checkedMobs = 0;
		for (auto* mob : MobList)
		{
			// D11.DH
			// Check if the mob is actually within range of the player before doing the more costly checks in the loop
			// Reduced the average for mob checks from 127 to 10 in pumpkinpastures 3 6 0
			// average saving of around 500 microseconds with 3 ms taken off of the max time taken in the profiler
			if (mob->mPlayerVisible)
			{

				++checkedMobs;
				SCOPE_CYCLE_COUNTER(STAT_AAutoTarget_UpdateMobsOnScreen_ValidityCheck);
				if (IsMobValid(mob, *initialController))
				{
					SCOPE_CYCLE_COUNTER(STAT_AAutoTarget_UpdateMobsOnScreen_ProjectWorld);
					
					if(mob->WasRecentlyRendered(0.5f))
					{
						SCOPE_CYCLE_COUNTER(STAT_AAutoTarget_UpdateMobsOnScreen_);
							
						if(mob->WasRecentlyRendered(0.2f))
						{
							gi->OnScreenMobs.Add(mob);
						}
						else
						{
							gi->MobsInOuterScreenPadding.Add(mob);
						}
					}
				}
			}
			
		}
		
	}
}


void AAutoTarget::UpdateCone(ABasePlayerController& controller, float radius, float angle, float weight)
{
	SCOPE_CYCLE_COUNTER(STAT_AAutoTarget_UpdateCone_);

	UDungeonsGameInstance* gi = Cast<UDungeonsGameInstance>(GetGameInstance());

	const auto PlayerCharActor = controller.GetControlledPlayerCharacter();
	const float radiusSqared = radius * radius;
	const auto playerRotation = PlayerCharActor->GetActorRotation();
	const auto playerQuat = playerRotation.Quaternion();

	FVector playerForward;
	if (FMath::IsNearlyZero(controller.mLastMoveX) && FMath::IsNearlyZero(controller.mLastMoveY))
	{
		playerForward = playerQuat.GetForwardVector();
	}
	else
	{
		playerForward = controller.ProjectInputAxesFromPlayer(FVector(controller.mLastMoveX, controller.mLastMoveY, 0.0f), 1.0f) - PlayerCharActor->GetActorLocation();
	}

	const auto playerForwardAngle = FMath::Atan2(playerForward.Y, playerForward.X);
	const auto playerLocation = PlayerCharActor->GetActorLocation();

	const auto playerConeLeft = playerForward.RotateAngleAxis(angle * 0.5f, FVector(0.0f, 0.0f, 1.0f));
	const auto playerConeRight = playerForward.RotateAngleAxis(angle * -0.5f, FVector(0.0f, 0.0f, 1.0f));

	const bool activeShooting = GetMode() == Mode::Shooting && (FMath::Abs(controller.mLastMoveX) > 0.05f || FMath::Abs(controller.mLastMoveY) > 0.05);
	const int reserveCount = activeShooting ? gi->OnScreenMobs.Num() + gi->MobsInOuterScreenPadding.Num() : gi->OnScreenMobs.Num();

	Targets.Reserve(reserveCount);
	{
		SCOPE_CYCLE_COUNTER(STAT_AAutoTarget_UpdateCone_MobList);

		auto IsInCone = [&](const TWeakObjectPtr<AMobCharacter>& mob)
		{
			if (mob.IsValid() && mob->IsTargetable())
			{
				const auto locationDelta = mob->GetActorLocation() - playerLocation;

				// D11.DB - Determine if the mob is inside the cone via 2D cross products.
				const auto vecX = FVector2D(locationDelta.X, locationDelta.Y);
				const auto vecA = FVector2D(playerConeLeft.X, playerConeLeft.Y);
				const auto vecB = FVector2D(playerConeRight.X, playerConeRight.Y);
				const float c1 = FVector2D::CrossProduct(vecA, vecX);
				const float c2 = FVector2D::CrossProduct(vecX, vecB);
				const float c3 = FVector2D::CrossProduct(vecA, vecB);

				bool cone = false;
				if (angle > 180.0f)
				{
					cone = !(c1 > 0.0f && c2 > 0.0f && c3 > 0.0f);
				}
				else
				{
					cone = c1 < 0.0f && c2 < 0.0f && c3 < 0.0f;
				}

				// D11.DB - Target is inside the code, inside the radius and is still alive.
				if (cone && locationDelta.SizeSquared() < radiusSqared)
				{
					AAutoTarget::MobTarget TargetMob = CalculateTargetOrder(*mob, locationDelta, playerForwardAngle, angle, radius, weight);
					const int lastIndex = Targets.IndexOfByPredicate(RETLAMBDA(it.Order > TargetMob.Order));
					if(lastIndex == INDEX_NONE)
					{
						Targets.Push(TargetMob);
					}
					else
					{
						Targets.Insert(TargetMob, lastIndex);
					}
				}
				
			}
		};

		for (auto mob : gi->OnScreenMobs)
		{
			IsInCone(mob);
		}

		if (activeShooting)
		{
			for (auto mob : gi->MobsInOuterScreenPadding)
			{
				IsInCone(mob);
			}
		}
		
	}
}


bool AAutoTarget::IsMobValid(AMobCharacter* mob, ABasePlayerController& controller)
{
	auto playerCharacter = controller.GetControlledPlayerCharacter();

	if( !mob || !playerCharacter )
		return false;

	// auto hostile = characterquery::is::hostile( controller.GetControlledPlayerCharacter() );
	bool isAnimal = hasMobTag(mob->EntityType, MobTags::HashTag_Passive);
	bool isTargetDummy = mob->EntityType == EntityType::Dummy;
	bool isKey = mob->GetCurrentTeam() == ETeamName::Keys;
	bool validMob = (isAnimal || characterquery::is::hostile(playerCharacter, mob) || isTargetDummy || (isKey && !bIsShooting)) && characterquery::is::targetable(mob) && actorquery::is::alive(mob);

	if (IsRangedTargeting) {
		validMob = validMob && hasMobTag(mob->EntityType, MobTags::HashTag_Ranged);
	}
	return validMob;
}

bool AAutoTarget::IsTargetingStickActive(const ABasePlayerController& controller) const {
	return FMath::Abs(controller.mLastMoveX) > 0.05f || FMath::Abs(controller.mLastMoveY) > 0.05f;
}

void AAutoTarget::GetTargetingData(const ABasePlayerController& controller, float& outAngle, float& outRadius, float& outWeight)
{
	bool active = IsTargetingStickActive(controller);

	// D11.DB ...There is probably a nicer way of doing this.

	const float localCoopRangedMultiplier = GetGameInstance<UDungeonsGameInstance>()->IsLocalCoop()  + 1;

	const auto CurrMode = GetMode();
	const float minimumDefaultTime = 0.2f;
	const bool holdShooting = mPrevModeCache == Mode::Shooting && CurrMode == Mode::Default && TimeInCurrentState < minimumDefaultTime && active;

	if (IsRangedTargeting)
	{
		if (active)
		{
			outAngle = ConfigRangedActive->Angle;
			outRadius = ConfigRangedActive->Radius * localCoopRangedMultiplier;
			outWeight = ConfigRangedActive->Weight;
		}
		else 
		{
			outAngle = ConfigRangedIdle->Angle;
			outRadius = ConfigRangedIdle->Radius * localCoopRangedMultiplier;
			outWeight = ConfigRangedIdle->Weight;
		}
	}
	else
	{
		if (bIsShooting || holdShooting)
		{
			if (active)
			{
				outAngle = ConfigShootingActive->Angle;
				outRadius = ConfigShootingActive->Radius * localCoopRangedMultiplier;
				outWeight = ConfigShootingActive->Weight;
			}
			else
			{
				outAngle = ConfigShootingIdle->Angle;
				outRadius = ConfigShootingIdle->Radius * localCoopRangedMultiplier;
				outWeight = ConfigShootingIdle->Weight;
			}
		}
		else
		{
			if (active)
			{
				outAngle = ConfigDefaultActive->Angle;
				outRadius = ConfigDefaultActive->Radius * localCoopRangedMultiplier;
				outWeight = ConfigDefaultActive->Weight;
			}
			else
			{
				outAngle = ConfigDefaultIdle->Angle;
				outRadius = ConfigDefaultIdle->Radius * localCoopRangedMultiplier;
				outWeight = ConfigDefaultIdle->Weight;
			}
		}
	}
}

void AAutoTarget::DebugDraw(const ABasePlayerController& controller)
{
	if (!bDebugDraw)
	{
		return;
	}

	float angle, radius, weight;
	GetTargetingData(controller, angle, radius, weight);

	DebugDrawCone(controller, radius, angle);

	// D11.DB - We can also draw a bit of text on top of each enemy.
	FlushDebugStrings(controller.GetWorld());

	enum class StringMode { Priority, Weights };
	auto mode = bDebugDrawVerbose ? StringMode::Weights : StringMode::Priority;

	const auto playerlocation = controller.GetControlledPlayerCharacter()->GetActorLocation();
	const auto playerrot = controller.GetControlledPlayerCharacter()->GetActorRotation();
	const auto playerForward = playerrot.Quaternion().GetForwardVector();
	const auto playerForwardAngle = FMath::Atan2(playerForward.Y, playerForward.X);

	int counter = 1;
	for (auto&& targetOrder : Targets)
	{
		auto mob = targetOrder.Mob;
		switch (mode)
		{
			case StringMode::Weights:
			{
				const auto loc = mob->GetActorLocation();

				float distanceValue, angleValue;
				auto mobOffset = loc - playerlocation;
				ComputeHeuristics(mobOffset, playerForwardAngle, angle, radius, distanceValue, angleValue);

				float AH = FMath::Lerp(distanceValue, angleValue, weight);

				FString S1;
				S1 += FString("A: ") + FString::SanitizeFloat(distanceValue) + FString("\n");
				S1 += FString("B: ") + FString::SanitizeFloat(angleValue) + FString("\n");
				S1 += FString("X: ") + FString::SanitizeFloat(AH) + FString("\n");
				S1 += FString("W: ") + FString::SanitizeFloat(weight) + FString("\n");
				S1 += FString("I: ") + FString::FromInt(counter++) + FString("\n");
				DrawDebugString(controller.GetWorld(), loc, S1);
			}
			break;

			case StringMode::Priority:
			{
				auto loc = mob->GetActorLocation();
				DrawDebugString(controller.GetWorld(), loc, FString::FromInt(counter++));
			}
			break;
		}
	}
}

void AAutoTarget::DebugDrawCircle(const ABasePlayerController& controller, float radius)
{
	const auto playerloc = controller.GetControlledPlayerCharacter()->GetActorLocation();
	int steps = 20;
	float angle = 0.0f;
	float angleinc = (PI * 2.0f) / (float)steps;
	FOccluderVertexArray verts;
	TArray<int32> indices;
	for (int i = 0; i < steps; ++i)
	{
		verts.Add(playerloc);
		verts.Add(playerloc + FVector(cos(angle) * radius, sin(angle) * radius, 0.0f));
		angle += angleinc;
		verts.Add(playerloc + FVector(cos(angle) * radius, sin(angle) * radius, 0.0f));
		indices.Add(i * 3);
		indices.Add(i * 3 + 1);
		indices.Add(i * 3 + 2);
	}
	DrawDebugMesh(controller.GetWorld(), verts, indices, FColor(255, 0, 0, 58));

}

void AAutoTarget::DebugDrawCone(const ABasePlayerController& controller, float radius, float angle)
{
	const auto playerloc = controller.GetControlledPlayerCharacter()->GetActorLocation();
	const auto playerrot = controller.GetControlledPlayerCharacter()->GetActorRotation();
	const auto playerforward = playerrot.Quaternion().GetForwardVector();
	const auto playerleft = playerforward.RotateAngleAxis(angle * 0.5f, FVector(0.0f, 0.0f, 1.0f));
	const auto playerright = playerforward.RotateAngleAxis(angle * -0.5f, FVector(0.0f, 0.0f, 1.0f));

	const int resolution = 20;

	const float angleMin = (angle * -0.5f) + 360.0f;
	const float angleMax = (angle * 0.5f) + 360.0f;
	float currentAngle = angleMin;
	const float delta = (angleMax - angleMin) / resolution;

	FOccluderVertexArray verts;
	TArray<int32> indices;

	for (int i = 0; i < resolution; ++i)
	{
		auto vec1 = playerloc + playerforward.RotateAngleAxis(currentAngle, FVector(0.0f, 0.0f, 1.0f)) * radius;
		auto vec2 = playerloc + playerforward.RotateAngleAxis(currentAngle + delta, FVector(0.0f, 0.0f, 1.0f)) * radius;
		verts.Add(playerloc);
		verts.Add(vec1);
		verts.Add(vec2);
		currentAngle += delta;
	}

	for (int i = 0; i < verts.Num(); ++i)
	{
		indices.Add(i);
	}

	DrawDebugMesh(controller.GetWorld(), verts, indices, FColor(255, 0, 0, 58));
	DrawDebugLine(controller.GetWorld(), playerloc, playerloc + (playerforward * radius), FColor(255, 0, 0, 255), false, -1.0f, 0, 10.0f);

}
