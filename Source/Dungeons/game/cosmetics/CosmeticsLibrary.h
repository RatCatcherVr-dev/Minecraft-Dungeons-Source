#pragma once

#include "CoreMinimal.h"
#include "Engine/ObjectLibrary.h"
#include "Engine/DataTable.h"
#include "CosmeticItemDef.h"
#include "save/CharacterSaveData.h"
#include "CosmeticsLibrary.generated.h"

UCLASS()
class DUNGEONS_API UCosmeticsLibrary final : public UObject {

	GENERATED_BODY()

public:
	UCosmeticsLibrary();

	UCosmeticItemDef* FindDefinition(const FName& name) const;
	TArray<UCosmeticItemDef*> FindDefinitions(const TArray<CosmeticsSaveData>& saveData) const;

	using CosmeticsDefFilterPredicate = std::function<bool(const UCosmeticItemDef&)>;
	TArray<UCosmeticItemDef*> FindDefinitions(CosmeticsDefFilterPredicate) const;

	UCosmeticItemDef* FindDefinitionByEntitlementName(const FString& entitlementName) const;

private:
	static const FString ContextString;

	UPROPERTY()
	UDataTable* CosmeticsLookupTable;
};
