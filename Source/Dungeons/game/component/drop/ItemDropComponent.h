#pragma once

#include "CoreMinimal.h"
#include "DungeonsGameInstance.h"
#include "Components/ActorComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/item/ItemRarityChance.h"
#include "ItemDropComponent.generated.h"

/**
 * Component that generates item drops. 
 * If added to a MobCharacter and bApplyAutomatically is set to true an item drop will automatically be triggered when the mob dies.
 * If added to a LootActor and bApplyAutomatically is set to true an item drop will automatically be triggered when the loot is opened.
 */
UCLASS(ClassGroup=(Custom))
class DUNGEONS_API UItemDropComponent : public UActorComponent {
	GENERATED_BODY()

public:	
	UItemDropComponent();

	void BeginPlay() override;

	UFUNCTION()
	void TearDownComponent();
	
	void OnAttributeHealthChange(const FOnAttributeChangeData& data);

	/**
	 * Trigger an item drop.
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation, Category = "Dungeons")
	void ServerDropItems(const FVector& spawnLocation, AActor* triggeringPlayer, AActor* source);

	bool ShouldApplyAutomatically() const { return bApplyAutomatically; };

	static void GenerateDropsForPlayer(UWorld* world, AActor* targetPlayer, AActor* sourceActor, const FItemDropSource& dropSource, const FNetworkedItemDropData& dropData, bool replicateItems, bool elongatedDrops, float dropDelay, float dropDelayCountSpeedUpFactor, EItemRarityChanceCategory rarityChanceCategory);
protected:
	virtual void OnMobKilled(const FOnAttributeChangeData& data);
	virtual void OnMobDamaged(const FOnAttributeChangeData& data);

	virtual TArray<FNetworkedItemDropData> GatherItemDropData(const FItemDropSource& dropSource);

	static TArray<FInventoryItemData> GenerateDropData(UWorld* world, const FItemDropSource& dropSource, const FNetworkedItemDropData& dropData, AActor* targetPlayer, EItemRarityChanceCategory rarityChanceCategory);
	void DropItems(const FItemDropSource& dropSource, const FNetworkedItemDropData& dropData, AActor* source);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	bool bEnableDrops;

	/** Adds delay between drops when dropping multiple items at once */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	bool bElongatedDrops;

	/** Seconds between each elongated drop */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons", meta = (EditCondition = "bElongatedDrops", ClampMin = "0.0", ClampMax = "3.0"))
	float ElongatedDropDelay;

	/** Speeds up delay between each elongated drop if the count is higher */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons", meta = (EditCondition = "bElongatedDrops", ClampMin = "0.0", ClampMax = "1.0"))
	float ElongatedDropDelayCountSpeedUpFactor;

	/** Automatically drop items on mob death / loot opened */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	bool bApplyAutomatically;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	EItemRarityChanceCategory RarityChanceCategory;

private:
	float RarityChanceProbabilityMultiplier = 1.0f;

	static void GenerateElongatedDrops(UWorld* world, float dropDelay, float dropDelayCountSpeedUpFactor, const FItemDropSource& dropSource, const FNetworkedItemDropData& dropData, AActor* targetPlayer, AActor* sourceActor, bool bReplicateItems);
	static bool IsDropAllowedFrom(AActor* source);

	static AActor* GetActorCreditedForKill(AActor* instigator);

#if WITH_EDITOR
	void DebugDumpItemDrops( int count, const FItemDropSource& dropSource );
#endif
};
