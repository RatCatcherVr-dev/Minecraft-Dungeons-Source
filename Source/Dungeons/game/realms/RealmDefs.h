#pragma once

#include "Realms.h"
#include "RealmDef.h"
#include "RealmDefs.generated.h"

namespace realms {

const RealmDef& get(ERealmName);
const RealmDef* getChecked(ERealmName);
const TArray<ERealmName> getAllEnabled();
bool shouldPackageAssets(ERealmName);

const ERealmName Default = ERealmName::ArchIllagerRealm;
}

UCLASS(BlueprintType)
class URealmDefs : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static TArray<ERealmName> GetEnabledRealmNames();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool IsRealmLocked(ERealmName realm);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const FText& GetRealmNameText(ERealmName realm);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const FText& GetRealmTravelText(ERealmName realm);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const FText& GetRealmLockedTravelText(ERealmName realm);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const FText& GetRealmLockedDescriptionText(ERealmName realm);
};
