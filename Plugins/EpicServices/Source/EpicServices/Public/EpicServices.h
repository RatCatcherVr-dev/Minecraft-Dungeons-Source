#pragma once

#include "Templates/UniquePtr.h"
#include "Modules/ModuleManager.h"
#include "Delegates/DelegateCombinations.h"

namespace minecraft {
	namespace epicstore {

		DECLARE_DELEGATE_OneParam(FOnEpicEntitlementsReceived, TArray<FString>);
		DECLARE_DELEGATE(FOnEpicEntitlementsRequestFailed);
		DECLARE_DELEGATE(FOnEpicGameServicesSignInFailed);

		class EPICSERVICES_API FEpicServicesModule : public IModuleInterface
		{
		public:
			FEpicServicesModule();
			virtual void StartupModule() override;
			virtual void ShutdownModule() override;

			static FEpicServicesModule& Get();

			void SetEpicGameServicesSignInFailedCallback(FOnEpicGameServicesSignInFailed);
			
			/* Authenticate to Epic Account Services using the exchange code received from the Game Launcher.
			 * Done on game initialization.
			 */
			void SignIntoEpicAccountServices();

			/* Authenticate to Epic Game Services using the token from Xbox Live.
			 * Done after player logs into the Microsoft Account.
			 * bCurrentToken - has the token just been updated ?
			 */
			void SignIntoEpicGameServices(bool bCurrentToken, char const *XboxLiveToken);
			
			void RequestEntitlements(FOnEpicEntitlementsReceived, FOnEpicEntitlementsRequestFailed);

			/* Logout all accounts signed into the Epic Game Services.
			 * Player still remains signed into the Epic Account Services.
			 * Should be called when player switches the Microsoft Account.
			 */
			void LogoutOfEpicGameServices();

			void SetLastBootTimestamp(int64);
		private:
			class Impl;
			TUniquePtr<Impl> PImpl;
		};

	}
}
