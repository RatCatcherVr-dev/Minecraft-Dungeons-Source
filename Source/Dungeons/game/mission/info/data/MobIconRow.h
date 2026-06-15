#pragma once
#include "Engine/DataTable.h"
#include "ui/info/ChanceInfoIconWidget.h"
#include "MobIconRow.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FMobIconRow : public FTableRowBase {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	TSoftClassPtr<UChanceInfoIconWidget> MobChanceInfoIcon;
};
