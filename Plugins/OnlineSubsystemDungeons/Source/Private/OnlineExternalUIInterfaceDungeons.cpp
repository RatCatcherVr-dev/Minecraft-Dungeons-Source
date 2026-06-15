#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineExternalUIInterfaceDungeons.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemDungeons.h"
#include "OnlineIdentityDungeons.h"

#if PLATFORM_XBOXONE
#include "OnlineIdentityDungeonsXboxOne.h"
#include "XboxOne/XboxOneApplication.h"
#include "XboxOne/XboxOneInputInterface.h"

using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Xbox::UI;
using namespace Windows::Xbox::ApplicationModel;

using Windows::Xbox::ApplicationModel::Store::ProductItemTypes;
#elif PLATFORM_SWITCH
#include "HAL/PlatformMisc.h"
#include "Misc/ConfigCacheIni.h"

#include <nn/web/web_Api.h>
#include <nn/account/account_Selector.h>
#include <nn/nn_ApplicationId.h>
#include <nn/ec/ec_ShopApi.h>
#include <nn/friends/friends_Api.h>
#include <nn/friends/friends_Applet.h>
#include <nn/oe/oe_ApplicationControlApis.h>
#endif
#include "CoreDelegates.h"

FOnlineExternalUIDungeons::FOnlineExternalUIDungeons(FOnlineSubsystemDungeons* InSubsystem)
	: Subsystem(InSubsystem)
{
	OnLoginCompleteDelegate = FOnLoginCompleteDelegate::CreateRaw(this, &FOnlineExternalUIDungeons::TriggerUIClosedDelegate);

	IOnlineIdentityPtr IdentityPtr = Subsystem->GetIdentityInterface();

	if (!IdentityPtr.IsValid())
	{
		UE_LOG_ONLINE(Warning, TEXT("[ExternalUI] Failed to get IdentityInterface (nullptr)"));
		return;
	}
	//D11.PC
	for (uint8 i = 0; i < 8; ++i)
	{
		OnLoginCompleteDelegateHandles.Add(IdentityPtr->AddOnLoginCompleteDelegate_Handle(i, OnLoginCompleteDelegate));
	}
}

FOnlineExternalUIDungeons::~FOnlineExternalUIDungeons()
{}

#if !PLATFORM_XBOXONE
bool FOnlineExternalUIDungeons::ShowProfileUI(const FUniqueNetId& Requestor, const FUniqueNetId& Requestee, const FOnProfileUIClosedDelegate& Delegate)
{
	// #D11.CM - TODO - Currently this just needs to handle windows and xbox
	// - Switch doesn't need to view profiles
	// - Playstation isn't currently on the Dungeons OSS.

	return false;
}
#else
bool FOnlineExternalUIDungeons::ShowProfileUI(const FUniqueNetId& Requestor, const FUniqueNetId& Requestee, const FOnProfileUIClosedDelegate& Delegate)
{
	if (!Subsystem || !Subsystem->GetIdentityInterface().IsValid())
	{
		return false;
	}

	FXboxOneApplication* XboxOneApp = FXboxOneApplication::GetXboxOneApplication();
	if (XboxOneApp == nullptr)
	{
		return false;
	}

	auto XboxInputInterface = XboxOneApp->GetXboxInputInterface();

	FPlatformUserId PlatformUserId = Subsystem->GetIdentityInterface()->GetPlatformUserIdFromUniqueNetId(*FUniqueNetIdDungeons::Create(Requestor));
	Windows::Xbox::System::User^ RequestingUser = XboxInputInterface->GetXboxUserFromPlatformUserId(PlatformUserId);

	if (!ensure(RequestingUser != nullptr))
	{
		UE_LOG_ONLINE_EXTERNALUI(Warning, TEXT("Passed in invalider requester to"));
		return false;
	}

	Windows::Xbox::UI::SystemUI::ShowProfileCardAsync(RequestingUser, ref new Platform::String(*Requestee.ToString()));

	return true;
}
#endif

#if !PLATFORM_SWITCH
bool FOnlineExternalUIDungeons::ShowLoginUI(const int ControllerIndex, bool bShowOnlineOnly, bool bShowSkipButton, const FOnLoginUIClosedDelegate& Delegate /*= FOnLoginUIClosedDelegate()*/)
{
	IOnlineIdentityPtr IdentityPtr = Subsystem->GetIdentityInterface();

	if (!IdentityPtr.IsValid())
	{
		UE_LOG_ONLINE(Warning, TEXT("[ExternalUI] Failed to get IdentityInterface (nullptr)"));
		return false;
	}

	// cleanup previous delegate if there was one cached.
	if (ShowLoginDelegate.IsBound()) {
		ShowLoginDelegate.Unbind();
	}

	ShowLoginDelegate = Delegate;
	ShowLoginControllerIndex = ControllerIndex;

	return IdentityPtr->Login(ControllerIndex, FOnlineAccountCredentials()); //TODO - Is controllerindex equivalent to LocalUserNum? Can we treat it like that?
}

#else

#include <nn/account.h>

bool FOnlineExternalUIDungeons::ShowLoginUI(const int ControllerIndex, bool bShowOnlineOnly, bool bShowSkipButton, const FOnLoginUIClosedDelegate& Delegate)
{
	if (Delegate.IsBound())
	{
		ShowLoginDelegate = FOnLoginUIClosedDelegate(Delegate);
		ShowLoginControllerIndex = ControllerIndex;
	}

	nn::account::Uid Uid;
	TSharedPtr<const FUniqueNetId> UniqueNetId;
	IOnlineIdentityPtr IdentityPtr = Subsystem->GetIdentityInterface();
	bool success = FSwitchPlatformMisc::GetUidForControllerId(ControllerIndex, Uid);
	//ShowLoginDelegate.ExecuteIfBound(UniqueNetId, ShowLoginControllerIndex, FOnlineError(success));
	return IdentityPtr->Login(ControllerIndex, FOnlineAccountCredentials()); 
}


#endif
void FOnlineExternalUIDungeons::TriggerUIClosedDelegate(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	FOnlineIdentityDungeons* IdentityPtr = Subsystem->GetDungeonsIdentityInterface();
	if (!IdentityPtr)
	{
		UE_LOG_ONLINE(Warning, TEXT("[ExternalUI] Failed to get IdentityInterface (nullptr)"));
		return;
	}

	const auto user = IdentityPtr->GetUserFromLocalUser(LocalUserNum);
	const auto systemId = user ? user->SystemId : 0;

	FOnlineError error(bWasSuccessful);
	error.SetFromErrorCode(Error);
	ShowLoginDelegate.ExecuteIfBound(IdentityPtr->GetUniquePlayerId(LocalUserNum), systemId, error);
} 


bool FOnlineExternalUIDungeons::ShowStoreUI(int32 LocalUserNum, const FShowStoreParams& ShowParams, const FOnShowStoreUIClosedDelegate& Delegate)
{
#if PLATFORM_XBOXONE
	FOnlineIdentityDungeons* IdentityPtr = Subsystem->GetDungeonsIdentityInterface();
	if (!IdentityPtr)
	{
		UE_LOG_ONLINE(Warning, TEXT("[ExternalUI] Failed to get IdentityInterface (nullptr)"));
		return false;
	}

	FXboxOneApplication* XboxOneApp = FXboxOneApplication::GetXboxOneApplication();
	if (XboxOneApp == nullptr)
	{
		return false;
	}

	auto XboxInputInterface = XboxOneApp->GetXboxInputInterface();
	Windows::Xbox::System::User^ LiveUser = XboxInputInterface->GetXboxUserFromPlatformUserId(LocalUserNum);

	IAsyncAction^ ShowStoreTask = nullptr;
	if (!ShowParams.ProductId.IsEmpty())
	{
		ShowStoreTask = Store::Product::ShowDetailsAsync(LiveUser,
			ref new Platform::String(*ShowParams.ProductId));
	}
	else
	{
		ShowStoreTask = Store::Product::ShowMarketplaceAsync(LiveUser,
			ProductItemTypes::Game,
			ref new Platform::String(*ShowParams.ProductId),
			ProductItemTypes::Consumable | ProductItemTypes::Durable | ProductItemTypes::Game | ProductItemTypes::App | ProductItemTypes::GameDemo);
	}

	concurrency::create_task(ShowStoreTask).then([this](concurrency::task<void> Task)
	{
		try
		{
			Task.get();
			UE_LOG_ONLINE_EXTERNALUI(Log, TEXT("ShowStoreUI: Marketplace UI now displaying."));
		}
		catch (const std::exception& Ex)
		{
			UE_LOG_ONLINE_EXTERNALUI(Warning, TEXT("FOnlineExternalUI::ShowStoreUI call failed with error %s"), ANSI_TO_TCHAR(Ex.what()));
		}
		catch (Platform::Exception^ Ex)
		{
			UE_LOG_ONLINE_EXTERNALUI(Warning, TEXT("FOnlineExternalUI::ShowStoreUI call failed with 0x%0.8X %ls"), Ex->HResult, Ex->Message->Data());
		}
	});
	return true;
#elif PLATFORM_SWITCH
	Subsystem->QueueAsyncTask(
		MakeUnique<FOnlineAsyncTaskTrackStoreUIStatus>(LocalUserNum, ShowParams, Delegate));
	return true;
#endif

	return false;
}
#if PLATFORM_SWITCH
FOnlineAsyncTaskTrackStoreUIStatus::FOnlineAsyncTaskTrackStoreUIStatus(int32 InLocalUserNum, const FShowStoreParams& InShowParams, const FOnShowStoreUIClosedDelegate& OnShowStoreUIClosedDelegate)
	:
	ShowStoreUIClosedDelegate(OnShowStoreUIClosedDelegate),
	LocalUserNum(InLocalUserNum),
	ShowParams(InShowParams)
{}

void FOnlineAsyncTaskTrackStoreUIStatus::Tick()
{

	nn::account::Uid Uid;
	nn::account::UserHandle Handle;
	FSwitchPlatformMisc::GetUidForControllerId(LocalUserNum, Uid);
	FSwitchPlatformMisc::GetOpenedUserHandle(Handle, Uid);


	nn::ApplicationId AppId = nn::ApplicationId();
	FString ApplicationIdString;
	if (!GConfig->GetString(TEXT("/Script/SwitchRuntimeSettings.SwitchRuntimeSettings"), TEXT("ProgramId"), ApplicationIdString, GEngineIni))
	{
		UE_LOG_ONLINE(Warning, TEXT("Could not find ApplicationId in SwitchRuntimeSettings"));
		AppId = nn::ApplicationId::GetInvalidId();
	}
	else
	{
		AppId.value = FCString::Strtoui64(*ApplicationIdString, nullptr, 16);
	}
	

	if (AppId == nn::ApplicationId::GetInvalidId())
	{
		UE_LOG_ONLINE_EXTERNALUI(Warning, TEXT("OnlineAsyncTaskSwitchTrackStoreUIStatus::Tick(): ApplicationId is invalid"));
		bWasSuccessful = false;
		bIsComplete = true;
		return;
	}

	if (ShowParams.Category == SHOP_PURCHASABLE_ADDON)
	{
		nn::ec::ShowShopAddOnContentList(AppId, Handle);
	}
	else if (ShowParams.Category == SHOP_PURCHASABLE_SUBSCRIPTION)
	{
		nn::ec::ShowShopSubscriptionList(AppId, Handle);
	}
	else if (ShowParams.Category == SHOP_PURCHASABLE_CONSUMABLE)
	{
		nn::ec::ShowShopConsumableItemList(AppId, Handle);
	}
	else
	{
		nn::ec::ShowShopApplicationInformation(AppId, Handle);
	}

	bWasSuccessful = true;
	bIsComplete = true;

}
#endif
