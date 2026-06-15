#pragma once

#include "Engine/DataTable.h"
#include "CosmeticsEntryBase.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FCosmeticsEntryBase : public FTableRowBase {
	GENERATED_BODY()
	
	virtual TArray<FSoftObjectPath> LinkedAssets() const PURE_VIRTUAL(FCosmeticsEntryBase::LinkedAssets, return {}; );

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	bool IsReleased = false;
};
