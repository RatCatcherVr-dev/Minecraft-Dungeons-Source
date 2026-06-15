#include "EpicStore.h"
#include "EosPlatform.h"

#include <eos_ecom.h>

namespace minecraft {
	namespace epicstore {

		void FEpicStore::QueryEntitlements( EOS_EpicAccountId LocalUserId,
											FOnEpicEntitlementsReceived successCallback,
											FOnEpicEntitlementsRequestFailed failedCallback)
		{
			OnEpicEntitlementsReceived = successCallback;
			OnEpicEntitlementsRequestFailed = failedCallback;
			QueryEntitlements(LocalUserId);
		}

		void FEpicStore::QueryEntitlements(EOS_EpicAccountId LocalUserId)
		{
			if (!Platform.IsInitialized())
			{
				UE_LOG(LogEpicOnlineSDK, Warning, TEXT("[EOS SDK] Can't Query the Entitlements - EOS SDK not Initialized!"));
				OnEpicEntitlementsRequestFailed.ExecuteIfBound();
				return;
			}

			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Querying the Entitlements"));
			EOS_HPlatform platformHandle = Platform.GetHandle();
			EOS_HEcom EcomHandle = EOS_Platform_GetEcomInterface(platformHandle);

			EOS_Ecom_QueryEntitlementsOptions QueryOptions{ 0 };
			QueryOptions.ApiVersion = EOS_ECOM_QUERYENTITLEMENTS_API_LATEST;
			QueryOptions.LocalUserId = LocalUserId;
			QueryOptions.bIncludeRedeemed = true;

			EOS_Ecom_QueryEntitlements(EcomHandle, &QueryOptions, this, EntitlementsReceivedCallback);
		}

		void EOS_CALL FEpicStore::EntitlementsReceivedCallback(EOS_Ecom_QueryEntitlementsCallbackInfo const *EntitlementData)
		{
			ensure(EntitlementData);
			FEpicStore *Store = static_cast<FEpicStore*>(EntitlementData->ClientData);
			ensure(Store);

			if (EntitlementData->ResultCode != EOS_EResult::EOS_Success)
			{
				UE_LOG(LogEpicOnlineSDK, Error, TEXT("[EOS SDK] Query entitlement error: %s"), UTF8_TO_TCHAR(EOS_EResult_ToString(EntitlementData->ResultCode)));
				Store->OnEpicEntitlementsRequestFailed.ExecuteIfBound();
				return;
			}
			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Entitlements received"));

			EOS_Ecom_GetEntitlementsCountOptions CountOptions{ 0 };
			CountOptions.ApiVersion = EOS_ECOM_GETENTITLEMENTSCOUNT_API_LATEST;
			CountOptions.LocalUserId = EntitlementData->LocalUserId;
			EOS_HPlatform platformHandle = Store->Platform.GetHandle();
			EOS_HEcom EcomHandle = EOS_Platform_GetEcomInterface(platformHandle);
			uint32_t EntitlementCount = EOS_Ecom_GetEntitlementsCount(EcomHandle, &CountOptions);

			UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] NumEntitlements: %d"), EntitlementCount);
			TArray<FEntitlementData> NewEntitlements;
			TArray<FString> EntitlementNames;

			EOS_Ecom_CopyEntitlementByIndexOptions IndexOptions{ 0 };
			IndexOptions.ApiVersion = EOS_ECOM_COPYENTITLEMENTBYINDEX_API_LATEST;
			IndexOptions.LocalUserId = EntitlementData->LocalUserId;
			for (IndexOptions.EntitlementIndex = 0; IndexOptions.EntitlementIndex < EntitlementCount; ++IndexOptions.EntitlementIndex)
			{
				EOS_Ecom_Entitlement* Entitlement;
				EOS_EResult CopyResult = EOS_Ecom_CopyEntitlementByIndex(EcomHandle, &IndexOptions, &Entitlement);

				switch (CopyResult)
				{
				case EOS_EResult::EOS_Success:
				case EOS_EResult::EOS_Ecom_EntitlementStale:
					UE_LOG(LogEpicOnlineSDK, Log, TEXT("[EOS SDK] Entitlement[%d] : %s : %s : %s"),
						IndexOptions.EntitlementIndex,
						UTF8_TO_TCHAR(Entitlement->EntitlementName),
						UTF8_TO_TCHAR(Entitlement->EntitlementId),
						Entitlement->bRedeemed ? "TRUE" : "FALSE"
					);

					NewEntitlements.Emplace(
						EntitlementData->LocalUserId,
						FString(UTF8_TO_TCHAR(Entitlement->EntitlementName)),
						FString(UTF8_TO_TCHAR(Entitlement->EntitlementId)),
						Entitlement->bRedeemed == EOS_TRUE
					);

					EntitlementNames.Emplace(
						FString(UTF8_TO_TCHAR(Entitlement->EntitlementName))
					);

					EOS_Ecom_Entitlement_Release(Entitlement);
					break;
				default:
					UE_LOG(LogEpicOnlineSDK, Warning, TEXT("[EOS SDK] Entitlement[%d] invalid : %d"), IndexOptions.EntitlementIndex, CopyResult);
					break;
				}
			}
			if (Store->OnEpicEntitlementsReceived.IsBound())
			{
				Store->OnEpicEntitlementsReceived.Execute(EntitlementNames);
				Store->OnEpicEntitlementsReceived.Unbind();
			}
			if (Store->OnEpicEntitlementsRequestFailed.IsBound())
				Store->OnEpicEntitlementsRequestFailed.Unbind();
		}
	}
}
