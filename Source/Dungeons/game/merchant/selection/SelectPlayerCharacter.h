#pragma once
#include "MerchantSelectionBase.h"
#include "SelectPlayerCharacter.generated.h"

class APlayerCharacter;

UCLASS(BlueprintType)
class DUNGEONS_API USelectPlayerCharacter : public UMerchantSelectionBase {
	GENERATED_BODY()

protected:
	UPROPERTY()
	APlayerCharacter* mSelectedPlayerCharacter;

	virtual bool IsPlayerCharacterSelectable(APlayerCharacter* playerCharacter) const;

	void OnSetupSession() override;
	void OnCleanupSession() override;
	
	void OnPlayerAddedOrRemoved();

public:
	void EnsureSelection() override;
	void ClearSelection() override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	APlayerCharacter* GetPlayerCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<APlayerCharacter*> GetSelectablePlayerCharacters() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SelectPlayerCharacter(APlayerCharacter* playerCharacter);
	
	bool CanSelectAny() const override;	
	bool HasSelectedAny() const override;	
};

