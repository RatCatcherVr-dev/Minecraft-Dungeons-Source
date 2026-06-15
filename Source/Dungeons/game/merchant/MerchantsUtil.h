#pragma once
#include "CoreMinimal.h"
#include "MerchantDef.h"
#include "MerchantsUtil.generated.h"

UCLASS()
class DUNGEONS_API UMerchantsUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetUnlockedMerchantsCount(UObject* worldContext);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetTotalMerchantsCount();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool IsMerchantLockedForLevel(UObject* worldContext, ELevelNames level);	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons", meta = (WorldContext = "WorldContextObject"))
	static bool IsMerchantUnlocked(UObject* WorldContextObject, TSubclassOf<UMerchantDef> def);
	UFUNCTION(BlueprintCallable, Category = "Dungeons", meta = (WorldContext = "WorldContextObject"))
	static void UnlockMerchantForAllLocalPlayers(UObject* WorldContextObject, TSubclassOf<UMerchantDef> def);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const FText& GetMerchantName(TSubclassOf<UMerchantDef> def);
};

