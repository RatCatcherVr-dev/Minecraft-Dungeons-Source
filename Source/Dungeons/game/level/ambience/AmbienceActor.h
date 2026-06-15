#pragma once

#include "GameFramework/Actor.h"
#include "Ambience.h"

#include "AmbienceActor.generated.h"

class APlayerCharacter;

UCLASS()
class DUNGEONS_API AAmbienceActor : public AActor {
	GENERATED_BODY()

public:
	void SetActive(bool active);
	void SetHidden(bool hidden);

	const TArray<FString>& GetAttachedNames() const;

	//When creating actor from blueprint we want to ignore all actor editor state and make sure ambience starts hidden
	//When playin in editor this is not call and we retain editor active ambience actor
	void forceClearActiveAndHideAll();

	void PlayerExited(APlayerCharacter*);
	void PlayerEntered(APlayerCharacter*);

	void SetAttachedActorNames(TArray<AActor*> childActors);
	void SetGroupName(FString groupName);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FString& GetGroupName() const;
protected:
	
	void SetAllAttachedActorVisibility(bool visibility) const;
	float GetRevealRadiusScale() const;
private:
	bool ShouldBeVisible() const;
	void SetAttachedActorsVisible(bool visible);
	void OnActivated() const;
	void OnDeactivated() const;
	bool bIsActive = false;
	bool bIsHidden = false;

	bool bAreAttachedActorsVisible = false;

	UPROPERTY(EditAnywhere, SaveGame)
	TArray<FString> attachedNames;

	UPROPERTY(EditAnywhere, SaveGame)
	float revealRadiusScale = 1.f;

	FString mGroupName;
};

UCLASS()
class DUNGEONS_API AAmbienceVisualActor : public AAmbienceActor {
	GENERATED_BODY()

public:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool Matches(const FAmbienceIDGroup&) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EAmbienceID GetAmbienceID() const;

	UFUNCTION(CallInEditor, Category = "Dungeons")
	void SetAsActiveAmbience() const;

	UPROPERTY(AssetRegistrySearchable)
	FName Id;

	void PreSave(const ITargetPlatform* TargetPlatform) override;
private:
	UPROPERTY(EditAnywhere, SaveGame)
	EAmbienceID AmbienceID;

};

UCLASS()
class DUNGEONS_API AAmbienceAudioActor : public AAmbienceActor {
	GENERATED_BODY()

public:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool Matches(const FAmbienceAudioIDGroup&) const;

	EAmbienceAudioID GetAmbienceID() const;

	UPROPERTY(AssetRegistrySearchable)
	FName Id;

	void PreSave(const ITargetPlatform* TargetPlatform) override;
	
private:
	UPROPERTY(EditAnywhere, SaveGame)
	EAmbienceAudioID AmbienceAudioID;
};
