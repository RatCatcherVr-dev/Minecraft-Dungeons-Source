#include "PlayerManager.h"

namespace minecraft {
	namespace epicstore {

		void FPlayerManager::AddPlayer(FEpicAccountId UserId)
		{
			UserIdToPlayers.Add(UserId, MakeShared<FPlayer>(UserId));
			bSignedInEpicAccountServices = UserIdToPlayers.Num() > 0;
		}

		void FPlayerManager::RemovePlayer(FEpicAccountId const &UserId)
		{
			if (auto player = UserIdToPlayers.Find(UserId))
			{
				UserIdToPlayers.Remove(UserId);
				auto productUserId = (*player)->GetProductUserID();
				if (productUserId) 
					ProductIdToPlayers.Remove(productUserId);

				bSignedInEpicAccountServices = UserIdToPlayers.Num() > 0;
				bSignedInEpicGameServices = ProductIdToPlayers.Num() > 0;
			}
		}

		PlayerPtr FPlayerManager::FindPlayer(FEpicAccountId const &UserId)
		{
			return *UserIdToPlayers.Find(UserId);
		}

		PlayerPtr FPlayerManager::FindPlayer(FProductUserId const &ProductUserId)
		{
			return *ProductIdToPlayers.Find(ProductUserId);
		}

		TArray<PlayerPtr> FPlayerManager::GetEASLoggedInPlayers()
		{
			TArray<PlayerPtr> players;
			UserIdToPlayers.GenerateValueArray(players);
			return players;
		}

		PlayerPtr FPlayerManager::GetCurrentPlayer()
		{
			TArray<PlayerPtr> players;
			UserIdToPlayers.GenerateValueArray(players);
			/* consider the first player to be the prime player */
			return players.Num() > 0 ? players[0] : nullptr;
		}

		void FPlayerManager::SetProductAccount(FEpicAccountId const &UserId, FProductUserId ProductUserId)
		{
			if (auto player = UserIdToPlayers.Find(UserId))
			{
				(*player)->SetProductUserID(ProductUserId);
				ProductIdToPlayers.Add(ProductUserId, *player);
				bSignedInEpicGameServices = ProductIdToPlayers.Num() > 0;
			}
		}

	}
}
