#pragma once
#include "AccountHelpers.h"

#include "Containers/Array.h"
#include "Containers/Map.h"
#include "HAL/ThreadSafeBool.h"

namespace minecraft {
	namespace epicstore {

		class FPlayer
		{
		public:
			FPlayer(FEpicAccountId const e) : UserId(e) {}
			FPlayer(FPlayer const&) = delete;
			FPlayer& operator=(FPlayer const&) = delete;

			FEpicAccountId const GetUserID() { return UserId; }
			FProductUserId const GetProductUserID() { return ProductUserId; }			

		private:
			void SetProductUserID(FProductUserId const p) { ProductUserId = p; }

			FEpicAccountId UserId;
			FProductUserId ProductUserId;

			friend class FPlayerManager;
		};

		using PlayerPtr = TSharedPtr<FPlayer>;

		class FPlayerManager
		{
		public:
			FPlayerManager() = default;
			FPlayerManager(FPlayerManager const&) = delete;
			FPlayerManager& operator=(FPlayerManager const&) = delete;

			/* returns all players signed into the Epic Account Services */
			TArray<PlayerPtr> GetEASLoggedInPlayers();

			void AddPlayer(FEpicAccountId);
			void RemovePlayer(FEpicAccountId const&);
			void SetProductAccount(FEpicAccountId const&, FProductUserId);
			PlayerPtr GetCurrentPlayer();

			PlayerPtr FindPlayer(FEpicAccountId const&);
			PlayerPtr FindPlayer(FProductUserId const&);

			/* Removes all the players signed into Epic Game Services */
			void RemoveAllPlayersEGS() { ProductIdToPlayers.Reset(); }
		private:
			/* Players signed into Epic Account Services */
			TMap<FEpicAccountId, PlayerPtr> UserIdToPlayers{};
			/* Players signed into Epic Game Services */
			TMap<FProductUserId, PlayerPtr> ProductIdToPlayers{};

			FThreadSafeBool bSignedInEpicGameServices{ false };
			FThreadSafeBool bSignedInEpicAccountServices{ false };

			friend class FEpicServicesModule;
		};

	}
}
