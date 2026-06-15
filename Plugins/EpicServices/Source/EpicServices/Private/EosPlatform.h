#pragma once

#include "Delegates/IDelegateInstance.h"
#include "Logging/LogMacros.h"

#include <eos_sdk.h>

namespace minecraft {
	namespace epicstore {

		DECLARE_LOG_CATEGORY_EXTERN(LogEpicOnlineSDK, Log, All);
		
		class FEosPlatform
		{
		public:
			FEosPlatform() = default;
			FEosPlatform(FEosPlatform const&) = delete;
			FEosPlatform& operator=(FEosPlatform const&) = delete;

			void Init();
			void Shutdown();
			bool IsInitialized() const { return bIsInitialized; }
			EOS_HPlatform const GetHandle() const { return PlatformHandle; }

		private:
			bool InitPlatform();
			bool CreatePlatform();
			bool Tick(float);

			bool bIsInitialized{ false };
			EOS_HPlatform PlatformHandle{ nullptr };
			FDelegateHandle TickDelegateHandle{};
		};
	}
}
