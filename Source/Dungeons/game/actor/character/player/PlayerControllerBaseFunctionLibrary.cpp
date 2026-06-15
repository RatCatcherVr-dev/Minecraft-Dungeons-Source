#include "Dungeons.h"
#include "PlayerControllerBaseFunctionLibrary.h"
#include "Engine.h"
#include "DungeonsGameInstance.h"
#include "DungeonsUserManagement.h"
#include "PlayerControllerBase.h"

static int32 sMaxControllerIndex = 3;
FAutoConsoleVariableRef CVarMaxControllerIndex(
	TEXT("PlayerController.MaxControllerIndex"),
	sMaxControllerIndex,
	TEXT("Determines the Max controller Index"),
	ECVF_Default);


int32 UPlayerControllerBaseFunctionLibrary::GetMaxControllerIndex()
{
	return sMaxControllerIndex;
}

int32 UPlayerControllerBaseFunctionLibrary::CountControllers(const UObject* WorldContextObject)
{
	int32 iCount = 0;
	const int32 iMaxControllerIndex = sMaxControllerIndex;
	for (int32 i = 0; i < iMaxControllerIndex; ++i)
	{
		if (auto* pPlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, i))
		{
			if (pPlayerController->IsLocalController())
			{
				++iCount;
			}
		}
	}
	return iCount;
}

APlayerControllerBase* UPlayerControllerBaseFunctionLibrary::GetFirstDungeonsPlayerController(const UObject* WorldContextObject)
{
	if (auto* gi = Cast<UDungeonsGameInstance>(WorldContextObject->GetWorld()->GetGameInstance()))
	{
		return Cast<APlayerControllerBase>(gi->GetUserManager()->GetInitialPlayerController());
	}
	return nullptr;
}

