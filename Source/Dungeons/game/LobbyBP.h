#pragma once

#include "game/GameBP.h"
#include "LobbyBP.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTriggerAnnouncement, EGameDifficulty, DifficulyUnlocked, EEndGameContentType, EndGameContentUnlocked);

UCLASS(Blueprintable)
class ALobbyBP : public AGameBP {
	GENERATED_BODY()
public:
	ALobbyBP();
	~ALobbyBP() = default;
	
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	const FObjectiveLocations& GetObjectiveLocations() override;

	void AddUnlockableChestObjective(FVector Location);
	void RemoveUnlockableChestObjective(FVector Location);

	UFUNCTION(BlueprintCallable)
	bool QuitToMenuAfterRewardScreen() const;

	// Debug trigger
	UPROPERTY(BlueprintAssignable, Category="Dungeons")
	FOnTriggerAnnouncement OnTriggerAnnouncement;

	bool IsIngame() override { return false; }
protected:
	FLevelSettings CreateLevelSettings() const override;
	
	UPROPERTY(BlueprintReadWrite)
	bool AllStartWidgetsAreClosed = false;

private:
	bool HasLocalGameplayStarted() override;

	FObjectiveLocations UnlockableChestLocations;
	int32 counter = 0;
};
