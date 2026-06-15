#pragma once
#include <Anticheat.hpp>

namespace minecraft {
	namespace epicstore {
		namespace productconfig {
			/** The product id for the running application, found on the dev portal */
			static char const *ProductId = ANTICHEAT_STRINGS("cf0d959025704790801a3152d6cbc023");

			/** Client id of the service permissions entry, found on the dev portal */
			static char const *ClientCredentialsId = ANTICHEAT_STRINGS("xyza7891hf80FEjU32r0xEZbuYZFT872");

			/** Client secret for accessing the set of permissions, found on the dev portal */
			static char const *ClientCredentialsSecret = ANTICHEAT_STRINGS("h6179YNiGpXlO9x1ACbUMePqChcw05QC6JiE2B9uuUE");

			/** The sandbox id for the running application, found on the dev portal */
			static char const *SandboxId = ANTICHEAT_STRINGS("4a578f84d09d4f17b4ea75f9348f263f");

			/** The deployment id for the running application, found on the dev portal */
			static char const *DeploymentId = ANTICHEAT_STRINGS("325e5ca7a71846d7b803cd73bed7b21e");

#if UE_BUILD_SHIPPING
			/** URI to the game in the Epic Games Launcher, this is public info */
			static constexpr TCHAR *LauncherGameURL = TEXT("com.epicgames.launcher://apps/b0079867a2504421bdd920c01d7dd163?action=launch&silent=true");
#else 
			/** URI to the game in the Epic Games Launcher, this is public info */
			static constexpr TCHAR *LauncherGameURL = TEXT("com.epicgames.launcher://apps/8f39452926a547e5bf2046c39532c1b5?action=launch&silent=true");
#endif 
		}
	}
}
