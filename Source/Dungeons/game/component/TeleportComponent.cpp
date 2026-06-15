#include "Dungeons.h"
#include "TeleportComponent.h"
#include "Components/CapsuleComponent.h"
#include "game/util/LocationQuery.h"
#include "game/Game.h"
#include "GameFramework/PlayerStart.h"
#include "game/GameProgress.h"
#include "game/Conversion.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "Animation/AnimMontage.h"
#include <NavigationSystem.h>
#include "PlayerCharacterMovementComponent.h"
#include "DungeonsGameState.h"
#include "DungeonsUserManagement.h"
#include "DungeonsGameInstance.h"
#include "Assets/DungeonsAssetManager.h"
#include "CommonTypes.h"
#include "game/component/ElytraComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "UnrealRespawnCandidateComponent.h"

DEFINE_LOG_CATEGORY(LogTeleport)

namespace teleportcomponent {
	static const FVector SearchCuboid = {1000, 1000, 500};
	static const BlockPos SearchCuboidBlockSpace = conversion::ueToBlock(SearchCuboid);
	static const FVector DefaultNavMeshExtent{ 25, 25, 125 };
	static const FVector NarrowNavMeshExtent{ 10, 10, 125 };

	bool isOccupied(UWorld& world, const FVector& center, float radius) {
		return actorquery::getNearbyActors<AMobCharacter>(&world, center, radius).Num() > 0;
	}

	bool canPathfind(const AActor* owner, const FVector& from, const FVector& to) {
		const auto* navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(owner->GetWorld());
		FPathFindingQuery QueryParams;
		QueryParams.StartLocation = from;
		QueryParams.EndLocation = to;
		QueryParams.NavData = navSystem->GetDefaultNavDataInstance();
		QueryParams.Owner = owner;
		QueryParams.SetAllowPartialPaths(false);
		return navSystem->TestPathSync(QueryParams, EPathFindingMode::Hierarchical);
	}

	TOptional<FVector> projectToNavMesh(UWorld& world, const FVector& pos, const FVector& extent) {
		FNavLocation out;
		if (!FNavigationSystem::GetCurrent<UNavigationSystemV1>(&world)->ProjectPointToNavigation(pos, out, extent))
			return {};
		return {out.Location};
	}

	// #D11.CM
	bool isWithinNavData(UWorld& world, const FVector& center, float radius) {
		// Create our player "corners"
		FVector vXPlus{ center.X + radius, center.Y, center.Z };
		FVector vXMinus{ center.X - radius, center.Y, center.Z };
		FVector vYPlus{ center.X , center.Y + radius, center.Z };
		FVector vYMinus{ center.X, center.Y - radius, center.Z };

		// Check if our "corners" are in the nav mesh
		auto navXPlus = projectToNavMesh(world, vXPlus, NarrowNavMeshExtent);
		auto navXMinus = projectToNavMesh(world, vXMinus, NarrowNavMeshExtent);
		auto navYPlus = projectToNavMesh(world, vYPlus, NarrowNavMeshExtent);
		auto navYMinus = projectToNavMesh(world, vYMinus, NarrowNavMeshExtent);

		if (navXPlus.IsSet() && navXMinus.IsSet() && navYPlus.IsSet() && navYMinus.IsSet()) {
			return true;
		}

		return false;
	}

	auto DistanceSorter(const FVector& center) {
		return [center](const FVector& lhs, const FVector& rhs) {
			return FVector::DistSquared(lhs, center) < FVector::DistSquared(rhs, center);
		};
	}
}

UTeleportComponent::UTeleportComponent()
{
	bReplicates = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = true;
}

bool UTeleportComponent::TeleportToStart() const
{
	UE_LOG(LogTeleport, Log, TEXT("TeleportToStart"));

	const auto StartPositions = GetStartPositions();
	return TeleportToFirstAdjustable(StartPositions, false);
}

bool UTeleportComponent::TeleportAfterDeath() const
{
	UE_LOG(LogTeleport, Log, TEXT("TeleportAfterDeath"));

	if (GetGame()) {
		return TeleportToNearestUnrealRespawn() || TeleportToCurrentCheckpoint() || TeleportToPreviousDoor() || TeleportToPreviousCheckpoint() || TeleportToStart();
	} else {
		TArray<FVector> candidates;
		candidates.Add(actorquery::getFirstActor<APlayerStart>(GetWorld())->GetActorLocation());
		candidates.Add(GetPlayerOwner()->GetActorLocation());
		return TeleportToFirstAdjustable(candidates, false);
	}
}

bool UTeleportComponent::TeleportAfterReconnect() const
{
	UE_LOG(LogTeleport, Log, TEXT("TeleportAfterReconnect"));

	const auto* game = GetGame();
	const auto& tiles = game->tiles();

	TArray<FVector> candidates;
	if (const auto furthest = game->progress().currentlyFurthest()) {
		if (TeleportToPosition(furthest->player->GetActorLocation())) {
			return true;
		}
		if (const auto& prev = tiles.getRespawnDoor(furthest->tile)) {
			if (TeleportToFirstAdjustable(GetNearDoorPositions(prev->door), false)) {
				return true;
			}
		}
	}
	return TeleportToStart();
}

bool UTeleportComponent::TeleportToPosition(const FVector& position, bool ensureNavMesh /* = false */) const {
	if (position == FVector::ZeroVector) {
		ensure(false && "TeleportToPosition: tried to teleport to 0, 0, 0");
		return false;
	}

	UE_LOG(LogTeleport, Log, TEXT("TeleportToPosition"));
	return TeleportToFirstAdjustable(GetPositionsAround(position), true, ensureNavMesh);
}

// #D11.CM - This uses TeleportToPosition but does some pathfinding checks first.
// This checks if the proposed popping radius position is a viable target, pathfinding out from the leaders position. 
// If we can't find a viable pathing option, this likely means our target is out of bounds and we must instead pop to our leader.
bool UTeleportComponent::PopToPosition(const FVector& target, const FVector& leaderPosition) const {
	const auto navStart = teleportcomponent::projectToNavMesh(*GetWorld(), leaderPosition, teleportcomponent::DefaultNavMeshExtent);
	const auto navTarget = teleportcomponent::projectToNavMesh(*GetWorld(), target, teleportcomponent::DefaultNavMeshExtent);

	// If our radius pop target is viable, teleport there.
	if (navStart.IsSet() && navTarget.IsSet()) {
		if (teleportcomponent::canPathfind(GetPlayerOwner(), navStart.GetValue(), navTarget.GetValue())) {
			if (TeleportToPosition(target, true)) {
				return true;
			}
		}
	}

	// If our radius position isn't viable, teleport to the leader.
	return TeleportToPosition(leaderPosition, true);
}

bool UTeleportComponent::TeleportToPositionWithoutPathfinding(const FVector& position) const {
	if (position == FVector::ZeroVector) {
		ensure(false && "TeleportToPositionWithoutPathfinding: tried to teleport to 0, 0, 0");
		return false;
	}

	UE_LOG(LogTeleport, Log, TEXT("TeleportToPositionWithoutPathfinding"));
	return TeleportToFirstAdjustable(GetPositionsAround(position), false);
}

bool UTeleportComponent::TeleportToFirstLivingPlayer() const
{
	const auto* playerOwner = GetPlayerOwner();

	for (auto playerControllersIterator = GetWorld()->GetPlayerControllerIterator(); playerControllersIterator; ++playerControllersIterator) {
		if (const auto player = Cast<APlayerCharacter>(playerControllersIterator->Get()->GetCharacter())) {
			if (player == playerOwner) {
				continue;
			}
			
			if (player->GetAliveState() != EAliveState::Dead && player->GetWorldState() == ECharacterWorldState::InWorld) {
				if (TeleportToPosition(player->GetActorLocation())) {
					return true;
				}				
			}
		}
	}

	return false;
}

bool UTeleportComponent::TeleportToSafety() const {
	return TeleportToFirstLivingPlayer() || TeleportToPreviousDoor() || TeleportToStart();
}

bool UTeleportComponent::TeleportToInstantTravel(const FVector& DestLocation, const FRotator& DestRotation) const
{
	auto AdjustedLocation = DestLocation;

	FHitResult result;

	if (GetWorld()->LineTraceSingleByChannel(result, DestLocation, DestLocation - FVector(0, 0, 5000.f), (ECollisionChannel)ECustomTraceChannels::TerrainAndPlayerOnly)) {
		AdjustedLocation = result.Location;
	}

	if (const auto actorCapsule = GetOwner()->FindComponentByClass<UCapsuleComponent>()) {
		AdjustedLocation.Z += actorCapsule->GetScaledCapsuleHalfHeight();
	}

	return TeleportToRaw(AdjustedLocation, DestRotation);
}

bool UTeleportComponent::TeleportToInstantTravel(USceneComponent* Destination) const
{
	const auto destinationRotation = Destination->GetComponentRotation();
	const auto destinationLocation = Destination->GetComponentLocation();
	
	auto AdjustedLocation = destinationLocation;

	FHitResult result;

	if (GetWorld()->LineTraceSingleByChannel(result, destinationLocation, destinationLocation - FVector(0, 0, 5000.f), (ECollisionChannel)ECustomTraceChannels::TerrainAndPlayerOnly)) {
		AdjustedLocation = result.Location;
	}

	if (const auto actorCapsule = GetOwner()->FindComponentByClass<UCapsuleComponent>()) {
		AdjustedLocation.Z += actorCapsule->GetScaledCapsuleHalfHeight();
	}

	if(auto character = Cast<ABaseCharacter>(GetOwner()))
	{
		character->ResetFallFromLocation();
	}

	return TeleportToRaw(AdjustedLocation, destinationRotation);
}

TArray<FVector> UTeleportComponent::GetStartPositions() const {
	return GetPositionsInside(GetGame()->tiles().getStartCuboid());
}

TArray<FVector> UTeleportComponent::GetNearDoorPositions(const DoorDef& door) const {
	const auto outOfDoorOffset = Facing::DIRECTION[door.neighbourFacing()] * 2;
	auto positions = locationquery::findGroundBlockPositions(*GetWorld(), door.positions() + outOfDoorOffset, teleportcomponent::SearchCuboidBlockSpace);
	positions.Sort(teleportcomponent::DistanceSorter(conversion::blockToUe(door.position() + outOfDoorOffset)));
	return positions;
}

TArray<FVector> UTeleportComponent::GetPositionsInside(const BlockCuboid& area) const {
	TArray<FVector> positions;
	for (auto&& pos : area) {
		positions.Add(conversion::posToUe(pos.above().center()));
	}
	const FVector centerPos = centerFloor(area).toUeSpace();
	positions.Sort(teleportcomponent::DistanceSorter(centerPos));
	return positions;
}

TArray<FVector> UTeleportComponent::GetPositionsAround(const FVector& center) const {
	auto positions = locationquery::findGroundBlockPositions(*GetWorld(), center, teleportcomponent::SearchCuboid);

 	if (positions.Num() == 0) {
 		// #D11.CM - Try and get navmesh positions, as we may be on a bridge or another navigable unreal object.
		positions = locationquery::findNavMeshPositions(*GetWorld(), center, teleportcomponent::SearchCuboid);
 	}

	positions.Sort(teleportcomponent::DistanceSorter(center));

	return positions;
}

APlayerCharacter* UTeleportComponent::GetPlayerOwner() const {
	return Cast<APlayerCharacter>(GetOwner());
}

game::Game* UTeleportComponent::GetGame() const {
	return actorquery::getGame(GetPlayerOwner()->GetWorld());
}

APlayerCharacter* UTeleportComponent::GetClosestFilteredCandidate(CandidateFilter filter, CandidateScorer scorer) const {
	return algo::min_element_by(algo::copy_if(GetTeleportCandidates(), filter), scorer).Get(nullptr);
}

APlayerCharacter* UTeleportComponent::GetClosestDownedCandidate() const {
	const auto loc = GetOwner()->GetActorLocation();
	CandidateScorer scorer = [loc](const APlayerCharacter* candidate) {
		return actorquery::getActorDistance(loc, candidate);
	};
	return GetClosestFilteredCandidate(RETLAMBDA(it->GetAliveState() == EAliveState::Down), scorer);
}

TArray<APlayerCharacter*> UTeleportComponent::GetTeleportCandidates() const {
	return algo::copy_if(TeleportTargets, RETLAMBDA(it != nullptr));
}

TArray<APlayerCharacter*> UTeleportComponent::GetTeleportDependents() const {
	TArray<APlayerCharacter*> players;

	if (const auto* state = Cast<ADungeonsGameState>(GetWorld()->GetGameState())) {
		for (const auto* playerState : state->PlayerArray) {
			APlayerCharacter* player = Cast<APlayerCharacter>(playerState->GetPawn());

			// #D11.CM - This works for server based local co-op, but client side local co-op currently won't correctly populate dependents server side.
			if (player && player != GetPlayerOwner() && player->IsLocallyControlled() && !player->GetPlayerController()->IsPrimaryPlayer()) {
				players.Add(player);
			}
		}
	}
	return players;
}

bool UTeleportComponent::IsTeleportTarget(APlayerCharacter * potentialTarget) const
{
	return mTeleportTargetPlayer == potentialTarget;
}

TArray<FVector> UTeleportComponent::GetNavPositionsAroundPoint(FVector point) {
	auto positions = GetPositionsAround(point);
	positions.Sort(teleportcomponent::DistanceSorter(point));
	return positions;
}

void UTeleportComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	if(AttemptBindToGamePlayerJoinOrLeft()) {
		SetComponentTickEnabled(false);
	}
}

void UTeleportComponent::BeginPlay() {
	Super::BeginPlay();
	if(GetPlayerOwner()->IsLocallyControlled()) {
		if(!AttemptBindToGamePlayerJoinOrLeft()) {
			SetComponentTickEnabled(true);
		}
	}
}

void UTeleportComponent::OnPlayerLeftOrJoined() {
	UpdatePlayerList(GetWorld()->GetGameState());
}

bool UTeleportComponent::AttemptBindToGamePlayerJoinOrLeft() {
	if(auto* state = Cast<ADungeonsGameState>(GetWorld()->GetGameState())) {
		state->OnPlayerAddedOrRemovedInternal.AddUObject(this, &UTeleportComponent::OnPlayerLeftOrJoined);
		UpdatePlayerList(state);
		return true;
	}

	return false;
}

void UTeleportComponent::UpdatePlayerList(const AGameStateBase* gameState) {
	TeleportTargets.Empty();

	for(const auto& player : InstanceTracker<APlayerCharacter>::GetList(GetWorld())) {
		if(!player->IsLocallyControlled() && GetOwner() != player) {
			TeleportTargets.Add(player);
		}
	}

	OnHasValidTeleportTargetsChanged.Broadcast();
}

namespace {

TOptional<game::DoorToTile> findDoorBetweenCurrentAndPreviousTile(const game::Game* game, const APlayerCharacter& player) {
	if (!game) {
		return {};
	}
	return game->tiles().findDoorBetween(
		game->progress().current(player),
		game->progress().previous(player)
	);
}

}

bool UTeleportComponent::TeleportToPreviousDoor() const
{
	auto* player = GetPlayerOwner();
	const auto* game = GetGame();

	auto gi = GetPlayerOwner()->GetGameInstance<UDungeonsGameInstance>();

	if (gi->IsLocalCoop())
	{
		if (auto leader = gi->GetLastLeader())
		{
			player = leader;
		}
	}

	if (const auto between = findDoorBetweenCurrentAndPreviousTile(game, *player)) {
		if (TeleportToFirstAdjustable(GetNearDoorPositions(between->door), false)) {
			return true;
		}
	}

	UE_LOG(LogTeleport, Log, TEXT("Falling back on previous door"));
	if (const auto prev = game->tiles().getRespawnDoor(game->progress().current(*player))) {
		if (TeleportToFirstAdjustable(GetNearDoorPositions(prev->door), false)) {
			return true;
		}
	}

	return false;
}

bool UTeleportComponent::TeleportToCurrentCheckpoint() const {
	UE_LOG(LogTeleport, Log, TEXT("TeleportToCurrentCheckpoint"));
	UDungeonsGameInstance* gameInstance = GetPlayerOwner()->GetGameInstance<UDungeonsGameInstance>();

	TOptional<BlockCuboid> currentCheckpoint = GetPlayerOwner()->GetCurrentCheckpoint();

	if(gameInstance->IsLocalCoop()) {
		if(auto leader = gameInstance->GetLastLeader()) {
			currentCheckpoint = leader->GetCurrentCheckpoint();
		}
	}

	return TeleportToCheckpoint(currentCheckpoint);
}

bool UTeleportComponent::TeleportToPreviousCheckpoint() const {
	UE_LOG(LogTeleport, Log, TEXT("TeleportToPreviousCheckpoint"));
	UDungeonsGameInstance* gameInstance = GetPlayerOwner()->GetGameInstance<UDungeonsGameInstance>();

	TOptional<BlockCuboid> previousCheckpoint = GetPlayerOwner()->GetPreviousCheckpoint();

	if (gameInstance->IsLocalCoop()) {
		if (APlayerCharacter* leader = gameInstance->GetLastLeader()) {
			previousCheckpoint = leader->GetPreviousCheckpoint();
		}
	}

	return TeleportToCheckpoint(previousCheckpoint);
}

bool UTeleportComponent::TeleportToCheckpoint(TOptional<BlockCuboid> checkpoint) const {
	UE_LOG(LogTeleport, Log, TEXT("TeleportToCheckpoint"));

	if (checkpoint) {
		const auto& checkPoint = checkpoint.GetValue();
		auto candidates = locationquery::findGroundBlockPositions(*GetWorld(), checkPoint, teleportcomponent::SearchCuboidBlockSpace);
		candidates.Sort(teleportcomponent::DistanceSorter(conversion::blockCuboidToUe(checkPoint).GetCenter()));
		return TeleportToFirstAdjustable(candidates, false);
	}

	return false;
}

bool UTeleportComponent::TeleportToNearestUnrealRespawn() const {
	// Check if we have any respawn candidates
	auto respawnComponents = InstanceTracker<URespawnCandidateComponent>::GetList(GetWorld());
	if (respawnComponents.Num() == 0) {
		return false;
	}

	// Setup initial Variables
	auto game = GetGame();
	auto player = GetPlayerOwner();
	auto gameInstance = player->GetGameInstance<UDungeonsGameInstance>();
	if (gameInstance->IsLocalCoop()) {
		if (auto leader = gameInstance->GetLastLeader()) {
			player = leader;
		}
	}

	// Try and find a valid checkpoint in this tile
	FVector potentialRespawnLocation = FVector::ZeroVector;
	TOptional<BlockCuboid> checkpoint = GetPlayerOwner()->GetCurrentCheckpoint();
	if (checkpoint.IsSet() && !checkpoint->isEmpty()) {
		potentialRespawnLocation = conversion::blockToUe(checkpoint->maxInclusive());
	}

	// We don't have a valid checkpoint, so instead try to find a valid door.
	if (potentialRespawnLocation.IsZero()) {
		TOptional<game::DoorToTile> doorTile = findDoorBetweenCurrentAndPreviousTile(game, *player);

		if (!doorTile) {
			doorTile = game->tiles().getRespawnDoor(game->progress().current(*player));
		}

		if (doorTile) {
			potentialRespawnLocation = conversion::blockToUe(doorTile->door.position());
		}
	}

	// We don't have a location, so fail out
	if (potentialRespawnLocation.IsZero()) {
		return false;
	}

	// Find our closest candidate
	float distance = FLT_MAX;
	FVector finalRespawnLocation = FVector::ZeroVector;
	for (auto candidate : respawnComponents) {
		if (!candidate->GetIsRespawnActive()) {
			continue;
		}

		auto currentLocation = candidate->GetRespawnLocation();

		// Make sure that our candidate is within our current tile.
		if (game->progress().current(*player).bounds().contains(conversion::ueToBlock(currentLocation))) {
			auto newDist = FVector::DistSquared(potentialRespawnLocation, currentLocation);
			if (newDist < distance) {
				distance = newDist;
				finalRespawnLocation = currentLocation;
			}
		}
	}

	// If we have a location, try to teleport there.
	if (!finalRespawnLocation.IsZero()) {
		return TeleportToFirstAdjustable(GetPositionsAround(finalRespawnLocation), false);
	}

	return false;
}

bool UTeleportComponent::TeleportToFirstAdjustable(const TArray<FVector>& sortedCandidates, bool pathfind, bool ensureNavMesh /* = false */) const {
	UE_LOG(LogTeleport, Log, TEXT("TeleportToFirstAdjustable"));
	if (sortedCandidates.Num() == 0) {
		return false;
	}

	auto groundPos = sortedCandidates[0];
	if (pathfind) {
		const auto pathfindTo = teleportcomponent::projectToNavMesh(*GetWorld(), groundPos, teleportcomponent::DefaultNavMeshExtent);
		if (pathfind && !pathfindTo) {
			UE_LOG(LogTeleport, Warning, TEXT("Trying to find a teleport position that is pathfindable failed since the center could not find any nav mesh."));
			return false;
		}
		groundPos = pathfindTo.GetValue();
	}
	float radius = 0.0f;
	const auto actorCapsule = GetPlayerOwner()->FindComponentByClass<UCapsuleComponent>();
	if (actorCapsule) {
		radius = actorCapsule->GetScaledCapsuleRadius();
	}
	const auto* owner = GetPlayerOwner();
	const auto& rotation = owner->GetActorRotation();
	for (const auto& pos : sortedCandidates) {
		FVector respawnPos;
		if (!CreateAdjustedTeleportToVector(pos, rotation, respawnPos)) {
			continue;
		}

		if (teleportcomponent::isOccupied(*GetWorld(), pos, radius)) {
			continue;
		}

		// #D11.CM - Check if we need to ensure the nav mesh or not.
		if (ensureNavMesh && !teleportcomponent::isWithinNavData(*GetWorld(), pos, radius)) {
			continue;
		}

		if (pathfind && !teleportcomponent::canPathfind(owner, groundPos, pos)) {
			continue;
		}

		if (TeleportToRaw(respawnPos, rotation)) {
			UE_LOG(LogTeleport, Log, TEXT("Teleport successful to %s"), *respawnPos.ToString());
			return true;
		}
		UE_LOG(LogTeleport, Log, TEXT("Teleport failed %s"), *respawnPos.ToString());
	}
	UE_LOG(LogTeleport, Error, TEXT("Teleport failed after trying all %d positions"), sortedCandidates.Num());
	return false;
}


void UTeleportComponent::IssueTeleportMovement(const FVector& DestLocation, const FRotator& DestRotation) const {
	GetPlayerOwner()->GetPlayerCharacterMovementComponent()->SetPresumedTeleportToLocationAndRotation(DestLocation, DestRotation);
	Client_IssueTeleportMovement(DestLocation, DestRotation);
}

void UTeleportComponent::Client_IssueTeleportMovement_Implementation(const FVector& DestLocation, const FRotator& DestRotation) const {
	GetPlayerOwner()->GetPlayerCharacterMovementComponent()->SetWantsTeleportTo(DestLocation, DestRotation);
}

//We make our own because noCheck causes physics to not be teleported
bool UTeleportComponent::TeleportToRaw(const FVector& DestLocation, const FRotator& DestRotation) const {
	auto* player = GetPlayerOwner();
	auto* rootComponent = player->GetRootComponent();
	if (rootComponent == nullptr) {
		return false;
	}

	UWorld* MyWorld = GetWorld();

	// Can't move non-movable actors during play
	if ((rootComponent->Mobility == EComponentMobility::Static) && MyWorld->AreActorsInitialized())
	{
		return false;
	}

	IssueTeleportMovement(DestLocation, DestRotation);
	return true;
}

bool UTeleportComponent::CreateAdjustedTeleportToVector(const FVector& DestLocation, const FRotator& DestRotation, FVector& OutAdjustedTeleportLocation) const {
	auto* player = GetPlayerOwner();
	auto* rootComponent = player->GetRootComponent();
	if (rootComponent == nullptr) {
		return false;
	}

	auto MyWorld = GetWorld();
	// Can't move non-movable actors during play
	if ((rootComponent->Mobility == EComponentMobility::Static) && MyWorld->AreActorsInitialized())	{
		return false;
	}

	FVector const PrevLocation = player->GetActorLocation();
	OutAdjustedTeleportLocation = DestLocation;
	bool bAdjustmentSuccessful = true;
	UPrimitiveComponent* ActorPrimComp = Cast<UPrimitiveComponent>(rootComponent);
	if (ActorPrimComp) {
		if (ActorPrimComp->IsQueryCollisionEnabled() || (GetNetMode() != NM_Client))
		{
			// Apply the pivot offset to the desired location
			FVector Offset = rootComponent->Bounds.Origin - PrevLocation;
			OutAdjustedTeleportLocation += Offset;

			const auto actorCapsule = player->FindComponentByClass<UCapsuleComponent>();
			if(actorCapsule){
				OutAdjustedTeleportLocation.Z += actorCapsule->GetScaledCapsuleHalfHeight();
			}

			// check if able to find an acceptable destination for this actor that doesn't embed it in world geometry
			bAdjustmentSuccessful = MyWorld->FindTeleportSpot(player, OutAdjustedTeleportLocation, DestRotation);
			OutAdjustedTeleportLocation -= Offset;
		}
	}

	return bAdjustmentSuccessful;
}

void UTeleportComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTeleportComponent, mTeleportState);
	DOREPLIFETIME(UTeleportComponent, mTeleportTargetPlayer);
	DOREPLIFETIME(UTeleportComponent, mTeleportToCheckpoint);

}


APlayerCharacter* UTeleportComponent::PlayerCharacter() const 
{
	return Cast<APlayerCharacter>(GetOwner()); 
}

//// Teleport to friend feature
// get rid of
void UTeleportComponent::SetState(ETeleportState state)
{
	ETeleportState oldState = mTeleportState; // for server state diff (needed?)
	mTeleportState = state; // replicates
	if (GetOwner()->HasAuthority())
		OnRep_TeleportState(oldState);
}

bool UTeleportComponent::Server_AbortTeleportTo_Validate(bool force)
{
	return true;
}

void UTeleportComponent::Server_AbortTeleportTo_Implementation(bool force) {
	// stop teleport sequence
	if (mTeleportState == ETeleportState::Charging || force)
	{
		mTeleportToCheckpoint = false;
		GetWorld()->GetTimerManager().ClearTimer(mTeleportTimerHandle);
		SetState(ETeleportState::Idle);
	}
}

//// static helper (not used <<< remove)
static float MatchTeleportDelayToAnimation(UAnimMontage* teleportAnimation) {
	auto timeout = 4.0f - 2.4;

	if (teleportAnimation) {
		float animationTime = 0;
		const auto finishIndex = teleportAnimation->GetSectionIndex("TeleportFinish");

		for (auto i = 0; i < finishIndex; ++i) {
			animationTime += teleportAnimation->GetSectionLength(i);
		}

		if (4.0f > animationTime) {
			const auto swingTime = teleportAnimation->GetSectionLength(teleportAnimation->GetSectionIndex("loopstart"));
			const auto n = FMath::FloorToFloat((4.0f - animationTime + swingTime) / swingTime);
			timeout = animationTime + n * swingTime;
		}
		else {
			timeout = animationTime;
		}
	}

	return timeout;
}

bool UTeleportComponent::Server_TeleportToFriend_Validate(const APlayerCharacter* target)
{
	return TeleportAnimation != nullptr;
}

void UTeleportComponent::Server_TeleportToFriend_Implementation(const APlayerCharacter* target)
{
	check(TeleportAnimation); //, TEXT("Error! Need to set animation asset in BP!"));
	if (!TeleportAnimation)
		return;

	mTeleportToCheckpoint = false;
	mTeleportTargetPlayer = target;
	SetState(ETeleportState::Charging);

	// #D11.CM - Push to our Dependents
	for (auto dependent : GetTeleportDependents())
	{
		dependent->TeleportToFriend(target);
	}
}

bool UTeleportComponent::Server_TeleportToCheckpoint_Validate()
{
	return TeleportAnimation != nullptr;
}

void UTeleportComponent::Server_TeleportToCheckpoint_Implementation()
{
	check(TeleportAnimation); //, TEXT("Error! Need to set animation asset in BP!"));
	if (!TeleportAnimation)
		return;
	
	// Mark us as checkpoint teleporting
	mTeleportToCheckpoint = true;
	SetState(ETeleportState::Charging);
}

void UTeleportComponent::OnRep_TeleportState(ETeleportState oldState)
{
	// updates the animation calls on server and client, triggered by replication
	FString auth = GetOwner()->HasAuthority() ? "S" : "C";
	UE_LOG(LogDungeons, Log, TEXT("(%s)Changed teleport state (p:%s) [%d]->[%d] "), *auth, *(PlayerCharacter()->GetDebugName(PlayerCharacter())), static_cast<int>(oldState), static_cast<int>(mTeleportState));
	
	
	if (static_cast<int>(mTeleportState) - static_cast<int>(oldState) != 1)
		UE_LOG(LogDungeons, Warning, TEXT("Skipped >1 state in transition!"));

	float TeleportMinDelay = 0.01f;
	switch (mTeleportState)
	{
	case ETeleportState::Idle: /// do nothing
		//cancel animation
		if (oldState == ETeleportState::Charging)
		{
			PlayerCharacter()->StopMontage(0.2f, TeleportAnimation);
			if (GetOwner()->HasAuthority()) {
				GetWorld()->GetTimerManager().ClearTimer(mTeleportTimerHandle); // can I do it double?
			}

		}
		break;
	case ETeleportState::Charging:
	{
		if (GetOwner()->HasAuthority()) {
			const auto delay = MatchTeleportDelayToAnimation(TeleportAnimation);
			GetWorld()->GetTimerManager().SetTimer(mTeleportTimerHandle, FTimerDelegate::CreateUObject(this,
				&UTeleportComponent::TeleportFlyOff), FMath::Max(TeleportMinDelay, delay), false);
		}

		if (TeleportAnimation) {
			PlayerCharacter()->PlayMontage(TeleportAnimation);
		}

		break;
	}
	case ETeleportState::Flying:
	{
		if (GetOwner()->HasAuthority()) {
			float teleport_flyoff_time = TeleportAnimation->GetSectionLength(TeleportAnimation->GetSectionIndex("TeleportFinish"));
			// trigger finish delay
			float teleport_timeout = FMath::Max(TeleportMinDelay, teleport_flyoff_time);
			FTimerHandle TeleportAwayHandle;
			GetWorld()->GetTimerManager().SetTimer(TeleportAwayHandle, FTimerDelegate::CreateUObject(this, &UTeleportComponent::TeleportComplete
			), teleport_timeout, false);
		}

		PlayerCharacter()->PauseMontage(TeleportAnimation);
		PlayerCharacter()->JumpToSectionMontage(TEXT("TeleportFinish"), TeleportAnimation);
		PlayerCharacter()->ResumeMontage(TeleportAnimation);
		break;
	}
	case ETeleportState::Landing:
	{
		/// play the landing animation and delay the 'complete' STATE CHANGE
		PlayerCharacter()->PauseMontage(TeleportAnimation);
		PlayerCharacter()->JumpToSectionMontage(TEXT("TeleportLanding"), TeleportAnimation);
		PlayerCharacter()->ResumeMontage(TeleportAnimation);

		if (GetOwner()->HasAuthority()) {
			FTimerHandle TeleportIdleHandle;
			float teleport_landing_time = TeleportAnimation->GetSectionLength(TeleportAnimation->GetSectionIndex("TeleportLanding"));
			GetWorld()->GetTimerManager().SetTimer(TeleportIdleHandle, FTimerDelegate::CreateUObject(
				this, &UTeleportComponent::SetState, ETeleportState::Idle), FMath::Max(TeleportMinDelay, teleport_landing_time), false);
		}

		break;
	}

	} // end of switch(mTeleportState)

	// feedback BP
	OnTeleportStateChanged.Broadcast(mTeleportState);

}

void UTeleportComponent::TeleportFlyOff()
{
	SetState(ETeleportState::Flying);

}

void UTeleportComponent::TeleportComplete()
{
	if (PlayerCharacter()->IsAlive()) {
		SetState(ETeleportState::Landing);
	}
	else {
		SetState(ETeleportState::Idle);
	}

	if (!GetOwner()->HasAuthority() || (!mTeleportTargetPlayer && !mTeleportToCheckpoint))
		return;

	if (mTeleportToCheckpoint) {
		if (!TeleportAfterDeath()) {
			UE_LOG(LogDungeons, Error, TEXT("Reset teleport failed."));
		}

		mTeleportToCheckpoint = false;
	}
	else {
		const FVector& targetLocation = mTeleportTargetPlayer->GetActorLocation();


		if (!TeleportToPosition(targetLocation)) {
			UE_LOG(LogDungeons, Error, TEXT("Teleport failed %s"), *targetLocation.ToString());
		} 
		else {
			if(mTeleportTargetPlayer->GetElytraComponent()->IsEquipped())
			{
				PlayerCharacter()->GetElytraComponent()->Equip();
			}
		}
	}
}
