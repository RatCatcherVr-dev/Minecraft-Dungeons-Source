#include "AccountHelpers.h"
#include "EosPlatform.h"
#include "Logging/LogMacros.h"

namespace minecraft {
	namespace epicstore {

		TCHAR* EpicAccountIDToString(EOS_EpicAccountId InAccountId)
		{
			if (nullptr == InAccountId)
				return ANSI_TO_TCHAR("NULL");

			static char TempBuffer[EOS_EPICACCOUNTID_MAX_LENGTH + 1];
			int32_t TempBufferSize = sizeof(TempBuffer);
			EOS_EResult Result = EOS_EpicAccountId_ToString(InAccountId, TempBuffer, &TempBufferSize);

			if (Result == EOS_EResult::EOS_Success)
			{
				return UTF8_TO_TCHAR(TempBuffer);
			}
			UE_LOG(LogEpicOnlineSDK, Error, TEXT("[EOS SDK] Epic Account Id To String Error : %d"), (int32_t)Result);

			return ANSI_TO_TCHAR("ERROR");
		}
		
		TCHAR* ProductUserIDToString(EOS_ProductUserId InAccountId)
		{
			if (nullptr == InAccountId)
				return ANSI_TO_TCHAR("NULL");
		
			static char TempBuffer[EOS_PRODUCTUSERID_MAX_LENGTH + 1];
			int32_t TempBufferSize = sizeof(TempBuffer);
			EOS_EResult Result = EOS_ProductUserId_ToString(InAccountId, TempBuffer, &TempBufferSize);
		
			if (Result == EOS_EResult::EOS_Success)
			{
				return UTF8_TO_TCHAR(TempBuffer);
			}
			UE_LOG(LogEpicOnlineSDK, Error, TEXT("[EOS SDK] Epic Account Id To String Error: %d"), (int32_t)Result);
		
			return ANSI_TO_TCHAR("ERROR");
		}
	}
}
