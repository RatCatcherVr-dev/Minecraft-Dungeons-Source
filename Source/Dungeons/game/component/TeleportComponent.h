#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "lovika/tile/TileDef.h"
#include "lovika/BlockCuboid.h"
#include "game/actor/character/player/PlayerCommonTypes.h"
#include "Animation/AnimMontage.h"
#include "TeleportComponent.generated.h"

class APlayerCharacter;

DECLARE_LOG_CATEGORY_EXTERN(LogTeleport, Log, All);
DECLARE_MULTICAST_DELEGATE(FOnTeleported);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeleportStateChanged, ETeleportState, TeleportState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHasValidTeleportTargetsChanged);

namespace game { class Game; }

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class DUNGEONS_API UTeleportComponent : public UActorComponent
{
GENERATED_BODY()
public:
	UTeleportComponent();
	bool TeleportToStart() const;
	bool TeleportAfterDeath() const;
	bool TeleportAfterReconnect() const;
	bool TeleportToPosition(const FVector& position, bool ensureNavMesh = false) const;
	bool TeleportToPreviousDoor() const;
	bool PopToPosition(const FVector& target, const FVector& leaderPosition) const;
	bool TeleportToPositionWithoutPathfinding(const FVector& position) const;
	bool TeleportToFirstLivingPlayer() const;
	bool TeleportToSafety() const;
	bool TeleportToInstantTravel(const FVector& DestLocation, const FRotator& DestRotation) const;
	bool TeleportToInstantTravel(USceneComponent* Destination) const;
	bool TeleportToRaw(const FVector& DestLocation, const FRotator& DestRotation) const;
	FOnTeleported OnTeleported;
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	inline ETeleportState TeleportState() { return mTeleportState; }

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnTeleportStateChanged OnTeleportStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnHasValidTeleportTargetsChanged OnHasValidTeleportTargetsChanged;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Animation")
	class UAnimMontage* TeleportAnimation;
	UFUNCTION(Server, Reliable, WithValidation) // private or public?
	void Server_TeleportToFriend(const APlayerCharacter* target);
	UFUNCTION(Server, Reliable, WithValidation) // private or public?
	void Server_TeleportToCheckpoint();
	UFUNCTION(Server, Unreliable, WithValidation)
	void Server_AbortTeleportTo(bool force);

	using CandidateFilter = std::function<bool(const APlayerCharacter*)>;
	using CandidateScorer = std::function<float(const APlayerCharacter*)>;
	APlayerCharacter* GetClosestFilteredCandidate(CandidateFilter filter, CandidateScorer scorer) const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	APlayerCharacter* GetClosestDownedCandidate() const;

	// #D11.CM
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<APlayerCharacter*> GetTeleportCandidates() const;

	// #D11.CM
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<APlayerCharacter*> GetTeleportDependents() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsTeleportTarget(APlayerCharacter* potentialTarget) const;

	// #D11.CM
	void ProcessHeldStates();

	TArray<FVector> GetNavPositionsAroundPoint(FVector point);

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void BeginPlay() override;
	
private:
	void OnPlayerLeftOrJoined();
	bool AttemptBindToGamePlayerJoinOrLeft();
	void UpdatePlayerList(const class AGameStateBase*);
	
	bool TeleportToNearestUnrealRespawn() const;
	bool TeleportToCurrentCheckpoint() const;
	bool TeleportToPreviousCheckpoint() const;
	bool TeleportToCheckpoint(TOptional<BlockCuboid> checkpoint) const;
	bool TeleportToFirstAdjustable(const TArray<FVector>& sortedCandidates, bool pathfind, bool ensureNavMesh = false) const;
	
	class APlayerCharacter* PlayerCharacter() const;
	void SetState(ETeleportState state);

	UFUNCTION()
	void OnRep_TeleportState(ETeleportState oldState);
	
	void TeleportFlyOff();
	void TeleportComplete();

	void IssueTeleportMovement(const FVector& DestLocation, const FRotator& DestRotation) const;

	UFUNCTION(Client, Reliable)
	void Client_IssueTeleportMovement(const FVector& DestLocation, const FRotator& DestRotation) const;

	
	bool CreateAdjustedTeleportToVector(const FVector& DestLocation, const FRotator& DestRotation, FVector& OutAdjustedTeleportLocation) const;

	FTimerHandle mTeleportTimerHandle;
	UPROPERTY(ReplicatedUsing = OnRep_TeleportState)
	ETeleportState mTeleportState = ETeleportState::Idle;
	UPROPERTY(Replicated)
	const APlayerCharacter* mTeleportTargetPlayer = nullptr;
	UPROPERTY(Replicated)
	bool mTeleportToCheckpoint = false;

	class APlayerCharacter* GetPlayerOwner() const;
	game::Game* GetGame() const;
	TArray<FVector> GetStartPositions() const;
	TArray<FVector> GetNearDoorPositions(const DoorDef&) const;
	TArray<FVector> GetPositionsAround(const FVector& center) const;
	TArray<FVector> GetPositionsInside(const BlockCuboid& center) const;


	UPROPERTY(Transient)
	TArray<APlayerCharacter*> TeleportTargets;
};
