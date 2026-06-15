#pragma once

#include "CoreMinimal.h"
#include "game/item/ItemType.h"
#include "LootDefs.h"
#include "game/item/ItemRarityChance.h"
#include "game/item/SerializableItemId.h"
#include "LobbyChest.generated.h"

class ULootUnlockerComponent;
class APlayerCharacter;

UENUM(BlueprintType)
enum class ELobbyChestType : uint8 {
	UNSET,
	Chest,
	Blacksmith,
	Librarian,
	Villager,
	TravelingMerchant
};

UCLASS()
class DUNGEONS_API ALobbyChest : public AActor {
	GENERATED_BODY()

public:
	ALobbyChest();	

	void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	ELobbyChestType ChestType = ELobbyChestType::Chest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	int EmeraldsReward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	FSerializableItemId StaticRewardItemId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float StaticRewardItemPower = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	ELobbyChestLootType ChestLootType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UAnimSequenceBase* unlockingAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UAnimSequenceBase* lockingAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UAnimSequenceBase* unlockedAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UAnimSequenceBase* lockedAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UAnimSequenceBase* unlockFailedAnim;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	class USceneComponent* RootSceneComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UInteractableComponent* InteractableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	FUnlockRequirements UnlockRequirements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float DelayedUnlockTime = 2.f;

	UPROPERTY(EditDefaultsOnly)
	FName AnimationSlot = FName(TEXT("DefaultSlot"));

	void UnlockedBy(const APlayerCharacter* character);

	bool CanBeUnlockedBy(const APlayerCharacter* character) const;

	void Tick(float DeltaSecs) override;

	static EItemRarityChanceCategory GetItemRarityChanceCategory();

	int32 GetEmeraldUnlockCost(const APlayerCharacter* character) const;

	void FailedToUnlock();

private:
	void SetUnlocked();
	void SetLocking();
	void SetDelayedLocking();

	UFUNCTION()
	void HandleInteraction(class ACharacter* interactor);

	bool IsPlayerLevelRequirementMet(const APlayerCharacter* character) const;
	bool IsEmeraldsRequirementMet(const APlayerCharacter* character) const;
	bool IsMissionCompletionRequirementMet(const APlayerCharacter* character) const;
	bool IsMobKillsRequirementMet(const APlayerCharacter* character) const;

	int32 GetChestUnlockedTimesForPlayer(const APlayerCharacter* character) const;
	
	enum class OpenState {
		Open,
		Closed
	};
	OpenState GetTargetState() const;

	void PlayAnimation(UAnimSequenceBase* animation) const;

	UPROPERTY()
	class USkeletalMeshComponent* MeshComp;
	
	UPROPERTY() //This is now a local property - only determines if it looks unlocked on this client
	bool bLootUnlocked;

	float mLockingDelay;
	bool bLockingDelayed;
};
