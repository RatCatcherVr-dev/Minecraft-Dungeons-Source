
#include "DungeonsCloudSaveBP.h"
#include "DungeonsCloudLoadSave.h"
#include "OnlineSubsystem.h"
#include "Engine.h"
#include "game/actor/character/player/PlayerCharacterSaveSlot.h" 
#include "DungeonsUserManagement.h"
#include "DungeonsCloudLoadSave.h"


static TWeakObjectPtr<UDungeonsCloudSaveBPProxy>  spDungeonsInstance;

#if CLOUDSAVE_ENABLED

static FDungeonsCloudLoadSave* GetCloudInterface()
{
	return FDungeonsCloudLoadSave::Instance();
}
#endif

UDungeonsCloudSaveBPProxy::UDungeonsCloudSaveBPProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	check(spDungeonsInstance == nullptr);
	spDungeonsInstance = this;
}

UDungeonsCloudSaveBPProxy::~UDungeonsCloudSaveBPProxy()
{
	
}

UDungeonsCloudSaveBPProxy* UDungeonsCloudSaveBPProxy::GetInstance()
{
	check(spDungeonsInstance.IsValid());
	return spDungeonsInstance.Get();;
}


UDungeonsCloudSaveBPProxy* UDungeonsCloudSaveBPProxy::GetCloudCharacters()
{
	auto pInstance = GetInstance();
	pInstance->mbGetCharacters = true;
	return pInstance;
}

UDungeonsCloudSaveBPProxy* UDungeonsCloudSaveBPProxy::SetCloudCharacters(const TArray<APlayerCharacterSaveSlot*>& List )
{
	auto pInstance = GetInstance();
	pInstance->mbGetCharacters = false;
	pInstance->mCloudSaveChraracterList = List;
	return pInstance;
}

void UDungeonsCloudSaveBPProxy::DownloadCloudCharacter( APlayerCharacterSaveSlot* pSaveSlot)
{
#if CLOUDSAVE_ENABLED
	GetCloudInterface()->DownloadDungeonsCloudCharacter( pSaveSlot);
#endif
}

void UDungeonsCloudSaveBPProxy::Activate()
{
#if CLOUDSAVE_ENABLED
	auto SystemoId = UDungeonsUserManager::Instance()->GetInitialUserSystemId();
	if ( mbGetCharacters )
		GetCloudInterface()->GetDungeonsCloudCharacters(this, SystemoId);
	else
		GetCloudInterface()->SetDungeonsCloudCharacters(mCloudSaveChraracterList);
#endif
}

void UDungeonsCloudSaveBPProxy::CloudSaveExit()
{
#if CLOUDSAVE_ENABLED	
	GetCloudInterface()->CloudSaveExit( true) ;
#endif
}
