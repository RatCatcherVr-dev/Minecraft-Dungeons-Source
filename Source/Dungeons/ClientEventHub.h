#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/item/SerializableItemId.h"
#include "world/entity/EntityTypes.h"
#include "game/component/AwardsGeneratorComponent.h"
#include "ClientEventHub.generated.h"

UCLASS()
class DUNGEONS_API UClientEventHubComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UClientEventHubComponent();

	void UseItem(const FSerializableItemId& itemId) const;
	void OpenChest() const;
	void XPChanged(int amount) const;
	void ReviveFriend() const;
	void ReceivedCurrency(const FSerializableItemId& type, int amount) const;
	void KilledMob(const EntityType mobType, bool enchanted) const;
	void Moved(float deltaSeconds, const FVector& oldLocation, const FVector& oldVelocity) const;
	void MissionFinished(ELevelNames name, EGameDifficulty difficulty, FEndlessStruggle endlessStruggle, EExtraChallenge extraChallenge, bool hyperMission, ELevelVariationType levelVariation, int collectedTokens) const;

private:
	UFUNCTION(Client, Unreliable)
	void Client_DoUseItem(const FSerializableItemId& itemId) const;
	void DoUseItem(const FSerializableItemId& itemId) const;

	UFUNCTION(Client, Unreliable)
	void Client_DoOpenChest() const;
	void DoOpenChest() const;

	template <typename WhenLocallyControlled, typename WhenNonLocallyControlled>
	void Execute(WhenLocallyControlled localAction, WhenNonLocallyControlled nonLocalAction) const;

	template <typename StatTrackerCall>
	void TrackStat(StatTrackerCall call) const;

	template <typename LiveOpsCall>
	void TrackLiveOps(LiveOpsCall call) const;

	bool OwnerIsLocallyControlled() const;
	bool OwnerHasAuthority() const;
	bool ShouldTrackLiveOpsProgress() const;
	bool OwnerIsPrimaryLocalPlayer() const;
	ABaseCharacter* GetBaseCharacterOwner() const;
};
