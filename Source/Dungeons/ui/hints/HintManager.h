#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "game/UIHintDefs.h"
#include "game/Game.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/trigger/Triggers.h"
#include "ui/hints/UIHintType.h"
#include "HintManager.generated.h"

class AMissionProgressHandler;
class UItemSlot;
enum class ELobbyChestType : uint8;

// Blueprint delegates for UI
UENUM(BlueprintType)
enum class EVisibilityReason : uint8 {
	Activated,
	Completed,
	Forgot
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUIHintEvent, EUIHintType, HintType, EVisibilityReason, Reason);

// should be a component on the PlayerCharacter so each player can have its progress.
UCLASS()
class DUNGEONS_API UHintManager : public UObject
{
	GENERATED_BODY()
		
public:
	void LevelLoaded(const game::Game&);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnUIHintEvent OnUIHintVisibility;

	bool IsHintVisible(EUIHintType) const;

	void InitHintsUserEnabled(bool Enabled);
	void SetHintsUserEnabled(bool Enabled);

private:

	void ResetInitializers(const game::Game&, TArray<ABasePlayerController*>, TArray<APlayerCharacter*>);
	void SetupHintListeners(const game::Game&, ELevelNames, const TArray<APlayerCharacter*>&);

	void SaveHintExpired(EUIHintType);

	UFUNCTION()
	void OnObjectiveUpdated(const FText& Name, const FString& Id);
	void OnItemPickedUp(const FItemId& itemType, ABasePlayerController*);

	void OnPlayerMovement(const ABasePlayerController*);
	void OnRangedAttack(const APlayerCharacter*);	
	void OnDamageReceived(float dealtDamage, UHealthComponent*);

	void OnPopped(APlayerCharacter*);
	void OnPlayerCharacterAdded(APlayerCharacter*);
	void OnPlayerCharacterRemoved();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnInventoryOpened(ABasePlayerController* playerController);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnInventoryClosed(ABasePlayerController* playerController);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnEnchantmentSelected();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnMissionSelectOpened();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnMapToggled();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnInventoryFilterChanged(const FString& itemFilterMode);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnChatWheelVisibilityChanged(bool visible);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnChatWheelOptionSelected();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnChatWheelMessageSent();

	void OnSlotUpdated(UItemSlot* itemSlot) const;
	UFUNCTION()
	void OnSlotActivated(UItemSlot* itemSlot, bool outcome);
	UFUNCTION()
	void OnAvailableEnchantmentPointsChanged(int NewAmount, UItemStashComponent* itemStashComponent);
	void OnInventoryItemSlotSelected(const UInventoryItemSlot*);
	void OnLobbyChestOpened(ELobbyChestType);
	void OnLobbyChestClosed(ELobbyChestType, bool success);
	UFUNCTION()
	void OnStrongholdSecretLootFound();
	UFUNCTION()
	void OnStrongholdLockedSubDungeonDoor();
	void OnStrongholdEndPortalInteration(const StrongholdProgressSaveData* strongholdProgress);
	void OnMerchantItemReserved();

	UFUNCTION()
	void OnLevelUp(int32 newLevel);

	void OnPlayersFarAway();
	UFUNCTION()
	void OnTeleportPressed();

	bool ShowHint(EUIHintType);
	void ShowHintTimed(EUIHintType, std::chrono::milliseconds);
	void ShowHintTimedAndComplete(EUIHintType, std::chrono::milliseconds);
	void HideHintIfVisible(EUIHintType);
	void CompleteHintIfVisible(EUIHintType);
	void CompleteHint(EUIHintType);
	bool IsCompleted(EUIHintType) const;
	void ResetCompletion(EUIHintType);

	// D11.DJB
	void HideAllHints();
	bool mUserEnabledHints = true;

	bool mHintSaveStateDirty;

	template <typename CallbackType>
	class LazyListener {
	public:
		LazyListener() {}

		LazyListener(std::function<void()> initFunction)
			: InitFunction(std::move(initFunction))
		{
		}

		void AddListener(std::function<CallbackType> listener) {
			if (!Initialized && InitFunction) {
				Initialized = true;
				InitFunction();
			}

			Listeners.push_back(std::move(listener));
		}

		const std::vector<std::function<CallbackType>>& AllListeners() const {
			return Listeners;
		}

	private:
		bool Initialized = false;
		std::function<void()> InitFunction;
		std::vector<std::function<CallbackType>> Listeners;
	};

	LazyListener<void(const FString& objectiveId)> ObjectiveListeners;
	LazyListener<void()> PlayerMovementListeners;
	LazyListener<void(const FItemId&, ABasePlayerController*)> ItemPickedUpListeners;
	LazyListener<void()> RangedAttackListeners;
	LazyListener<void(float dealtDamage, const UHealthComponent*)> DamageReceivedListeners;
	LazyListener<void(UItemSlot*)> ItemEquippedListeners;
	LazyListener<void(ABasePlayerController*)> InventoryOpenedListeners;
	LazyListener<void(ABasePlayerController*)> InventoryClosedListeners;
	LazyListener<void(UItemSlot*)> ItemActivatedListeners;
	LazyListener<void(const UInventoryItemSlot*)> GearSelectedListener;
	LazyListener<void()> EnchantmentSelectedListener;
	LazyListener<void()> ItemEnchantedListener;
	LazyListener<void()> MissionSelectOpened;
	LazyListener<void(ELobbyChestType)> LobbyChestOpened;
	LazyListener<void(ELobbyChestType, bool success)> LobbyChestClosed;
	LazyListener<void()> StrongholdSecretLootFoundListener;
	LazyListener<void()> StrongholdSubDungeonDoorInteractionListener;
	LazyListener<void(const StrongholdProgressSaveData*)> StrongholdEndPortalInteractionListener;
	LazyListener<void()> DeadEndEntered;
	LazyListener<void()> MapToggled;
	LazyListener<void(const FString& itemFilterMode)> InventoryFilterChanged;
	LazyListener<void()> FarFromPlayers;
	LazyListener<void()> TeleportActivated;
	LazyListener<void(int32 newLevel)> LevelUpListeners;
	LazyListener<void(APlayerCharacter*)> PoppingListeners;
	LazyListener<void(bool visible)> ChatWheelVisibilityChangedListeners;
	LazyListener<void()> ChatWheelOptionSelected;
	LazyListener<void()> ChatWheelMessageSent;
	LazyListener<void(APlayerCharacter*)> ServerPlayerAdded;
	LazyListener<void()> ServerPlayerRemoved;
	LazyListener<void()> MerchantItemReserved;

	struct FHintState {
		bool visible = false;
		bool completed = false;
	};
	std::unordered_map<EUIHintType, FHintState> HintStates;
};
