#pragma once

#include <Kismet/BlueprintFunctionLibrary.h>
#include <Array.h>
#include <NoExportTypes.h>
#include "SkinDef.h"
#include "SkinsUtil.generated.h"

static const FColor PlayerIdentityErrorColor(FColor(255, 0, 255));

///// player colors <- todo: read from data in editor?
static const TArray<FColor> PlayerIdentityColors{
	FColor(243, 179, 62),
	FColor(44, 237, 176),
	FColor(250, 100, 197),
	FColor(136, 100, 255)
};

static TArray<FColor> PlayerGamepadColors
{
	FColor(255, 165, 0),
	FColor(0, 255, 80),
	FColor(200, 50, 147),
	FColor(86, 50, 205)
};

class UMaterialInstance;

UCLASS(BlueprintType)
class USkinsUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:	
	UFUNCTION(BlueprintCallable, BlueprintPure, category = "Dungeons", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UMaterialInstance* GetSkinAvatarMaterialInstance(const UObject* worldContextObject, FName skinId);

	UFUNCTION(BlueprintCallable, BlueprintPure, category = "Dungeons", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UMaterialInstance* GetSkinPortraitMaterialInstance(const UObject* worldContextObject, FName skinId);

	UFUNCTION(BlueprintCallable, BlueprintPure, category = "Dungeons", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static TSoftObjectPtr<UMaterialInstance> GetSkinAvatarMaterialSoft(const UObject* worldContextObject, FName skinId);

	UFUNCTION(BlueprintCallable, BlueprintPure, category = "Dungeons", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static TSoftObjectPtr<UMaterialInstance> GetSkinPortraitMaterialSoft(const UObject* worldContextObject, FName skinId);

	UFUNCTION(BlueprintCallable, BlueprintPure, category = "Dungeons", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static bool IsSkinEntitled(const UObject* worldContextObject, FName skinId);

	static TOptional<FEntitlement> FindEntitlement(const UObject* worldContextObject, const TArray<FEntitlement>& entitlements, FName skinId);

	UFUNCTION(BlueprintCallable, BlueprintPure, category = "Dungeons", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static bool IsSkinValid(const UObject* worldContextObject, FName skinId);

	UFUNCTION(BlueprintCallable, BlueprintPure, category = "Dungeons", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static bool IsEntitlementRequired(const UObject* worldContextObject, FName skinId);

	UFUNCTION(BlueprintCallable, BlueprintPure, category = "Dungeons")
	static FColor GetPlayerColor(int playerNumber);

	UFUNCTION(BlueprintCallable, BlueprintPure, category = "Dungeons", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static TArray<FName> GetSkinIds(const UObject* worldContextObject);

	UFUNCTION(BlueprintCallable, BlueprintPure, category = "Dungeons", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static TArray<FName> GetDefaultSkinIds(const UObject* worldContextObject);
	
	static bool IsEntitlementTamperedWith(const UObject* worldContextObject, FName skinId, const FEntitlement& entitlement, FString userId);

};
