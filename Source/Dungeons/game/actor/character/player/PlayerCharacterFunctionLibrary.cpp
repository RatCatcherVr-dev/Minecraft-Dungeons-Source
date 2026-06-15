#include "Dungeons.h"
#include "PlayerCharacterFunctionLibrary.h"
#include "Engine.h"
#include "DungeonsGameInstance.h"
#include "DungeonsUserManagement.h"
#include "PlayerCharacter.h"
#include "game/util/PlayerQuery.h"


APlayerCharacter* UPlayerCharacterFunctionLibrary::GetPlayerCharacterByPlayerId(const UObject* WorldContextObject, const int32 ID)
{
	const auto& PlayerList = InstanceTracker< APlayerCharacter >::GetList(WorldContextObject->GetWorld());

	for (APlayerCharacter* pPlayer : PlayerList)
	{
		if(pPlayer && pPlayer->GetDungeonsBasePlayerState() && pPlayer->GetDungeonsBasePlayerState()->PlayerId == ID)
			return pPlayer;
	}

	return nullptr;
}

int32 CountControlledPlayerCharacters(const UObject* WorldContextObject, bool Local)
{
	int32 iRet = 0;
	const auto& PlayerList = InstanceTracker< APlayerCharacter >::GetList(WorldContextObject->GetWorld());

	for (APlayerCharacter* pPlayer : PlayerList)
	{
		if (pPlayer && pPlayer->IsLocallyControlled() == Local)
			++iRet;
	}

	return iRet;
}

int32 UPlayerCharacterFunctionLibrary::CountLocalPlayerCharacters(const UObject* WorldContextObject)
{
	return CountControlledPlayerCharacters(WorldContextObject,true);
}

int32 UPlayerCharacterFunctionLibrary::CountRemotePlayerCharacters(const UObject* WorldContextObject)
{
	return CountControlledPlayerCharacters(WorldContextObject,false);
}

APlayerCharacter* UPlayerCharacterFunctionLibrary::GetFirstLocalPlayerCharacter(const UObject* WorldContextObject)
{
	return playerquery::getFirstLocalPlayerCharacter(WorldContextObject->GetWorld());
}
