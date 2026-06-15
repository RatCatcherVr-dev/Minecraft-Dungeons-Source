#include "Dungeons.h"
#include "PlayerCoopCamera.h"
#include "DungeonsUserManagement.h"
#include "DungeonsGameInstance.h"
#include "LoadingScreen/LoadingScreenInitializer.h"

APlayerCoopCamera::APlayerCoopCamera(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickGroup = ETickingGroup::TG_DuringPhysics;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;

	CameraAim = CreateDefaultSubobject<USceneComponent>(TEXT("CameraAim"));
	CameraAim->SetMobility(EComponentMobility::Movable);
	SetRootComponent(CameraAim);

	AllowPopping = false;

	LovikaSpringArm = CreateDefaultSubobject<ULovikaSpringArmComponent>(TEXT("LovikaSpringArm"));
	LovikaSpringArm->AttachTo(CameraAim);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachTo(LovikaSpringArm);
}

void APlayerCoopCamera::BeginPlay()
{
	
	if (UDungeonsGameInstance* gameInstance = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (ULoadingScreenInitializer* pLoadScreen = gameInstance->GetLoadingScreenInitializer())
		{

			FScriptDelegate BlockPoppingDelegate;
			BlockPoppingDelegate.BindUFunction(this, "BlockPopping");
			pLoadScreen->LoadingScreenStarted.AddUnique(BlockPoppingDelegate);

			FScriptDelegate UnblockPoppingDelegate;
			UnblockPoppingDelegate.BindUFunction(this, "UnblockPopping");
			pLoadScreen->LoadingScreenTornDown.AddUnique(UnblockPoppingDelegate);
		}
	}
	
	Super::BeginPlay();

	InstanceTracker< APlayerCoopCamera >::AddInstance(GetWorld(), this);
}

void APlayerCoopCamera::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UDungeonsGameInstance* gameInstance = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (ULoadingScreenInitializer* pLoadScreen = gameInstance->GetLoadingScreenInitializer())
		{
			pLoadScreen->LoadingScreenStarted.RemoveAll(this);
			pLoadScreen->LoadingScreenTornDown.RemoveAll(this);
		}
	}

	Super::EndPlay(EndPlayReason);

	InstanceTracker< APlayerCoopCamera >::RemoveInstance(GetWorld(), this);
}

void APlayerCoopCamera::Tick(float DeltaSeconds)
{

	if (UDungeonsGameInstance* gameInstance = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance()))
	{
		FVector PosOut = FVector::ZeroVector; 
	
		//Constant values from Original BP Node
		const float fStartZoomRadius = 300.0f * MaximumZoomMultiplier;
		const float fEndZoomRadius = 800.0f * MaximumZoomMultiplier;
		const float fZoomMaximum = 550.0f;
	
		gameInstance->UpdateLocalCoopCamera(fStartZoomRadius,fEndZoomRadius,fZoomMaximum, ZoomAmount,PosOut);
	
		LovikaSpringArm->SetDesiredArmLegnth(LovikaSpringArm->SeekArmLength * MaximumZoomMultiplier + ZoomAmount);
	
		FVector CamWorldLoc = CameraAim->GetComponentLocation();
		FVector DiffPos = CamWorldLoc - PosOut;
	
		if (DiffPos.SizeSquared() < (5000.0f*5000.0f)) //const value from BP
		{
			PosOut = FMath::VInterpTo(CamWorldLoc, PosOut, DeltaSeconds, 5.0f); //const value from BP
		}
	
		CameraAim->SetWorldLocation(PosOut);
	
		if (AllowPopping) {
			gameInstance->UpdateLocalCoopScreenStates();
			gameInstance->HandlePlayerPopping(DeltaSeconds);
		}
	}

	Super::Tick(DeltaSeconds);
}

void APlayerCoopCamera::BecomeViewTarget(class APlayerController* PC)
{
	IsViewTarget = true;

	UnblockPopping();

	Super::BecomeViewTarget(PC);
}

void APlayerCoopCamera::EndViewTarget(class APlayerController* PC)
{
	if (auto* gi = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (auto* pPlayerController = Cast<APlayerControllerBase>(gi->GetUserManager()->GetInitialPlayerController()))
		{
			pPlayerController->ClearAudioListenerOverride();
		}
	}

	IsViewTarget = false;

	BlockPopping(0);

	Super::EndViewTarget(PC);
}

void APlayerCoopCamera::BlockPopping(float val)
{
	AllowPopping = false;
}

void APlayerCoopCamera::UnblockPopping()
{
	AllowPopping = true;
}
