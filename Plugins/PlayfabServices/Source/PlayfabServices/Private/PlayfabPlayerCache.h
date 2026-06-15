#pragma once

#include <vector>
#include "PlayfabPlayer.h"
#include <UnrealString.h>

class PlayFabPlayerCache
{
public:
   PlayFabPlayerCache() = default;

   void AddPlayer(PlayFabPlayer pfp) { mPlayers.emplace_back(pfp); }
   void RemovePlayer(const FString& userID);
   PlayFabPlayer* GetPlayFabPlayer(FString userID);
   PlayFabPlayer* GetFirst();
   bool HasPlayer(FString userID) const;

private:
   std::vector<PlayFabPlayer> mPlayers;

};