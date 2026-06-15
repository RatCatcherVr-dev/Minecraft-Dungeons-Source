#pragma once
#include "Containers/Array.h"
#include "Delegates/DelegateCombinations.h"

#include <eos_sdk.h>

#include "AccountHelpers.h"

namespace minecraft {
	namespace epicstore {

		DECLARE_DELEGATE_OneParam(FOnEpicEntitlementsReceived, TArray<FString>);
		DECLARE_DELEGATE(FOnEpicEntitlementsRequestFailed);

		struct FEntitlementData
		{
			FEntitlementData() = default;

			FEntitlementData(FEpicAccountId acc, FString eName, FString eInstanceId, bool redeemed) 
				: UserId{ acc }, Name{ eName }, InstanceId{ eInstanceId }, bRedeemed{ redeemed }
			{}

			/** User associated with this entitlement */
			FEpicAccountId UserId;
			/** The EOS_Ecom_EntitlementName */
			FString Name;
			/** The EOS_Ecom_EntitlementInstanceId */
			FString InstanceId;
			/** If true then this entitlement has been retrieved */
			bool bRedeemed;
		};

		class FEosPlatform;

		class FEpicStore
		{
		public:
			FEpicStore(FEosPlatform const &p) : Platform{ p }
			{}

			FEpicStore(FEpicStore const&) = delete;
			FEpicStore& operator=(FEpicStore const&) = delete;

			/** Start a entitlement query for the user */
			void QueryEntitlements(EOS_EpicAccountId, FOnEpicEntitlementsReceived, FOnEpicEntitlementsRequestFailed);
			void QueryEntitlements(EOS_EpicAccountId);
			
			void SavePendingEntitlementRequest(FOnEpicEntitlementsReceived r, FOnEpicEntitlementsRequestFailed f) {
				OnEpicEntitlementsReceived = r;
				OnEpicEntitlementsRequestFailed = f;
			}

			bool HasPendingEntitlementRequest() const { return OnEpicEntitlementsReceived.IsBound() && OnEpicEntitlementsRequestFailed.IsBound(); }
		private:
			/** Static callback handler for Query Entitlement complete */
			static void EOS_CALL EntitlementsReceivedCallback(EOS_Ecom_QueryEntitlementsCallbackInfo const*);

			FOnEpicEntitlementsReceived OnEpicEntitlementsReceived{};
			FOnEpicEntitlementsRequestFailed OnEpicEntitlementsRequestFailed{};

			FEosPlatform const &Platform;
		};

	}
}
