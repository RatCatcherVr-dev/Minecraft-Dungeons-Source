#include "PlayfabPlayerCache.h"

#include <algorithm>

void PlayFabPlayerCache::RemovePlayer(const FString& userID)
{
	auto result = std::find_if(mPlayers.begin(), mPlayers.end(), [&userID](auto player) {
		return player.GetPlayerId() == userID;
	});

	if (result != mPlayers.end()) {
		mPlayers.erase(result);
	}
}

PlayFabPlayer* PlayFabPlayerCache::GetPlayFabPlayer(FString userID)
{
   auto result = std::find_if(mPlayers.begin(), mPlayers.end(), [&userID](auto player) {
      return player.GetPlayerId() == userID;
   });

   if (result == mPlayers.end())
      return nullptr;
   else
      return &(*result);
}

PlayFabPlayer* PlayFabPlayerCache::GetFirst()
{
	return &mPlayers.front();
}

bool PlayFabPlayerCache::HasPlayer(FString userID) const
{
   auto result = std::find_if(mPlayers.begin(), mPlayers.end(), [&userID](auto player) {
      return player.GetPlayerId() == userID;
   });

   return result != mPlayers.end();
}