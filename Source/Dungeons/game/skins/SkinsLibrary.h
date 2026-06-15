#pragma once

#include "CoreMinimal.h"
#include "Engine/ObjectLibrary.h"
#include "Engine/DataTable.h"
#include "SkinDef.h"
#include "SkinsLibrary.generated.h"

UCLASS()
class DUNGEONS_API USkinsLibrary final : public UObject {

	GENERATED_BODY()

public:
	USkinsLibrary();

	USkinDef* GetOrCreateDefinition(const FName& name);

	using SkinsDefFilterPredicate = std::function<bool(const USkinDef&)>;
	TArray<USkinDef*> GetOrCreateDefinitions(SkinsDefFilterPredicate defFilter = [](const USkinDef&){ return true; });

private:
	USkinDef* GetOrCreateDefinition(const FName& dataName, const FSkinEntry& dataEntry);

	static const FString ContextString;

	UPROPERTY()
	UDataTable* SkinsLookupTable;

	UPROPERTY() //Only TArray will be considered for by garbage collector
	TArray<USkinDef*> SkinsDefsLoaded;

	UPROPERTY()
	TMap<FName, USkinDef*> SkinsDefsLoadedMap;
};
