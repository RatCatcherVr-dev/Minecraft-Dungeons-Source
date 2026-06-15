#include "DungeonsLoginFlowNull.h"
#include "DungeonsGameInstance.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"


UDungeonsLoginFlowNull::UDungeonsLoginFlowNull(const FObjectInitializer& ObjectInitializer)
	: UDungeonsLoginFlow(ObjectInitializer)
{

}

void UDungeonsLoginFlowNull::PlatformLogin(ULocalPlayer* LocalPlayer)
{
	int ControllerId = GetPlayerControllerId();
	SetPlayerControllerId(ControllerId);
	if (UDungeonsLocalPlayer* DungeonsLocalPlayer = Cast<UDungeonsLocalPlayer>(LocalPlayer))
	{
		DungeonsLocalPlayer->SetUserSystemId(ControllerId);
	}
	LoadSaveData(ControllerId);
}
