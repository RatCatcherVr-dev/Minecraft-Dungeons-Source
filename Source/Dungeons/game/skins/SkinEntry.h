#pragma once

#include "Engine/DataTable.h"
#include <SoftObjectPtr.h>
#include <Materials/MaterialInstance.h>
#include "game/cosmetics/CosmeticsEntryBase.h"
#include "SkinEntry.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FSkinEntry : public FCosmeticsEntryBase {
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	TSoftObjectPtr<UMaterialInstance> AvatarMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	TSoftObjectPtr<UMaterialInstance> PortraitMaterial;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FString RequiredEntitlement;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	int Order = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	bool ConsiderForDefault = false;

	TArray<FSoftObjectPath> LinkedAssets() const override {
		return {AvatarMaterial.ToSoftObjectPath(), PortraitMaterial.ToSoftObjectPath()};
	}
};
