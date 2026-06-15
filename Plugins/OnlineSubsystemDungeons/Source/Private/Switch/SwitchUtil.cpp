#include "SwitchUtil.h"

#include "OnlineSubsystem.h"
#include <nn/account.h>
#include <nn/account/account_Result.h>
#include <nn/account/account_ApiNetworkServiceLicense.h>
#include <nn/friends/friends_Result.h>
#include <nn/ssl/ssl_Result.h>
#include <nn/err/err_ErrorResultVariant.h>

namespace SwitchUtil
{
	FString GetErrorCodeFromResult(const nn::err::ErrorResultVariant& ErrorResultVariant)
	{
		switch (ErrorResultVariant.GetState())
		{
		case nn::err::ErrorResultVariantState_HasErrorCode:
		{
			nn::err::ErrorCode ErrorCode = ErrorResultVariant;
			return FString::Printf(TEXT("%03u-%04u"), ErrorCode.category, ErrorCode.number);
		}
		case nn::err::ErrorResultVariantState_HasResult:
		{
			const nn::Result Result = ErrorResultVariant;
			return FString::Printf(TEXT("%03d-%04d-0x%08x"), Result.GetModule(), Result.GetDescription(), Result.GetInnerValueForDebug());
		}
		case nn::err::ErrorResultVariantState_HasNone:
			// Fall-through
			break;
		}

		return FString(TEXT("UnknownError"));
	}


	void HandleNintendoError(const nn::Result& Error, const TCHAR* APICall)
	{
		if (Error.IsFailure() && APICall)
		{
			UE_LOG_ONLINE(Log, TEXT("Error returned from %s %s"), APICall, *GetErrorCodeFromResult(Error));

			if (nn::account::ResultNetworkCommunicationError::Includes(Error)
				|| nn::friends::ResultInternetRequestNotAccepted::Includes(Error)
				|| nn::friends::ResultHttpError::Includes(Error)
				|| nn::friends::ResultServerError::Includes(Error)
				|| nn::account::ResultNintendoAccountAuthorizationInteractionRequired::Includes(Error)
				|| nn::account::ResultUserNotExist::Includes(Error)
				|| nn::account::ResultCancelled::Includes(Error)
				|| nn::account::ResultCancelled::Includes(Error)
#if WITH_NEX_LIBRARY
				|| nn::nex::ResultTransportConnectionFailure::Includes(Error)
				|| nn::nex::ResultAuthenticationUnderMaintenance::Includes(Error)
				|| nn::nex::ResultAuthenticationApplicationVersionIsOld::Includes(Error)
#endif // WITH_NEX_LIBRARY
				|| nn::ssl::ResultSslService::Includes(Error))
			{
				FSwitchPlatformMisc::ShowErrorDialog(Error);
			}
		}
	}

	bool RecoverFromNintendoAccountError(const nn::account::UserHandle& UserHandle, const nn::Result& InResult, const TCHAR* APICall, nn::Result* OutResult)
	{
		nn::Result Result = InResult;

		if (nn::account::ResultNetworkServiceAccountUnavailable::Includes(Result))
		{
			Result = nn::account::EnsureNetworkServiceAccountAvailable(UserHandle);
			if (Result.IsSuccess())
			{
				return true;
			}
			else
			{
				APICall = TEXT("nn::account::EnsureNetworkServiceAccountAvailable");
			}
		}

		HandleNintendoError(InResult, APICall);

		if (OutResult)
		{
			*OutResult = Result;
		}

		return false;
	}

	static inline FString ANSIToString(const char* Buffer, size_t Length)
	{
		auto ConvertedString = StringCast<TCHAR>(Buffer, Length);
		return FString(ConvertedString.Length(), ConvertedString.Get());
	}

	//-------------------------------------------------------------------------------------------------------------------

	FDungeonsSwitchNSACheck::FDungeonsSwitchNSACheck(int LocalUserNum, FSwitchNSCheckResultFunc func)
		: Uid(nn::account::InvalidUid)
		, ResultFunc(func)
		, Offline(false)
	{
		FSwitchPlatformMisc::GetUidForControllerId(LocalUserNum, Uid);
	}

	void FDungeonsSwitchNSACheck::Initialize()
	{
		if (Uid == nn::account::InvalidUid || !FSwitchPlatformMisc::GetOpenedUserHandle(UserHandle, Uid))
		{
			bWasSuccessful = false;
			bIsComplete = true;
		}
		//No longer do this here, we only want to do this when logging in and on priv check.
		//FSwitchPlatformMisc::SetNifmRequestPolicy(FSwitchPlatformMisc::ENifmRequestPolicy::Default);
	}

	bool FDungeonsSwitchNSACheck::CheckOnline()
	{
		if (!FSwitchPlatformMisc::ConditionalStartNifm())
		{
			bWasSuccessful = false;
			bIsComplete = true;
			Offline = true;
			return false;
		}
		return true;
	}

	void FDungeonsSwitchNSACheck::Finalize()
	{
		ResultFunc(bWasSuccessful, Offline);
	}

	void FDungeonsSwitchNSACheck::Tick()
	{
		nn::Result Result = nn::ResultSuccess();
		const TCHAR* APICall;

		if (!CheckOnline())
		{
			return;
		}

		do
		{
			APICall = TEXT("nn::account::GetNetworkServiceAccountId");
			Result = nn::account::GetNetworkServiceAccountId(&NetworkServiceAccountId, UserHandle);
		} while (RecoverFromNintendoAccountError(UserHandle, Result, APICall, &Result));


		if (Result.IsSuccess())
		{
			do
			{
				nn::account::AsyncContext AsyncContext;
				APICall = TEXT("nn::account::EnsureNetworkServiceAccountIdTokenCacheAsync");
				Result = nn::account::EnsureNetworkServiceAccountIdTokenCacheAsync(&AsyncContext, UserHandle);
				if (Result.IsSuccess())
				{
					nn::os::SystemEvent e;
					AsyncContext.GetSystemEvent(&e);
					e.Wait();
					APICall = TEXT("nn::account::AsyncContext::GetResult");
					Result = AsyncContext.GetResult();
					if (Result.IsSuccess())
					{
						size_t IdTokenLength = 0;
						char NsaIdToken[nn::account::NetworkServiceAccountIdTokenLengthMax];
						APICall = TEXT("nn::account::LoadNetworkServiceAccountIdTokenCache");
						Result = nn::account::LoadNetworkServiceAccountIdTokenCache(&IdTokenLength, NsaIdToken, nn::account::NetworkServiceAccountIdTokenLengthMax, UserHandle);

						if (Result.IsSuccess())
						{
							NetworkServiceAccountIdToken = ANSIToString(NsaIdToken, IdTokenLength);
							bIsComplete = true;
							bWasSuccessful = true;
							return;
						}
					}
				}
			} while (nn::account::ResultTokenCacheUnavailable::Includes(Result) || RecoverFromNintendoAccountError(UserHandle, Result, APICall, &Result));
		}

		UE_LOG_ONLINE(Warning, TEXT("NSA token error: %d - %d"), Result.GetModule(), Result.GetDescription());

		if (nn::account::ResultNetworkCommunicationError::Includes(Result))
		{
			Offline = true;
		}

		bIsComplete = true;
		bWasSuccessful = false;
	}



	FDungeonsSwitchNSOCheck::FDungeonsSwitchNSOCheck(int LocalUserNum, FSwitchNSCheckResultFunc func)
		: FDungeonsSwitchNSACheck(LocalUserNum, func)
	{
	}

	void FDungeonsSwitchNSOCheck::Tick()
	{
		//Do NSA check first
		FDungeonsSwitchNSACheck::Tick();

		if (!bWasSuccessful)
		{
			return;
		}
		bIsComplete = false;

		nn::Result Result = nn::ResultSuccess();
		const TCHAR* APICall;

		do
		{
			nn::account::AsyncNetworkServiceLicenseInfoContext  LicenseInfoContext;
			APICall = TEXT("nn::account::EnsureNetworkServiceAccountIdTokenCacheAsync");
			Result = nn::account::LoadNetworkServiceLicenseInfoAsync(&LicenseInfoContext, UserHandle);

			if (Result.IsSuccess())
			{
				nn::os::SystemEvent e;
				LicenseInfoContext.GetSystemEvent(&e);
				e.Wait();
				APICall = TEXT("nn::account::AsyncNetworkServiceLicenseInfoContext::GetResult");
				Result = LicenseInfoContext.GetResult();

				if (Result.IsSuccess())
				{
					if (LicenseInfoContext.GetNetworkServiceLicenseKind() == nn::account::NetworkServiceLicenseKind::NetworkServiceLicenseKind_Common)
					{
						bIsComplete = true;
						bWasSuccessful = true;
						return;
					}
					else
					{
						//This throws up the popup to sign out to NSO
						APICall = TEXT("nn::account::ShowLicenseRequirementsForNetworkService::GetResult");
						Result = nn::account::ShowLicenseRequirementsForNetworkService(UserHandle);

						if (Result.IsSuccess())
						{
							return;
						}
					}
				}
			}


		} while (RecoverFromNintendoAccountError(UserHandle, Result, APICall, &Result));


		UE_LOG_ONLINE(Warning, TEXT("NSO token error: %d - %d"), Result.GetModule(), Result.GetDescription());

		bIsComplete = true;
		bWasSuccessful = false;
	}



} //namespace SwitchUtil