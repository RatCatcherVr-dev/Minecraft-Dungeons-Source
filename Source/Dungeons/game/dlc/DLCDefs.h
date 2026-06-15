#pragma once

#include "DLCName.h"
#include "DLCDef.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "locale/LocalizationUtils.h"
#include "DLCDefs.generated.h"

namespace dlc {
const DLCDef& get(EDLCName);
const DLCDef* getChecked(EDLCName);
const TArray<EDLCName> getAllEnabled();
bool shouldPackageAssets(EDLCName);
}

UCLASS(BlueprintType)
class UDLCDefs : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool GetDLCNameReleased(EDLCName dlcName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const FText& GetDLCNameText(EDLCName dlcName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const FText& GetDLCUpsellTitleText(EDLCName dlcName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const FText& GetDLCUpsellDescriptionText(EDLCName dlcName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static TArray<FText> GetDLCUpsellBulletPoints(EDLCName dlcName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static UTexture2D* GetDLCInspectorBackgroundTexture(EDLCName dlcName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static UTexture2D* GetDLCInspectorLogoTexture(EDLCName dlcName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool GetDLCLocaleRequiresLogoTranslation(EDLCName dlcName, DungeonsLocale locale);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static void OpenExternalStorePageForDlcName(APlayerController* playerController, FString category, EDLCName dlcName);
};
