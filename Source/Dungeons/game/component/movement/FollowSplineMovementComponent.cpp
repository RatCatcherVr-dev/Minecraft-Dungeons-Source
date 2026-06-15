#include "FollowSplineMovementComponent.h"
#include "Components/SplineComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/mob/MobBtController.h"
#include "AIController.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "UnrealNetwork.h"

#define FOLLOWSPLINE_DEBUG 0

// Sets default values for this component's properties
UFollowSplineMovementComponent::UFollowSplineMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bReplicates = true;
}


// Called when the game starts
void UFollowSplineMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ABaseCharacter>(GetOwner());

	SplineComponentToFollow = ActorWithSpline ? ActorWithSpline->FindComponentByClass<USplineComponent>() : nullptr;

	if (Owner && Owner->HasAuthority())
	{
		OwnerController = Cast<AMobBtController>(Owner->GetController());

		SetComponentTickEnabled(SplineComponentToFollow != nullptr);

		moveRequest.SetUsePathfinding(true);
		moveRequest.SetAllowPartialPath(false);
		moveRequest.SetProjectGoalLocation(false);
		moveRequest.SetAcceptanceRadius(20.0f);
		moveRequest.SetReachTestIncludesAgentRadius(true);
		moveRequest.SetCanStrafe(true);

		currentDistance = 0;
		if (SplineComponentToFollow) {
			FVector OwnerNewLocation = SplineComponentToFollow->GetLocationAtDistanceAlongSpline(currentDistance, ESplineCoordinateSpace::World);
			Owner->SetActorLocation(OwnerNewLocation);
			SplineLength = SplineComponentToFollow->GetSplineLength();
		}

		bOnTheSpline = bStartOnTheSpline;

		SplineRestDistanceTime.GetKeys(RestPointsPercentages);

		if (MovementType != ESplineMovementType::WithoutNavMesh) {
			OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UFollowSplineMovementComponent::OnNormalMoveCompleted);
		}

		NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
		bIsOnNavMesh = IsLocationOnNavMesh(Owner->GetActorLocation());
	}
	else
	{
		SetComponentTickEnabled(false);
	}
}


// Called every frame
void UFollowSplineMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (IsActive()) {
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

		UpdateOnSplineMovement(DeltaTime);
	}
}

void UFollowSplineMovementComponent::UpdateOnSplineMovement(float DeltaTime)
{
	if (bOnTheSpline && SplineComponentToFollow && OwnerController) {
		float distanceToGoTo = FMath::Min(bIsOnNavMesh ? currentDistance + distanceForwardToCheck : currentDistance + (SplineSpeed * DeltaTime), SplineLength);
		FVector newLocation = SplineComponentToFollow->GetLocationAtDistanceAlongSpline(distanceToGoTo, ESplineCoordinateSpace::World);
		bool bIsCurrentlyOnNavMesh = ShouldDoNavMeshLogic(newLocation);
		if (bIsOnNavMesh != bIsCurrentlyOnNavMesh) {
			bIsOnNavMesh = bIsCurrentlyOnNavMesh;
		}

		if (RestPointsPercentages.Num() > 0 && RestPointsPercentages.IsValidIndex(currentRestPoint)) {
			if (distanceToGoTo > SplineLength * RestPointsPercentages[currentRestPoint]) {
				if (bIsOnNavMesh) {
					OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UFollowSplineMovementComponent::OnMoveToRestCompleted);
				}
				else {
					OnMoveToRestCompleted(FAIRequestID(), EPathFollowingResult::Success);
				}
				SetComponentTickEnabled(false);
				return;
			}
		}

		bIsMoving = true;

		if (bIsOnNavMesh) {
			SetMoveRequestGoal(newLocation);

			OwnerController->MoveTo(moveRequest);
		}
		else {
			newLocation = UKismetMathLibrary::VLerp(Owner->GetActorLocation(), newLocation, 1.0f);
			Owner->SetActorLocation(newLocation);
		}
		
		currentDistance = ComputeNearestDistanceToLocation(GetOwner()->GetActorLocation());

#if FOLLOWSPLINE_DEBUG
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("Current Distance = %f  UsingNavMesh = %d"), currentDistance, bIsOnNavMesh));
#endif

		if (currentDistance + distanceForwardToCheck >= SplineLength) {
			if (bLoopTrack) {
				currentDistance = 0;
			}
			else {
				bFinalSocketInformation = true;
			}
		}
		UpdateSocketInfo();
		TryDestroyOwner();
	}
}

void UFollowSplineMovementComponent::SetMoveRequestGoal(FVector newLocation)
{
	if (moveRequest.IsValid()) {
		moveRequest.UpdateGoalLocation(newLocation);
	}
	else {
		moveRequest.SetGoalLocation(newLocation);
	}
}

void UFollowSplineMovementComponent::OnRestCompleted()
{
	SetComponentTickEnabled(true);
}

void UFollowSplineMovementComponent::OnRep_OnCurrentDistanceUpdated()
{
	currentDistance = FMath::Lerp(lastCurrentDistance, currentDistance, 0.5f);
	lastCurrentDistance = currentDistance;

	FVector newLocation = SplineComponentToFollow->GetLocationAtDistanceAlongSpline(currentDistance, ESplineCoordinateSpace::World);
	newLocation = UKismetMathLibrary::VInterpTo(Owner->GetActorLocation(), newLocation, GetWorld()->GetDeltaSeconds() ,1.f);
	Owner->SetActorLocation(newLocation);
}

bool UFollowSplineMovementComponent::IsLocationOnNavMesh(FVector LocationToGo)
{
	const FNavAgentProperties& AgentProps = OwnerController->GetNavAgentPropertiesRef();
	FNavLocation ProjectedLocation;
	if (NavSys) {
		return NavSys->ProjectPointToNavigation(LocationToGo, ProjectedLocation, INVALID_NAVEXTENT, &AgentProps);
	}
	return false;
}

bool UFollowSplineMovementComponent::ShouldDoNavMeshLogic(FVector LocationToGo)
{
	switch (MovementType)
	{
	default:
		break;
	case ESplineMovementType::OnlyOnNavMesh:
		return true;
		break;
	case ESplineMovementType::WithoutNavMesh:
		return false;
		break;
	case ESplineMovementType::Mixed:
		return IsLocationOnNavMesh(LocationToGo);
		break;
	}
	return false;
}

void UFollowSplineMovementComponent::TryDestroyOwner()
{
	if (!bLoopTrack && SplineComponentToFollow && bFinalSocketInformation) {
		bool bShouldDestroy = true;
		for (FName name : Owner->GetMesh()->GetAllSocketNames()) {
			float SocketDistance = ComputeNearestDistanceToLocation(Owner->GetMesh()->GetSocketLocation(name));
			if (SocketDistance < SplineLength - (BoneDistance * 2)) {
				bShouldDestroy = false;
			}
		}

		if (bShouldDestroy) {
			Owner->Destroy();
		}
	}
}

void UFollowSplineMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFollowSplineMovementComponent, currentDistance);
	DOREPLIFETIME(UFollowSplineMovementComponent, bOnTheSpline);
	DOREPLIFETIME(UFollowSplineMovementComponent, bIsMoving);
	DOREPLIFETIME(UFollowSplineMovementComponent, SocketsInfo);
}

USplineComponent* UFollowSplineMovementComponent::GetFollowingSplineComponent()
{
	return SplineComponentToFollow;
}

float UFollowSplineMovementComponent::ComputeNearestDistanceToLocation(FVector InLoc)
{
	if (!SplineComponentToFollow)
	{
		return 0.f;
	}

	const float ClosestInputKey = SplineComponentToFollow->FindInputKeyClosestToWorldLocation(InLoc);
	const int32 PreviousPoint = FMath::TruncToInt(ClosestInputKey);

	// Lerp between the previous and the next spline points
	float Distance = SplineComponentToFollow->GetDistanceAlongSplineAtSplinePoint(PreviousPoint);
	Distance += (ClosestInputKey - PreviousPoint) * (SplineComponentToFollow->GetDistanceAlongSplineAtSplinePoint(PreviousPoint + 1) - Distance);

	// The linear approximation is not enough
	// So here is a kinda numerical approximation, a couple of iterations should do it
	for (int32 i = 0; i < 32; ++i)
	{
		const float InputKeyAtDistance = SplineComponentToFollow->SplineCurves.ReparamTable.Eval(Distance, 0.0f);

		// The euclidean distance between the current calculated distance and the real closest point
		const float Delta = (SplineComponentToFollow->GetLocationAtSplineInputKey(InputKeyAtDistance, ESplineCoordinateSpace::World) - SplineComponentToFollow->GetLocationAtSplineInputKey(ClosestInputKey, ESplineCoordinateSpace::World)).Size();

		if (InputKeyAtDistance < ClosestInputKey)
		{
			Distance += Delta;
		}
		else if (InputKeyAtDistance > ClosestInputKey)
		{
			Distance -= Delta;
		}
		else
		{
			break;
		}
	}

	return FMath::Clamp(Distance, 0.0f, SplineLength);
}

void UFollowSplineMovementComponent::RequestMoveToSpline()
{
	SetMoveRequestGoal(SplineComponentToFollow->GetLocationAtDistanceAlongSpline(currentDistance, ESplineCoordinateSpace::World));
	OwnerController->ReceiveMoveCompleted.AddDynamic(this, &UFollowSplineMovementComponent::OnMoveToSplineCompleted);
	OwnerController->MoveTo(moveRequest);
}

void UFollowSplineMovementComponent::OnMoveToSplineCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UFollowSplineMovementComponent::OnMoveToSplineCompleted);
	bOnTheSpline = true;
}

void UFollowSplineMovementComponent::OnMoveToRestCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UFollowSplineMovementComponent::OnMoveToRestCompleted);

	bIsMoving = false;

	FTimerHandle RestTimeHandler;
	GetWorld()->GetTimerManager().SetTimer(RestTimeHandler, this, &UFollowSplineMovementComponent::OnRestCompleted, SplineRestDistanceTime[RestPointsPercentages[currentRestPoint]]);

	currentRestPoint++;
}

void UFollowSplineMovementComponent::OnNormalMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	OwnerController->ReceiveMoveCompleted.RemoveDynamic(this, &UFollowSplineMovementComponent::OnNormalMoveCompleted);
	if (currentDistance < SplineLength) {
		if (MovementType == ESplineMovementType::OnlyOnNavMesh) {
			Owner->Destroy();
		}
	}
}

bool UFollowSplineMovementComponent::IsOnSpline()
{
	return bOnTheSpline;
}

TArray<FSocketTransformInfo> UFollowSplineMovementComponent::GetSocketInfo()
{
	return SocketsInfo;
}

void UFollowSplineMovementComponent::UpdateSocketInfo()
{
	if (Owner && SplineComponentToFollow) {
		SocketsInfo.Empty();
		float currentDistanceCopy = currentDistance;
		for (FName name : Owner->GetMesh()->GetAllSocketNames()) {
			if (bFinalSocketInformation) {
				currentDistanceCopy = FMath::Min(ComputeNearestDistanceToLocation(Owner->GetMesh()->GetSocketLocation(name)) + (SplineSpeed * UGameplayStatics::GetWorldDeltaSeconds(this)), SplineLength);
			}

			FSocketTransformInfo socketInfo;
			socketInfo.SocketName = name;
			socketInfo.SocketLocation = UKismetMathLibrary::VLerp(Owner->GetMesh()->GetSocketLocation(name), GetSocketLocation(name, currentDistanceCopy), 1.f);
			socketInfo.SocketRotation = SplineComponentToFollow->GetTangentAtDistanceAlongSpline(currentDistanceCopy, ESplineCoordinateSpace::World).ToOrientationRotator();
			socketInfo.SocketRotation += BoneRotatorOffset;
			SocketsInfo.Add(socketInfo);

			if (!bFinalSocketInformation) {
				currentDistanceCopy = currentDistanceCopy - BoneDistance < 0 ?
					bLoopTrack ? SplineLength + (currentDistanceCopy - BoneDistance) : 0
					:
					currentDistanceCopy - BoneDistance;
			}
		}
	}
}

FVector UFollowSplineMovementComponent::GetSocketLocation(FName socketName, float distance)
{
	FVector SocketOnSplineLocation = SplineComponentToFollow->GetLocationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World);
	return IsOnSpline() ? SocketOnSplineLocation : Owner->GetMesh()->GetSocketLocation(socketName);
}

float UFollowSplineMovementComponent::GetCurrentDistance()
{
	return currentDistance;
}

bool UFollowSplineMovementComponent::IsMoving()
{
	return bIsMoving;
}



FSocketTransformInfo UFollowSplineMovementComponent::GetSocketInformationFromName(FName SocketName)
{
	for (FSocketTransformInfo socketInfo : SocketsInfo)
	{
		if (socketInfo.SocketName.IsEqual(SocketName)) {
			return socketInfo;
		}
	}
	return FSocketTransformInfo();
}

FVector UFollowSplineMovementComponent::GetLocationFromSocketInformation(FSocketTransformInfo SocketInformation)
{
	return SocketInformation.SocketLocation;
}

FRotator UFollowSplineMovementComponent::GetRotationFromSocketInformation(FSocketTransformInfo SocketInformation)
{
	return SocketInformation.SocketRotation;
}
