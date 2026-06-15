#pragma once
#include "CoreMinimal.h"
#include <ObjectMacros.h>
#include <UserWidget.h>
#include <DelegateCombinations.h>
#include "OnlineBlueprintCallProxyBase.h"

#include "DungeonsCloudSaveBP.generated.h"

UENUM(BlueprintType)
enum class ECloudCharactersResult : uint8 {
	Failed,
	FailedAccountNotLinked,
	FailedLogin,
	FailedServiceUnavailable,
	Success
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCloudCharactersCompleted, const TArray<APlayerCharacterSaveSlot*>&, Result, ECloudCharactersResult, Reason);

class APlayerCharacterSaveSlot;
class ABasePlayerController;

UCLASS()
class UDungeonsCloudSaveBPProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()
public:
	void Activate() override;
	virtual ~UDungeonsCloudSaveBPProxy();

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Dungeons|Cloudsave")
	static UDungeonsCloudSaveBPProxy* GetCloudCharacters();		/// Get list of cloud characters
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Dungeons|Cloudsave")
	static UDungeonsCloudSaveBPProxy* SetCloudCharacters(const TArray<APlayerCharacterSaveSlot*>& Characters);		/// Set list of cloud characters
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Cloudsave")
	static void DownloadCloudCharacter( APlayerCharacterSaveSlot* pSaveSlot);		// Save downloaed character to local
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Cloudsave")
	static void CloudSaveExit();

	UPROPERTY(BlueprintAssignable)
	FCloudCharactersCompleted GetCloudCharactersCompleted;

	UPROPERTY(BlueprintAssignable)
	FCloudCharactersCompleted SetCloudCharactersCompleted;

private:
	static UDungeonsCloudSaveBPProxy* GetInstance();
	bool mbGetCharacters = false;
	TArray<APlayerCharacterSaveSlot*> mCloudSaveChraracterList;
};
