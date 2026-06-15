#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/mob/MobBtController.h"
#include "NavigationSystem.h"
#include "FollowSplineMovementComponent.generated.h"

UENUM(BlueprintType)
enum class ESplineMovementType : uint8{
	OnlyOnNavMesh,
	WithoutNavMesh,
	Mixed,
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FSocketTransformInfo {

	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FName SocketName;

	UPROPERTY(BlueprintReadWrite)
	FVector SocketLocation;

	UPROPERTY(BlueprintReadWrite)
	FRotator SocketRotation;
};

/*
	Component to make an actor to move along with a spline.
	The actor moves towards a point in the spline but if we need
	to have bones manipulations we need to do that in an anim BP.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UFollowSplineMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFollowSplineMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "FollowSplineMovement")
	AActor* ActorWithSpline;

	UFUNCTION(BlueprintPure, Category = "FollowSplineMovement")
	USplineComponent* GetFollowingSplineComponent();
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "FollowSplineMovement")
	float ComputeNearestDistanceToLocation(FVector InLoc);

	UFUNCTION(BlueprintCallable)
	void RequestMoveToSpline();
	UFUNCTION()
	void OnMoveToSplineCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	UFUNCTION()
	void OnMoveToRestCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	UFUNCTION()
	void OnNormalMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	UFUNCTION(BlueprintPure)
	bool IsOnSpline();

	UPROPERTY(EditAnywhere, Category = "FollowSplineMovement")
	ESplineMovementType MovementType;
	UPROPERTY(EditAnywhere, Category = "FollowSplineMovement")
	float SplineSpeed = 200.f;
	UPROPERTY(EditAnywhere, Category = "FollowSplineMovement")
	bool bStartOnTheSpline = true;
	UPROPERTY(EditAnywhere, Category = "FollowSplineMovement")
	bool bLoopTrack = false;

	// Defines rest points within the spline
	// Key: Percentage 0 - 1, being 1 the spline length
	// Value: Time to rest in seconds
	UPROPERTY(EditAnywhere, Category = "FollowSplineMovement")
	TMap<float, float> SplineRestDistanceTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FollowSplineMovement | Bones")
	FRotator BoneRotatorOffset = FRotator(0.f, -90.f, 90.f);

	// Used to calculate where the bone should be compared with it's root
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FollowSplineMovement | Bones")
	float BoneDistance = 30;

	UFUNCTION(BlueprintPure)
	TArray<FSocketTransformInfo> GetSocketInfo();
	void UpdateSocketInfo();

	FVector GetSocketLocation(FName socketName, float distance);

	UFUNCTION(BlueprintPure)
	float GetCurrentDistance();

	UFUNCTION(BlueprintPure)
	bool IsMoving();


	UFUNCTION(BlueprintPure)
	FSocketTransformInfo GetSocketInformationFromName(FName SocketName);
	UFUNCTION(BlueprintPure)
	FVector GetLocationFromSocketInformation(FSocketTransformInfo SocketInformation);
	UFUNCTION(BlueprintPure)
	FRotator GetRotationFromSocketInformation(FSocketTransformInfo SocketInformation);

private:
	void UpdateOnSplineMovement(float DeltaTime);
	void SetMoveRequestGoal(FVector newLocation);
	UFUNCTION()
	void OnRestCompleted();
	UFUNCTION()
	void OnRep_OnCurrentDistanceUpdated();

	bool IsLocationOnNavMesh(FVector LocationToGo);
	bool ShouldDoNavMeshLogic(FVector LocationToGo);
	void TryDestroyOwner();

	USplineComponent* SplineComponentToFollow;
	AMobBtController* OwnerController;
	ABaseCharacter* Owner;
	UPROPERTY(ReplicatedUsing = OnRep_OnCurrentDistanceUpdated)
	float currentDistance = 0.f;
	float lastCurrentDistance = .0f;
	FAIMoveRequest moveRequest;
	float distanceForwardToCheck = 200;
	UPROPERTY(Replicated)
	bool bOnTheSpline = false;
	float currentRestPoint = 0;
	float SplineLength = 0;

	UPROPERTY(Replicated)
	bool bIsMoving = false;

	UPROPERTY(Replicated)
	TArray<FSocketTransformInfo> SocketsInfo;

	TArray<float> RestPointsPercentages;

	UNavigationSystemV1* NavSys;
	bool bIsOnNavMesh = false;

	bool bFinalSocketInformation = false;
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
