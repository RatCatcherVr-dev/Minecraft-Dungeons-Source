#pragma once
#include "type/MerchantBase.h"
#include "MerchantDef.generated.h"

class APlayerControllerBase;

UCLASS(BlueprintType, Abstract)
class DUNGEONS_API UMerchantDef : public UObject {
	GENERATED_BODY()

protected:
	UPROPERTY()
	TSubclassOf<AMerchantBase> MerchantClass;

	UPROPERTY()
	FString UnlockProgressKey;

	UPROPERTY()
	TArray<FString> GenerationLevelProgressKeys;

	UPROPERTY()
	FText DisplayName;

	TOptional<ELevelNames> LegacyUnlockMission;

public:
	const TSubclassOf<AMerchantBase>& GetMerchantClass() const;
	const FString& GetUnlockProgressKey() const;
	TOptional<FString> GetMerchantLevelProgressKey(const APlayerControllerBase*) const;
	int GetMerchantLevelIndexFor(const APlayerControllerBase*) const;
	FName CreateSaveName() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FText& GetDisplayName() const;
	
	TOptional<ELevelNames> GetLegacyUnlockMission() const;
};