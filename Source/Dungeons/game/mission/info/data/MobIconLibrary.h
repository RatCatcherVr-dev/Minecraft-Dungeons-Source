#pragma once

#include "CoreMinimal.h"
#include "Engine/ObjectLibrary.h"
#include "Engine/DataTable.h"
#include "MobIconRow.h"
#include "MobIconLibrary.generated.h"

UCLASS()
class DUNGEONS_API UMobIconLibrary : public UObject{
public:
	GENERATED_BODY()

	UMobIconLibrary();

	const FMobIconRow* findRow(const FName& row) const;
private:
	UPROPERTY()
	UDataTable* mInfoDataTable;
	
	static const FString ContextString;
};
