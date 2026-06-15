#include "Dungeons.h"
#include "LobbyChest.h"
#include "game/Game.h"
#include "DungeonsGameInstance.h"
#include "game/component/InteractableComponent.h"
#include "game/component/LootUnlockerComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <Animation/AnimComposite.h>
#include <Animation/AnimSequenceBase.h>
#include "game/component/WalletComponent.h"
#include "game/component/MissionProgressComponent.h"
#include "game/component/KillTrackerComponent.h"
#include "game/component/CharacterSerializeComponent.h"
#include "Engine/LocalPlayer.h"

ALobbyChest::ALobbyChest() 
	: ChestLootType(ELobbyChestLootType::RandomItem)
	, mLockingDelay(0.0f)
	, bLockingDelayed(false)
{
	bAlwaysRelevant = true;
	NetUpdateFrequency = 10.0f;
	MinNetUpdateFrequency = 1.0f;

	//We will need tick later when closing re-usable chests
	PrimaryActorTick.bCanEverTick = true;

	//no need to tick this while idle
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	bLootUnlocked = false;
	RootSceneComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootSceneComponent->SetMobility(EComponentMobility::Static);

	SetRootComponent(RootSceneComponent);

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	InteractableComponent->SetType(EClickyEnum::CE_LobbyChest);
	InteractableComponent->SetBalloontipBehavior(EBalloonTipBehavior::Hidden);
	InteractableComponent->AttachTo(RootSceneComponent);
}

void ALobbyChest::BeginPlay() {
	Super::BeginPlay();

	MeshComp = FindComponentByClass<USkeletalMeshComponent>();

	InteractableComponent->OnInteracted.AddDynamic(this, &ALobbyChest::HandleInteraction);

	if (GetTargetState() == OpenState::Open) {
		SetUnlocked();
	}
}

void ALobbyChest::HandleInteraction(class ACharacter* instigator) {
	if (const auto* controller = instigator->GetController()) {
		if (auto* lootUnlocker = controller->FindComponentByClass<ULootUnlockerComponent>()) {
			lootUnlocker->RequestUnlock(this);
		}
	}
}

void ALobbyChest::Tick(float DeltaSecs) {	
	if (bLockingDelayed) {
		mLockingDelay -= DeltaSecs;
		if (mLockingDelay <= 0.0) {
			bLockingDelayed = false;
			SetLocking();
		}
	}
	else
	{
		SetActorTickEnabled(false);
	}
}

EItemRarityChanceCategory ALobbyChest::GetItemRarityChanceCategory() {
	return EItemRarityChanceCategory::LobbyChest;
}

bool ALobbyChest::CanBeUnlockedBy(const APlayerCharacter* character) const {
	if (character) {
		return 	IsPlayerLevelRequirementMet(character) &&
				IsMissionCompletionRequirementMet(character) &&
				IsMobKillsRequirementMet(character) &&
				IsEmeraldsRequirementMet(character) &&
				(UnlockRequirements.canBeReused || GetChestUnlockedTimesForPlayer(character) == 0)
			;
	}
	return false;
}

bool ALobbyChest::IsPlayerLevelRequirementMet(const APlayerCharacter* character) const {
	const auto requiredLevel = UnlockRequirements.playerLevel;
	
	if (requiredLevel == -1) {
		return true;
	}
	
	return requiredLevel <= character->GetCharacterLevel();
}

bool ALobbyChest::IsMissionCompletionRequirementMet(const APlayerCharacter* character) const {
	if (const auto* missionProgressComponent = character->FindComponentByClass<UMissionProgressComponent>()) {
		return missionProgressComponent->IsFulfilled(UnlockRequirements.CompletedLevel);
	}
	
	return false;
}

bool ALobbyChest::IsEmeraldsRequirementMet(const APlayerCharacter* character) const {
	if (UnlockRequirements.NumberOfGems == -1) {
		return true;
	}

	if (auto* wallet = character->FindComponentByClass<UWalletComponent>()) {
		return GetEmeraldUnlockCost(character) <= wallet->GetEmeraldBalance();
	}

	return false;
}

bool ALobbyChest::IsMobKillsRequirementMet(const APlayerCharacter* character) const {
	const auto mobKillsCompletion = UnlockRequirements.CompletedMobKills;
	
	if (mobKillsCompletion.KillCount == -1) {
		return true;
	}

	if (auto* KillTrackerComponent = character->FindComponentByClass<UKillTrackerComponent>()) {
		return mobKillsCompletion.KillCount <= KillTrackerComponent->GetNumKillsInCategory(mobKillsCompletion.MobEntityTypeName);
	}

	return false;
}

int32 ALobbyChest::GetChestUnlockedTimesForPlayer(const APlayerCharacter* character) const {
	if (character) {
		if (auto characterSerializer = character->GetCharacterSerializeComponent()) {
			return characterSerializer->GetChestUnlockedTimes(UnlockRequirements.lootID);
		}
	}
	return 0;
}

ALobbyChest::OpenState ALobbyChest::GetTargetState() const {
	if (UnlockRequirements.canBeReused) {
		return OpenState::Closed;
	}
	
	for (auto&& localPlayer : GetGameInstance()->GetLocalPlayers()) {
		if (auto controller = localPlayer->GetPlayerController(GetWorld())) {
			if (GetChestUnlockedTimesForPlayer(Cast<APlayerCharacter>(controller->GetPawn())) == 0) {
				return OpenState::Closed;
			}
		}
	}
	return OpenState::Open;
}

int32 ALobbyChest::GetEmeraldUnlockCost(const APlayerCharacter* character) const {
	if (UnlockRequirements.NumberOfGems < 0) {
		return 0;
	}

	float shopCostMultiplier = 1.0;
	if (const auto* game = actorquery::getGame(GetWorld())) {
		shopCostMultiplier = game->settings().difficultyStats.GetShopPriceMultiplier();
	}
	return UnlockRequirements.GetEmeraldCost() * shopCostMultiplier;
}

void ALobbyChest::FailedToUnlock() {
	PlayAnimation(unlockFailedAnim);
}

void ALobbyChest::PlayAnimation(UAnimSequenceBase* animation) const {
	if (animation && MeshComp) {
		if (MeshComp->GetAnimationMode() == EAnimationMode::AnimationBlueprint) {
			auto* animInstance = MeshComp->GetAnimInstance();
			animInstance->PlaySlotAnimationAsDynamicMontage(animation, AnimationSlot);
		}
		else {
			MeshComp->PlayAnimation(animation, false);
		}
	}
}

void ALobbyChest::SetUnlocked() {
	PlayAnimation(unlockedAnim);
	
	bLootUnlocked = true;
	InteractableComponent->DisableInteraction();

	if (UnlockRequirements.canBeReused) {
		SetDelayedLocking();
	}
}

void ALobbyChest::UnlockedBy(const APlayerCharacter* character) {
	if (!GetGameInstance()->IsDedicatedServerInstance()) {
		auto* characterSerializer = character->GetCharacterSerializeComponent();

		characterSerializer->ChestUnlocked(UnlockRequirements.lootID);
	}

	PlayAnimation(unlockingAnim);
	bLootUnlocked = true;
	InteractableComponent->DisableInteraction();

	if (UnlockRequirements.canBeReused || GetTargetState() == OpenState::Closed) {
		SetDelayedLocking();
	}
}

void ALobbyChest::SetLocking() {
	PlayAnimation(lockingAnim);
	
	bLootUnlocked = false;
	InteractableComponent->EnableInteraction();
}

void ALobbyChest::SetDelayedLocking() {
	bLockingDelayed = true;
	mLockingDelay = DelayedUnlockTime;
	SetActorTickEnabled(true);
}
