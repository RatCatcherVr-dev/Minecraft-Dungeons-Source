#pragma once

#include "CoreMinimal.h"
#include "SkinEntry.h"
#include "online/entitlements/Entitlement.h"
#include "SkinDef.generated.h"

class UMaterialInstance;

UCLASS(BlueprintType)
class DUNGEONS_API USkinDef : public UObject {
	
	GENERATED_BODY()

	USkinDef();

public:
	static USkinDef* CreateDefinition(const FName& dataName, FSkinEntry* dataEntry);
	static USkinDef* CreateDefinition(const FName& dataName, const FSkinEntry& dataEntry);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FString GetEntitlementName() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FName& GetSkinId() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UMaterialInstance* GetAvatarMaterial() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UMaterialInstance* GetPortraitMaterial() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TSoftObjectPtr<UMaterialInstance> GetAvatarMaterialSoftObject() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TSoftObjectPtr<UMaterialInstance> GetPortraitMaterialSoftObject() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetOrder() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool ShouldConsiderForDefault() const;

	bool RequiresEntitlement() const;
private:	
	FName DataName;
	TOptional<FSkinEntry> DataEntry;
};