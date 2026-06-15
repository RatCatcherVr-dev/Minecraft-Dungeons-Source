#pragma once

#include "Components/ActorComponent.h"
#include "game/difficulty/Difficulty.h"
#include "DungeonsGameInstance.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/actor/character/loot/LobbyChest.h"
#include "LootUnlockerComponent.generated.h"

class ALobbyChest;

// add more UI callbacks if adding LobbyChestUnlockTypes to the type enum
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnUnlockRequestedStatic, ELobbyChestType, chestType, struct FUnlockRequirements, unlockRequirements, struct FChestDynamicProperties, properties, struct FItemMetaData, metaData, bool, canBeUnlocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnUnlockRequestedRandomItem, ELobbyChestType, chestType, struct FUnlockRequirements, unlockRequirements, struct FChestDynamicProperties, properties, bool, canBeUnlocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnUnlockRequestedRandomGear, ELobbyChestType, chestType, struct FUnlockRequirements, unlockRequirements, struct FChestDynamicProperties, properties, bool, canBeUnlocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUnlockRequestedEmeralds, ELobbyChestType, chestType, struct FUnlockRequirements, unlockRequirements, struct FChestDynamicProperties, properties);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnlockRequested, ELobbyChestType);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUnlockCompleted, ELobbyChestType, bool);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFailedToOpenChest);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnlockFinished);

class ALobbyActor;
class ALootActor;
class ABasePlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API ULootUnlockerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULootUnlockerComponent();

	void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Configuration")
	TSubclassOf<class UUserWidget> LockedLootWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Configuration")
	TSubclassOf<class UUserWidget> UnlockableLootWidgetClass;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnUnlockRequestedStatic OnUnlockRequestedStatic;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnUnlockRequestedRandomItem OnUnlockRequestedRandomItem;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnUnlockRequestedRandomGear OnUnlockRequestedRandomGear;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnUnlockRequestedEmeralds OnUnlockRequestedEmeralds;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnFailedToOpenChest OnFailedToOpenChest;
	FOnUnlockRequested OnAnyUnlockRequested;
	FOnUnlockCompleted OnAnyUnlockCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnUnlockFinished OnUnlockFinished;

	UFUNCTION()
	void RequestUnlock(ALobbyChest* chest);
	
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Dungeons")
	void ServerRequestOpen(ALootActor* chest);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Accept();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Reject();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int32 GetNumCompletedMissionsAtDifficulty(EGameDifficulty difficulty) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool GetHasCompletedMissionAtDifficulty(EGameDifficulty difficulty, ELevelNames levelName) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int32 GetNumMobKills(const FString& MobEntityType) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int32 GetEmeraldCount() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int32 GetCharacterLevel() const;

private:
	struct Loot {
		TOptional<FInventoryItemData> item;
		int numEmeralds = 0;
	};
	void GiveLoot() const;
	Loot GetLoot() const;
	TOptional<FInventoryItemData> GetRandomLoot(const game::item::generator::Pred& predicate) const;
	void UnlockRequested() const;
	void UnlockChest();
	bool TryConsumeEmeralds();
	TOptional<FInventoryItemData> GenerateStaticLoot(const ALobbyChest& chest) const;

	ABasePlayerController* GetPlayerController() const;
	APlayerCharacter* GetPlayerCharacter() const;

	UPROPERTY(Transient)
	TWeakObjectPtr<ALobbyChest> mChest;
};
