#pragma once
#include <ObjectMacros.h>
#include <Interface.h>
#include "PlayerCharacterDataProvider.generated.h"

UINTERFACE(Category = "Dungeons", BlueprintType)
class DUNGEONS_API UPlayerCharacterDataProvider : public UInterface {
	GENERATED_BODY()
};

class DUNGEONS_API IPlayerCharacterDataProvider {
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dungeons")
	int32 GetLevel() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dungeons")
	int32 GetTotalEquippedGearPower() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dungeons")
	int32 GetOwnedEmeralds() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dungeons")
	int32 GetOwnedGold() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dungeons")
	FText GetCharacterName() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dungeons")
	FName GetCharacterSkinId() const;
};