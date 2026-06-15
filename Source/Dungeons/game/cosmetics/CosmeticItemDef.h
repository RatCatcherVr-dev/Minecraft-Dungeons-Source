#pragma once

#include "CoreMinimal.h"
#include "CosmeticsEntry.h"
#include "online/entitlements/Entitlement.h"
#include "CosmeticType.h"
#include "game/actor/cosmetics/CosmeticItemInfo.h"
#include "CosmeticItemDef.generated.h"

class UAnimSequenceBase;
class USoundCue;
class UTexture2D;

UCLASS(BlueprintType)
class DUNGEONS_API UCosmeticItemDef : public UObject {
	
	GENERATED_BODY()

	UCosmeticItemDef();

public:
	static UCosmeticItemDef* CreateDefinition(const FName& dataName, FCosmeticsEntry* dataEntry);
	static UCosmeticItemDef* CreateDefinition(const FName& dataName, const FCosmeticsEntry& dataEntry);
	static TArray<UCosmeticItemDef*> CreateDefinitions(const TMap<FName, FCosmeticsEntry>& dataEntries);

	bool IsCorrectType(const ECosmeticType& type) const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FString GetEntitlementName() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	ECosmeticType GetCosmeticType() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TSubclassOf<ACosmeticItemInfo> GetBlueprintClass() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FName& GetCosmeticId() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FText& GetCosmeticDisplayName() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FText& GetCosmeticDisplayDescription() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UTexture2D* GetCosmeticIconTexture() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	USoundCue* GetCosmeticEquippedSound() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UAnimSequenceBase* GetCosmeticEquippedAnimation() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetCosmeticInventoryRotation() const;
private:	
	FName DataName;
	TOptional<FCosmeticsEntry> DataEntry;
};