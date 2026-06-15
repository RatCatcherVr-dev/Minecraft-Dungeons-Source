#pragma once

#include "HAL/Platform.h"

#include <eos_sdk.h>

namespace minecraft {
	namespace epicstore {

		TCHAR* EpicAccountIDToString(EOS_EpicAccountId InAccountId);
		TCHAR* ProductUserIDToString(EOS_ProductUserId InAccountId);

		/**
		 * Simple wrapper around account ID. Allows easy conversion and adds handy operators.
		 */
		template<class TAccountType>
		struct TEpicAccountId
		{
			TEpicAccountId(TAccountType InAccountId) : AccountId(InAccountId) {}
			TEpicAccountId() = default;
			TEpicAccountId(TEpicAccountId const&) = default;
			TEpicAccountId& operator=(TEpicAccountId const&) = default;

			bool operator==(TEpicAccountId const &Other) const
			{
				return AccountId == Other.AccountId;
			}

			bool operator!=(TEpicAccountId const &Other) const
			{
				return !(this->operator==(Other));
			}

			bool operator<(TEpicAccountId const &Other) const
			{
				return AccountId < Other.AccountId;
			}

			/* Easy conversion to EOS account ID. */
			operator TAccountType() const
			{
				return AccountId;
			}

			/* EOS Account Id */
			TAccountType AccountId{ nullptr };
		};

		using FEpicAccountId = TEpicAccountId<EOS_EpicAccountId>;
		using FProductUserId = TEpicAccountId<EOS_ProductUserId>;

		FORCEINLINE uint32 GetTypeHash(FEpicAccountId const &account)
		{
			return FCrc::MemCrc32(&account.AccountId, sizeof(account.AccountId));
		}

		FORCEINLINE uint32 GetTypeHash(FProductUserId const &account)
		{
			return FCrc::MemCrc32(&account.AccountId, sizeof(account.AccountId));
		}

		static constexpr TCHAR* LoginStatusStrings[]{
			TEXT("NotLoggedIn"),
			TEXT("UsingLocalProfile"),
			TEXT("LoggedIn")
		};

	}
}