#if PLATFORM_XBOXONE

#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineIdentityDungeons.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineSubsystemDungeons.h"

#include <stdlib.h>
#include <memory>
#include <vector>
#include "utils.h"

#include "OnlineSubsystemTypes.h"
#include "XboxOne/XboxOneApplication.h"
#include "SlateApplication.h"
#include "XboxOne/XboxOneInputInterface.h"
#include "OnlineIdentityDungeonsXboxOne.h"
#include "online/OnlineCommon.h"

#include "HrLog.h"
#include "GenericPlatform/GenericPlatformHttp.h"


HC_DEFINE_TRACE_AREA(DUNGEONS, HCTraceLevel::Verbose);
HC_DECLARE_TRACE_AREA(XAL);
HC_DECLARE_TRACE_AREA(XAL_TELEMETRY);
HC_DECLARE_TRACE_AREA(HTTPCLIENT);
HC_DECLARE_TRACE_AREA(HC_CURL);
HC_DECLARE_TRACE_AREA(HC_WS);

#if USE_OSSLIVE_PRIVILEGECHECK
using namespace Microsoft;
using namespace Windows::Foundation;
using namespace Windows::Xbox::System;
using namespace Windows::Xbox::Input;
using namespace Windows::Xbox::ApplicationModel::Store;
using namespace concurrency;
using Windows::Xbox::System::UserAgeGroup;
using namespace Windows::Xbox::Management::Deployment;
using namespace Windows::Foundation::Collections;
using namespace Windows::Foundation;
#endif

// D11.PC

FOnlineIdentityDungeonsPtr dungeonsIdentity::platform::Create(class FOnlineSubsystemDungeons* InDungeonsSubsystem)
{
	return MakeShared<FOnlineIdentityDungeonsXboxOne, ESPMode::ThreadSafe>(InDungeonsSubsystem);
}


FOnlineIdentityDungeonsXboxOne::FOnlineIdentityDungeonsXboxOne(FOnlineSubsystemDungeons* InSubsystem)
	: FOnlineIdentityDungeons(InSubsystem)
{
	InitializePlatform();
}

FOnlineIdentityDungeonsXboxOne::~FOnlineIdentityDungeonsXboxOne()
{
	UnHookEvents();

	try
	{
		Windows::Xbox::System::User::SignOutStarted -= SignOutStartedToken;
	}
	catch (Platform::Exception^)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] Failed to teardown SignOutHandler"));
	}
}

TSharedPtr< class FXboxOneInputInterface, ESPMode::ThreadSafe > FOnlineIdentityDungeonsXboxOne::GetXboxInputInterface()
{
	if (!FSlateApplication::IsInitialized())
	{
		return nullptr;
	}

	FXboxOneApplication* XboxOneApp = FXboxOneApplication::GetXboxOneApplication();
	if (XboxOneApp == nullptr)
	{
		return nullptr;
	}

	return XboxOneApp->GetXboxInputInterface();
}

void FOnlineIdentityDungeonsXboxOne::GetUserIdString(Windows::Xbox::System::User^ user, std::string& stdString) const
{
	Platform::String^ userString = user->XboxUserId;
	std::wstring fooW(userString->Begin());
	stdString = std::string(fooW.begin(), fooW.end());

	//D11.KS - This is to catch a super edge case, if a guest is created on a Xbox that isn't connected to the internet
	//it'll be given a non numerical unique id which isn't valid, so we'll generate an hash for an id using the gamertag.
	if(!isdigit(stdString[0]))
	{
		stdString = std::to_string(FCrc::MemCrc32(user->DisplayInfo->Gamertag->Begin(), user->DisplayInfo->Gamertag->Length() * sizeof(TCHAR)));
	}
}

FUniqueNetIdDungeonsRef FOnlineIdentityDungeonsXboxOne::GetNetIdFromUser(Windows::Xbox::System::User^ user) const
{
	if (user)
	{
		std::string UserID;
		GetUserIdString(user, UserID);
		return FUniqueNetIdDungeons::Create(std::stoull(UserID));
	}
	return FUniqueNetIdDungeons::Create(0);
}

FPlatformUserId FOnlineIdentityDungeonsXboxOne::GetPlatformUserIdFromUniqueNetId(const FUniqueNetId& UniqueNetId) const
{
	FXboxOneApplication* XboxOneApp = FXboxOneApplication::GetXboxOneApplication();
	if (XboxOneApp == nullptr)
	{
		return PLATFORMUSERID_NONE;
	}
	auto XboxInputInterface = XboxOneApp->GetXboxInputInterface();

	if (!XboxInputInterface.IsValid())
	{
		return PLATFORMUSERID_NONE;
	}

	return XboxInputInterface->GetPlatformUserIdFromXboxUserId(*UniqueNetId.ToString());
}

#if USE_OSSLIVE_PRIVILEGECHECK
void FOnlineIdentityDungeonsXboxOne::GetUserPrivilege(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate)
{
	FUniqueNetIdDungeonsRef LiveUserId = StaticCastSharedRef<const FUniqueNetIdDungeons>(UserId.AsShared());
	Windows::Xbox::System::User^ LiveUser = GetXboxInputInterface()->GetXboxUserFromPlatformUserId(GetPlatformUserIdFromUniqueNetId(*LiveUserId));

	if (LiveUser == nullptr)
	{
		UE_LOG_ONLINE_IDENTITY(Log, TEXT("FOnlineIdentityLive::GetUserPrivilege couldn't find Live user for unique id %s."), *LiveUserId->ToString());
		DungeonsSubsystem->ExecuteNextTick([LiveUserId, Privilege, Delegate]()
		{
			Delegate.ExecuteIfBound(*LiveUserId, Privilege, static_cast<uint32>(EPrivilegeResults::UserNotFound));
		});
		return;
	}

	// Docs warn to not call CheckPrivilegeAsync if the user isn't signed in.
	if (!LiveUser->IsSignedIn)
	{
		UE_LOG_ONLINE_IDENTITY(Log, TEXT("FOnlineIdentityLive::GetUserPrivilege Live user %s is not signed in."), LiveUser->DisplayInfo->GameDisplayName->Data());
		DungeonsSubsystem->ExecuteNextTick([LiveUserId, Privilege, Delegate]()
		{
			Delegate.ExecuteIfBound(*LiveUserId, Privilege, static_cast<uint32>(EPrivilegeResults::UserNotLoggedIn));
		});
		return;
	}

	bool bShowUpsellUIIfPossible = true;
	uint32 KnownPrivilege = static_cast<uint32>(KnownPrivileges::XPRIVILEGE_MULTIPLAYER_SESSIONS);
	switch (Privilege)
	{
	case EUserPrivileges::CanPlayOnline:
	{
		KnownPrivilege = static_cast<uint32>(KnownPrivileges::XPRIVILEGE_MULTIPLAYER_SESSIONS);
		bShowUpsellUIIfPossible = true;
		break;
	}

	case EUserPrivileges::CanCommunicateOnline:
	{
		KnownPrivilege = static_cast<uint32>(KnownPrivileges::XPRIVILEGE_COMMUNICATIONS);
		break;
	}

	case EUserPrivileges::CanUseUserGeneratedContent:
	{
		KnownPrivilege = static_cast<uint32>(KnownPrivileges::XPRIVILEGE_USER_CREATED_CONTENT);
		break;
	}

	case EUserPrivileges::CanUserCrossPlay:
	{
		KnownPrivilege = TEMP_XPRIVILEGE_CROSSPLAY_ALLOWED;
		break;
	}

	default:
	{
		// @todo: Add other privilege types
		DungeonsSubsystem->ExecuteNextTick([LiveUserId, Privilege, Delegate]()
		{
			Delegate.ExecuteIfBound(*LiveUserId, Privilege, static_cast<uint32>(EPrivilegeResults::NoFailures));
		});
		return;
	}
	}

	try
	{
		IAsyncOperation<PrivilegeCheckResult>^ CheckOp = Product::CheckPrivilegeAsync(LiveUser, KnownPrivilege, bShowUpsellUIIfPossible, nullptr);
		Concurrency::create_task(CheckOp).then([this, LiveUserId, Privilege, Delegate, LiveUser](Concurrency::task<PrivilegeCheckResult> Task)
		{
			try
			{
				PrivilegeCheckResult Result = Task.get();

				auto PatchCheckCompletion = [this, Delegate, LiveUserId, Privilege, Result](const FOnlineError& ErrorResult, const TOptional<ECheckForPackageUpdateResult> OptionalPatchCheckResult)
				{
					if (!ErrorResult.bSucceeded)
					{
						// Ensure our patch check succeeds in non-development environments.  In development, unpackaged builds won't have a package
						DungeonsSubsystem->ExecuteNextTick([LiveUserId, Privilege, Delegate]()
						{
							Delegate.ExecuteIfBound(*LiveUserId, Privilege, static_cast<uint32>(EPrivilegeResults::GenericFailure));
						});
						return;
					}

					if (OptionalPatchCheckResult.IsSet())
					{
						const ECheckForPackageUpdateResult PatchCheckResult = OptionalPatchCheckResult.GetValue();
						if (PatchCheckResult == ECheckForPackageUpdateResult::MandatoryUpdateAvailable)
						{
							DungeonsSubsystem->ExecuteNextTick([LiveUserId, Privilege, Delegate]()
							{
								Delegate.ExecuteIfBound(*LiveUserId, Privilege, static_cast<uint32>(EPrivilegeResults::RequiredPatchAvailable));
							});
							return;
						}
					}

					const uint32 ResultInt = static_cast<uint32>(Result);

					UE_LOG_ONLINE_IDENTITY(Warning, TEXT("CheckPrivilegeAsync User=[%s] Privilege=[%d] Result=[%d]"), *LiveUserId->ToString(), static_cast<uint32>(Privilege), ResultInt);

					// Default to GenericFailure
					EPrivilegeResults PrivilegeResult = EPrivilegeResults::GenericFailure;
					if (Result == PrivilegeCheckResult::NoIssue)
					{
						PrivilegeResult = EPrivilegeResults::NoFailures;
					}
					else if (ResultInt & static_cast<uint32>(PrivilegeCheckResult::PurchaseRequired))
					{
						PrivilegeResult = EPrivilegeResults::AccountTypeFailure;
					}
					else if ((ResultInt & static_cast<uint32>(PrivilegeCheckResult::Restricted))
						|| (ResultInt & static_cast<uint32>(PrivilegeCheckResult::Banned)))
					{
						if (Privilege == EUserPrivileges::CanPlayOnline)
						{
							PrivilegeResult = EPrivilegeResults::OnlinePlayRestricted;
						}
						else if (Privilege == EUserPrivileges::CanCommunicateOnline)
						{
							PrivilegeResult = EPrivilegeResults::ChatRestriction;
						}
						else if (Privilege == EUserPrivileges::CanUseUserGeneratedContent)
						{
							PrivilegeResult = EPrivilegeResults::UGCRestriction;
						}
						else if (Privilege == EUserPrivileges::CanUserCrossPlay)
						{
							PrivilegeResult = EPrivilegeResults::OnlinePlayRestricted;
						}
					}
					DungeonsSubsystem->ExecuteNextTick([LiveUserId, Privilege, PrivilegeResult, Delegate]()
					{
						Delegate.ExecuteIfBound(*LiveUserId, Privilege, static_cast<uint32>(PrivilegeResult));
					});
				};

				PatchCheckCompletion(FOnlineError::Success(), ECheckForPackageUpdateResult::NoUpdateAvailable);

			}
			catch (Platform::Exception^ Ex)
			{
				UE_LOG_ONLINE_IDENTITY(Warning, TEXT("FOnlineIdentityLive::GetUserPrivilege failed with code 0x%0.8X."), Ex->HResult);

				DungeonsSubsystem->ExecuteNextTick([LiveUserId, Privilege, Delegate]()
				{
					Delegate.ExecuteIfBound(*LiveUserId, Privilege, static_cast<uint32>(EPrivilegeResults::GenericFailure));
				});
			}
			catch (const std::exception& Ex)
			{
				UE_LOG_ONLINE_IDENTITY(Warning, TEXT("FOnlineIdentityLive::GetUserPrivilege failed with reason '%s'."), ANSI_TO_TCHAR(Ex.what()));

				DungeonsSubsystem->ExecuteNextTick([LiveUserId, Privilege, Delegate]()
				{
					Delegate.ExecuteIfBound(*LiveUserId, Privilege, static_cast<uint32>(EPrivilegeResults::GenericFailure));
				});
			}
		});
	}
	catch (Platform::Exception^ Ex)
	{
		UE_LOG_ONLINE_IDENTITY(Warning, TEXT("FOnlineIdentityLive::GetUserPrivilege starting failed with code 0x%0.8X."), Ex->HResult);
		Delegate.ExecuteIfBound(UserId, Privilege, static_cast<uint32>(EPrivilegeResults::GenericFailure));
	}
	catch (const std::exception& Ex)
	{
		UE_LOG_ONLINE_IDENTITY(Warning, TEXT("FOnlineIdentityLive::GetUserPrivilege starting failed with reason '%s'."), ANSI_TO_TCHAR(Ex.what()));
		Delegate.ExecuteIfBound(UserId, Privilege, static_cast<uint32>(EPrivilegeResults::GenericFailure));
	}
}
#endif

void FOnlineIdentityDungeonsXboxOne::OnUserAdded(Windows::Xbox::System::User^ InUserAdded)
{
	auto XboxInputInterface = GetXboxInputInterface();

	ELoginStatus::Type LoginStatus = InUserAdded ? ELoginStatus::LoggedIn : ELoginStatus::NotLoggedIn;
	const FPlatformUserId PlatformUserId = XboxInputInterface->GetPlatformUserIdFromXboxUser(InUserAdded);

	FUniqueNetIdDungeonsRef UserAdded = GetNetIdFromUser(InUserAdded);

	UE_LOG_ONLINE_IDENTITY(Log, TEXT("UserAdded PlatformUserId %d"), PlatformUserId);

	TriggerOnLoginStatusChangedDelegates(PlatformUserId, (LoginStatus != ELoginStatus::LoggedIn) ? ELoginStatus::LoggedIn : ELoginStatus::NotLoggedIn, LoginStatus, *UserAdded);
}

// D11.PC
void FOnlineIdentityDungeonsXboxOne::OnUserRemoved(Windows::Xbox::System::User^ InUserRemoved)
{
	auto XboxInputInterface = GetXboxInputInterface();

	const FPlatformUserId PlatformUserId = XboxInputInterface->GetPlatformUserIdFromXboxUser(InUserRemoved);
	FUniqueNetIdDungeonsRef UserRemoved = GetNetIdFromUser(InUserRemoved);
	UE_LOG_ONLINE_IDENTITY(Log, TEXT("UserRemoved PlatformUserId %d"), PlatformUserId);

	TriggerOnLoginStatusChangedDelegates(PlatformUserId, ELoginStatus::LoggedIn, ELoginStatus::NotLoggedIn, *UserRemoved);
	//D11.PS This is needed to fix a CFR issue where the users presence says they have left the title if guest account signs out.
	SetAllUsersActiveInTitle();
}

// D11.PC
void FOnlineIdentityDungeonsXboxOne::OnEngineInitComplete()
{
	auto XboxInputInterface = GetXboxInputInterface();
	if (ensure(XboxInputInterface.IsValid()))
	{
		XboxInputInterface->OnUserAddedDelegates.AddRaw(this, &FOnlineIdentityDungeonsXboxOne::OnUserAdded);
		XboxInputInterface->OnUserRemovedDelegates.AddRaw(this, &FOnlineIdentityDungeonsXboxOne::OnUserRemoved);
	}
}

void FOnlineIdentityDungeonsXboxOne::OnControllerPairingChange(int32 InControllerIndex, FPlatformUserId InNewUserId, FPlatformUserId InOldUserId)
{
	UE_LOG_ONLINE_IDENTITY(Log, TEXT("FOnlineIdentityDungeonsXboxOne OnControllerPairingChange ControllerId %d"), InControllerIndex);
	auto XboxInputInterface = GetXboxInputInterface();

	Windows::Xbox::System::User^ NewUser = XboxInputInterface->GetXboxUserFromPlatformUserId(InNewUserId);
	Windows::Xbox::System::User^ OldUser = XboxInputInterface->GetXboxUserFromPlatformUserId(InOldUserId);

	FUniqueNetIdDungeonsRef PreviousUserNetId = GetNetIdFromUser(OldUser);
	FUniqueNetIdDungeonsRef NewUserNetId = GetNetIdFromUser(NewUser);

	if (InControllerIndex != -1)
	{
		TriggerOnControllerPairingChangedDelegates(InControllerIndex, *PreviousUserNetId, *NewUserNetId);
	}
}


void FOnlineIdentityDungeonsXboxOne::HookEvents()
{
	ControllerPairingChanged = FCoreDelegates::OnControllerPairingChange.AddRaw(this, &FOnlineIdentityDungeonsXboxOne::OnControllerPairingChange);
}

void FOnlineIdentityDungeonsXboxOne::UnHookEvents()
{
	FCoreDelegates::OnControllerPairingChange.Remove(ControllerPairingChanged);
	RemoveConnectionStateChangeHandler();
}

void FOnlineIdentityDungeonsXboxOne::ConnectionStateChanged(void* context, XblRealTimeActivityConnectionState state)
{
	auto Identity = static_cast<FOnlineIdentityDungeonsXboxOne*>(context);
	if (state != Identity->CurrentConnectionState)
	{
		Identity->CurrentConnectionState = state;
		if (XblRealTimeActivityConnectionState::Disconnected == state) {
			auto DungeonsSubsystem = Identity->DungeonsSubsystem;
			DungeonsSubsystem->QueueAsyncTask([DungeonsSubsystem]() {
				DungeonsSubsystem->GetSessionInterface()->TriggerOnSessionFailureDelegates(FUniqueNetIdString("XboxRTAConnectionLost"), ESessionFailure::ServiceConnectionLost);
			});
		}
	}
}

void FOnlineIdentityDungeonsXboxOne::AddConnectionStateChangeHandler() {
	if (IsConnectionStateHandlerInitialized())
		return;
	if (auto userContext = GetXBLContextHandle(0)) {
		ConnectionChangeContext = XblRealTimeActivityAddConnectionStateChangeHandler(userContext, ConnectionStateChanged, this);
	}
}

void FOnlineIdentityDungeonsXboxOne::RemoveConnectionStateChangeHandler() {
	if (!IsConnectionStateHandlerInitialized())
		return;

	if (auto userContext = GetXBLContextHandle(0)) {
		HrCheck(
			XblRealTimeActivityRemoveConnectionStateChangeHandler(userContext, ConnectionChangeContext)
			, "[Identity][XboxOne] XblRealTimeActivityRemoveConnectionStateChangeHandler"
		);
		ConnectionChangeContext = 0;
	}
}


bool FOnlineIdentityDungeonsXboxOne::ShouldSuccessCallbackOnAuthTokenFailure()
{
	return true;
}

void FOnlineIdentityDungeonsXboxOne::ChangeUserInfo(std::shared_ptr<XBLUserInfo>& UserToChange)
{
	if (UserToChange->UserId == 0)
	{
		Windows::Xbox::System::User^ XboxUser = GetXboxInputInterface()->GetXboxUserFromControllerId(UserToChange->LocalUserNum);
		if (XboxUser)
		{
			std::string UserIdString;
			GetUserIdString(XboxUser, UserIdString);
			UserToChange->UserId = std::stoll(UserIdString);
			UserToChange->SystemId = GetXboxInputInterface()->GetPlatformUserIdFromXboxUser(XboxUser);

			std::wstring UserGamertagDisplayInfo(XboxUser->DisplayInfo->Gamertag->Begin());
			std::string userGamertag = std::string(UserGamertagDisplayInfo.begin(), UserGamertagDisplayInfo.end());
			UserToChange->SetGamerTag(userGamertag.c_str());
		}
	}
	AddConnectionStateChangeHandler();
	UpdateUserSpeechAccessiblity(UserToChange);
}

void FOnlineIdentityDungeonsXboxOne::InitializePlatform()
{
	const auto queueHandle = GetDungeonsOnlineSubsystem()->GetQueueHandle();
	//XBL init
	XblInitArgs args = { };
	args.queue = queueHandle;

	//Xal init - D11.PS
	XalInitArgs xalInitArgs = {};
	HRESULT hr = XalInitialize(&xalInitArgs, queueHandle);

	XblInitialize(&args);

	AsyncInitialized = true;

	HookEvents();
	FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FOnlineIdentityDungeonsXboxOne::OnEngineInitComplete);

	/** Token for un-registering the signin callback */
	EventHandler<SignOutStartedEventArgs^>^ SignOutStartedEvent = ref new EventHandler<SignOutStartedEventArgs^>(
		[this](Platform::Object^, SignOutStartedEventArgs^ EventArgs)
	{
		// Since this event is called from non-game threads, marshal the call to the game thread
		// to ensure that data access is safe.
		DungeonsSubsystem->ExecuteNextTick([this, EventArgs]()
		{
			FString XUID = FString(EventArgs->User->XboxUserId->Begin());

			int32 FirstLocalNum = GetFirstLocalUserNumber();
			if (FirstLocalNum == -1)
			{
				return;
			}

			auto PrimaryUser = GetUserFromLocalUser(FirstLocalNum);
			if (!PrimaryUser)
			{
				return;
			}

			if (!FCString::IsNumeric(*XUID))
			{
				return;
			}

			//Get XUID for the user that is signing out
			uint64_t UXUID = FCString::Strtoui64(*XUID, nullptr, 10);

			if (UXUID != PrimaryUser->UserId)
			{
				//Do not trigger destroy session for guest users
				return;
			}

			if (!DungeonsSubsystem->GetSessionInterface()->GetNamedSession(DungeonsGameSessionName))
			{
				//If not in a session, do nothing
				return;
			}

			DungeonsSubsystem->GetSessionInterface()->DestroySession(DungeonsGameSessionName);
		});
	});

	SignOutStartedToken = Windows::Xbox::System::User::SignOutStarted += SignOutStartedEvent;

}

void FOnlineIdentityDungeonsXboxOne::PlatformTick(float DeltaTime)
{

}

bool FOnlineIdentityDungeonsXboxOne::Login(int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials)
{
	SignInWithUI(LocalUserNum); //First user, make sure we setup everything.
	return true;
}

bool FOnlineIdentityDungeonsXboxOne::Logout(int32 LocalUserNum)
{
	if (GetLoginStatus(LocalUserNum) == ELoginStatus::NotLoggedIn)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Identity] Failed to logout, LocalUserNum '%d' not signed in"), LocalUserNum);
		return false;
	}


	TryReleasePeer2Peer(LocalUserNum);

	RemoveLocalUser(LocalUserNum);

	return true;
}

TSharedPtr<const FUniqueNetId> FOnlineIdentityDungeonsXboxOne::GetSponsorUniquePlayerId(int32 ControllerIndex) const
{
	FXboxOneApplication* XboxOneApp = FXboxOneApplication::GetXboxOneApplication();
	if (XboxOneApp == nullptr)
	{
		return nullptr;
	}
	auto XboxInputInterface = XboxOneApp->GetXboxInputInterface();

	Windows::Xbox::System::User^ User = XboxInputInterface->GetXboxUserFromPlatformUserId(ControllerIndex);
	if (User != nullptr)
	{
		if (User->Sponsor != nullptr)
		{
			return GetNetIdFromUser(User);
		}
	}

	return nullptr;
}

int FOnlineIdentityDungeonsXboxOne::GetUserIDFromUser(Windows::Xbox::System::User^ user)
{
	std::string UserID;
	GetUserIdString(user, UserID);
	return std::stoull(UserID.c_str());
}


void FOnlineIdentityDungeonsXboxOne::SetAllUsersActiveInTitle()
{
	//D11.PS this is a bit of a hack but it needs to be done to ensure that the main users presence does not disappear when signing out of a guest account.
	//This seems like a system or XSAPI issue but its been reported as a CFR and we must have this fixed for certification.
	for (const auto it : LocalUsers) 
	{
		XblPresenceSetPresenceAsync(it.second->XBLcontext, true, nullptr, AsyncTasks::CreateAsyncBlock([](XAsyncBlock* asyncBlock)
		{

		}, GetDungeonsOnlineSubsystem()->GetQueueHandle()));
	}
}


void FOnlineIdentityDungeonsXboxOne::UpdateUserSpeechAccessiblity(std::shared_ptr<XBLUserInfo>& UserInfo)
{
	XblContextHandle contextHandle = GetXBLContextHandle(UserInfo->LocalUserNum);

	//This seems like a very cumbersome way of doing this but its the only way. Maybe there are better json parses in UE4 than the one I have chosen to use.

	XblHttpCallHandle httpCall = nullptr;
	if (HrSuccess(XblHttpCallCreate(contextHandle, "GET", "https://profile.xboxlive.com/users/me/profile/settings?settings=SpeechAccessibility", &httpCall), "[OnlineIdentityXboxOne] UpdateUserSpeechAccessiblity"))
	{
		//XblHttpCallRequestSetRetryAllowed(Call, true);
		XblHttpCallRequestSetHeader(httpCall, "x-xbl-contract-version", "2", true);
		XblHttpCallRequestSetRequestBodyString(httpCall, "");

		auto result = XblHttpCallPerformAsync(httpCall,
			XblHttpCallResponseBodyType::String,
			AsyncTasks::CreateAsyncBlock([httpCall, UserInfo](XAsyncBlock* asyncBlock)
		{
			uint32_t status = 0;
			const char* errMsg = nullptr;

			HrLog(XblHttpCallGetStatusCode(httpCall, &status), "[OnlineIdentityXboxOne]  Get status code");
			HrLog(XblHttpCallGetPlatformNetworkErrorMessage(httpCall, &errMsg), "[OnlineIdentityXboxOne]  Get platform network error message");

			const char *responseStr = nullptr;

			if (HrSuccess(XblHttpCallGetResponseString(httpCall, &responseStr), "[OnlineIdentityXboxOne]  XblHttpCallGetResponseString"))
			{
				TSharedPtr<FJsonObject> rootJsonObject;
				TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(FString(responseStr));
				if (FJsonSerializer::Deserialize(reader, rootJsonObject))
				{
					const TArray<TSharedPtr<FJsonValue>> *profiles;
 					if (rootJsonObject->TryGetArrayField(TEXT("profileUsers"), profiles) && profiles->Num() > 0)
 					{
						if (const TSharedPtr<FJsonObject> profileObj = ((*profiles)[0])->AsObject()) //0 is always the main profile from what I can tell
						{
							const TArray<TSharedPtr<FJsonValue>> *settings;
							if (profileObj->TryGetArrayField(TEXT("settings"), settings))
							{
								for (TSharedPtr<FJsonValue> setting : *settings)
								{
									if (const TSharedPtr<FJsonObject> settingsObj = setting->AsObject())
									{
										FString id;
										if (settingsObj->TryGetStringField("id", id))
										{
											if (id == "SpeechAccessibility")
											{
												FString speechVal;
												if (settingsObj->TryGetStringField("value", speechVal))
												{
													bool textToSpeechEnabled = speechVal.Find(TEXT("\"GameTextSS\":true")) > 0;
													UserInfo->SetTextToSpeechEnabled(textToSpeechEnabled);
												}
											}
										}
									}
								}
							}
						}

					}
				}
			}
			
			XblHttpCallCloseHandle(httpCall);
		}, DungeonsSubsystem->GetQueueHandle()));
	}
}


#endif
