#include "Dungeons.h"
#include "game/Game.h"
#include "game/component/EquipmentComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include <Materials/MaterialParameterCollectionInstance.h>
#include <Materials/MaterialParameterCollection.h>
#include "game/mission/MissionDefs.h"
#include "game/util/ActorQuery.h"
#include "Engine/AssetManager.h"
#include "DungeonsGameState.h"
#include "DungeonsGameInstance.h"


namespace DungeonsQA {
	extern TAutoConsoleVariable<int32> CVInfiniteLife;
};

ADungeonsGameState::ADungeonsGameState(): Lives(0) {
	DevelopmentLevelSeed = FMath::Rand();
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	NetUpdateFrequency = 5.f;
	mLoadingCinematic = nullptr;

	const ConstructorHelpers::FObjectFinder<UMaterialParameterCollection> MaterialParameterCollectionFinder(TEXT("/Game/UI/Materials/HotBar2/SlotBig/GameTime"));
	if (MaterialParameterCollectionFinder.Succeeded()) {
		MaterialParameterCollection = MaterialParameterCollectionFinder.Object;
	}
}

void ADungeonsGameState::OnConstruction(const FTransform &transform) {
	Super::OnConstruction(transform);
}

void ADungeonsGameState::BeginPlay() {
	Super::BeginPlay();
}


void ADungeonsGameState::AddPlayerCharacter(APlayerCharacter* playerCharacter) {
	OnPlayerAdding.Broadcast(playerCharacter);
	playerCharacter->OnAliveStateChanged.AddUObject(this, &ADungeonsGameState::PlayerAliveStateChanged, playerCharacter);
	if (auto&& equipment = playerCharacter->GetEquipmentComponent()) {
		equipment->OnTotalEquippedItemPowerChangedInternal.AddUObject(this, &ADungeonsGameState::RefreshAverageTeamPower);
	}
	RefreshAverageTeamPower();
	OnPlayerAddedOrRemoved.Broadcast();
	OnPlayerAddedOrRemovedInternal.Broadcast();
}

void ADungeonsGameState::RemovePlayerCharacter(APlayerCharacter* playerCharacter) {
	mPlayersPendingRemoval.emplace_back(playerCharacter);	
	OnPlayerRemoving.Broadcast(playerCharacter);
}


void ADungeonsGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADungeonsGameState, DevelopmentLevelSeed);
	DOREPLIFETIME(ADungeonsGameState, IsGameOver);	
	DOREPLIFETIME(ADungeonsGameState, Lives);
	DOREPLIFETIME(ADungeonsGameState, LivesLostThisSession);
	DOREPLIFETIME(ADungeonsGameState, ReviveEnabled);
	DOREPLIFETIME(ADungeonsGameState, ShowLives);
	DOREPLIFETIME(ADungeonsGameState, AverageTeamPower);
	DOREPLIFETIME(ADungeonsGameState, PartsDiscovered);
}

bool ADungeonsGameState::GetShowLives() const {
	return ShowLives;
}

void ADungeonsGameState::OnRep_ShowLives() {
	OnShowLivesChanged.Broadcast();
}

void ADungeonsGameState::SetShowLives(bool areLivesLimited) {
	if (areLivesLimited != ShowLives) {
		ShowLives = areLivesLimited;
		OnRep_ShowLives();
	}
}

float ADungeonsGameState::GetAverageTeamPower() const
{
	return AverageTeamPower;
}

int32 ADungeonsGameState::GetLives() const {
	if (DungeonsQA::CVInfiniteLife.GetValueOnGameThread() != 0) {
		return FMath::Max(Lives, 1);
	}

	return Lives;
}

void ADungeonsGameState::OnRep_Lives() {
	OnLivesChanged.Broadcast();
}

int ADungeonsGameState::GetLivesLostThisSession() const {
	return LivesLostThisSession;
}

void ADungeonsGameState::OnRep_LivesLostThisSession() {
	OnLivesLostThisSessionChanged.Broadcast();
}

void ADungeonsGameState::OnRep_AverageTeamPower()
{
	OnAverageTeamPowerChanged.Broadcast();
}

void ADungeonsGameState::SetLives(const int newLives) {
	Lives = newLives;
	OnRep_Lives();
}

void ADungeonsGameState::SubtractLives(const int livesToSubtract) {
	check(livesToSubtract > 0 && "Attempts to subtract less than one life.");

	if (DungeonsQA::CVInfiniteLife.GetValueOnGameThread() != 0) {
		return;
	}

	SetLives(Lives - livesToSubtract);

	LivesLostThisSession += livesToSubtract;
	OnRep_LivesLostThisSession();
}


void ADungeonsGameState::OnRep_OnIsGameOver() {
	OnIsGameOver.Broadcast(IsGameOver);
}


void ADungeonsGameState::SetIsGameOver(const bool gameOver) {
	if (gameOver == IsGameOver) {
		return;
	}
	IsGameOver = gameOver;
	OnIsGameOver.Broadcast(IsGameOver);
}

void ADungeonsGameState::StartLoadingCinematic(FSoftObjectPath ClassPath, Placement placement, BlockPos originalPos)
{
	if (mCinematicClassLoading)
		return;

	//start async load cinematic class here
	mCinematicClassLoading = true;

	UWorld* world = GetWorld();
	TWeakObjectPtr<ADungeonsGameState> WeakThis = this;

	UAssetManager::GetStreamableManager().RequestAsyncLoad(ClassPath,
		[WeakThis, ClassPath, world, placement, originalPos]() {

		if (WeakThis.IsValid())
		{
			UClass* introClass = Cast<UClass>(ClassPath.ResolveObject());
			const auto cinematic = UCinematicSequence::Construct(*world, introClass, placement, originalPos);
			WeakThis->StartLoadCinematicInstance(cinematic);
		}

	}
	, FStreamableManager::AsyncLoadHighPriority);
}


bool ADungeonsGameState::PlayLoadedCinematic()
{
	if (!mCinematicClassLoading) {
		if (mLoadingCinematic && mLoadingCinematic->load()) {
			//cinematic all loaded
			StartCinematic(mLoadingCinematic);
			mLoadingCinematic = nullptr;
			return true;
		}
	}

	return false;
}

void ADungeonsGameState::StartLoadCinematicInstance(UCinematicSequence* cinematic)
{
	//we have a new cinematic class to load
	mCinematicClassLoading = false;

	mLoadingCinematic = cinematic;

	if (mLoadingCinematic) {
		mLoadingCinematic->load(); //start loading
	}
}


void ADungeonsGameState::StartCinematic(UCinematicSequence* cinematic) {
	if (mCinematic.IsValid()) {
		mCinematic.Get()->stop();
	}

	mCinematicClassLoading = false;

	check(cinematic);
	mCinematic = cinematic;
	if (mCinematic.IsValid()) {
		mCinematic.Get()->play();
	}

	OnCinematicSequenceChanged.Broadcast(cinematic);
}

void ADungeonsGameState::OnCinematicCannotPlay()
{
	mCinematicClassLoading = false;

	if (UDungeonsGameInstance* OurGameInstance = GetWorld()->GetGameInstance<UDungeonsGameInstance>()) {
		OurGameInstance->CheckMultiplayerFeatures();
	}
}

UCinematicSequence* ADungeonsGameState::GetCinematic() const {
	if (mCinematic.IsValid()) {
		return mCinematic.Get();
	}
	return nullptr;
}

bool ADungeonsGameState::IsCinematicPlaying() const {
	return IsCinematicLoading() || (GetCinematic() && GetCinematic()->isPlaying());
}

bool ADungeonsGameState::IsCinematicLoading() const
{
	return mCinematicClassLoading || (mLoadingCinematic && !mLoadingCinematic->load());
}

bool ADungeonsGameState::HasCinematicStartedLoading() const
{
	return mCinematicClassLoading || mLoadingCinematic;
}

int ADungeonsGameState::GetPlayersMatchingAliveStateCount(const EAliveState aliveState) const {
	auto match = 0;

	for (auto&& player : actorquery::getActors<APlayerCharacter>(GetWorld())) {
		if (player->GetAliveState() == aliveState) {
			++match;
		}
	}

	return match;
}

bool ADungeonsGameState::IsAnyPlayerMatchingAliveState(const EAliveState aliveState) const {
	return GetPlayersMatchingAliveStateCount(aliveState) >= 1;
}

void ADungeonsGameState::OnRep_ReviveEnabled() {
	OnRevivePossibleChanged.Broadcast();
	OnReviveEnabledChanged.Broadcast(ReviveEnabled);
}

void ADungeonsGameState::RefreshAverageTeamPower()
{
	if (HasAuthority()) {
		int totalDisplayPower = 0;
		int numTotal = 0;

		for (auto&& player : actorquery::getActors<APlayerCharacter>(GetWorld())) {
			if (auto&& equipment = player->GetEquipmentComponent()) {				
				totalDisplayPower += equipment->GetTotalEquippedDisplayItemPower();
				numTotal++;
			}
		}

		AverageTeamPower = (numTotal > 0 ? (float)totalDisplayPower / (float)numTotal : 0.0f);
		OnRep_AverageTeamPower();
	}
}

void ADungeonsGameState::OnRep_PartsDiscovered() {
	OnPartsDiscoveredChanged.Broadcast();
}

bool ADungeonsGameState::GetReviveEnabled() const {
	return ReviveEnabled;
};

bool ADungeonsGameState::GetIsGameOver() const {
	return IsGameOver;
};

void ADungeonsGameState::SetReviveEnabled(bool Enabled){
	if(Enabled != ReviveEnabled) {
		ReviveEnabled = Enabled;
		OnRep_ReviveEnabled();
	}
}

void ADungeonsGameState::PlayerAliveStateChanged(APlayerCharacter* player) const {
	if (player->GetAliveState() == EAliveState::Down) {
		OnAnnouncePlayerDown.Broadcast(player);
	}

	OnRevivePossibleChanged.Broadcast();
}

bool ADungeonsGameState::IsPendingRemoval(APlayerCharacter* player) const {
	if (!player) {
		return false;
	}

	for (auto& playerPendingRemoval : mPlayersPendingRemoval) {
		if (playerPendingRemoval != nullptr && playerPendingRemoval.IsValid()) {
			if (playerPendingRemoval.Get() == player) {
				return true;	
			}			
		}		
	}

	return false;
}

int ADungeonsGameState::GetPartsDiscovered() const {
	return PartsDiscovered;
}

void ADungeonsGameState::SetPartsDiscovered(int partsDiscovered) {
	if (PartsDiscovered != partsDiscovered) {
		PartsDiscovered = partsDiscovered;
		OnRep_PartsDiscovered();
	}
}

void ADungeonsGameState::Tick(float deltaSeconds) {
	Super::Tick(deltaSeconds);
	if (!mPlayersPendingRemoval.empty()) {
		UpdatePlayersPendingRemoval();
	}
	if (MaterialParameterCollection && GetWorld()) {
		if(materialParameterCollectionInstance.IsValid()) {
			materialParameterCollectionInstance->SetScalarParameterValue(FName(TEXT("GameTime")), GetWorld()->GetTimeSeconds());
		} else {
			materialParameterCollectionInstance = GetWorld()->GetParameterCollectionInstance(MaterialParameterCollection);
		}
	}	
}

bool ADungeonsGameState::CanAnyoneReviveMe(APlayerCharacter* me) const {
	//Allowed / Disallowed
	if (!ReviveEnabled) {
		return false;
	}

	//Could it happen?
	const bool RevivePossibleInTheory = [&]() {
				
		for (auto&& player : actorquery::getActors<APlayerCharacter>(GetWorld())) {
			if (player != me && 
				(player->GetAliveState() == EAliveState::Alive || player->GetAliveState() == EAliveState::Reviving)) {
				return true;
			}
		}
		return false;
	}();

	return RevivePossibleInTheory;
}

void ADungeonsGameState::UpdatePlayersPendingRemoval() {
	for (auto index = 0; index < mPlayersPendingRemoval.size(); index++) {
		auto& playerPendingRemoval = mPlayersPendingRemoval[index];
		if (playerPendingRemoval == nullptr || !playerPendingRemoval.IsValid() || !IsPlayerInWorld(playerPendingRemoval.Get())) {
			OnPlayerRemoved.Broadcast();
			OnRevivePossibleChanged.Broadcast();
			RefreshAverageTeamPower();
			mPlayersPendingRemoval.erase(mPlayersPendingRemoval.begin() + index);
			OnPlayerAddedOrRemoved.Broadcast();
			OnPlayerAddedOrRemovedInternal.Broadcast();
		}
	}
}

bool ADungeonsGameState::IsPlayerInWorld(APlayerCharacter* player) const {
	for (auto&& playerInWorld : actorquery::getActors<APlayerCharacter>(GetWorld())) {
		if (playerInWorld == player) {
			return true;
		}
	}
	return false;
}

bool ADungeonsGameState::IsMultiplayer() const {
	return PlayerArray.Num() - mPlayersPendingRemoval.size() > 1;
}
