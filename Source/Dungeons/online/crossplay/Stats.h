#pragma once
#include "SubOSS.h"

namespace online {
namespace Crossplay {

class Stats : public SubOSS {
public:
	Stats(const SubsystemRepo&);

	// These calls had DUNGEONS_OSS ifdefs, so we should only call them in dungeons oss
	void UpdateStats(const TSharedRef<const FUniqueNetId> LocalUserId, const TArray<FOnlineStatsUserUpdatedStats>& UpdatedUserStats, const FOnlineStatsUpdateStatsComplete& Delegate = FOnlineStatsUpdateStatsComplete());
	void QueryStats(const TSharedRef<const FUniqueNetId> LocalUserId, const TSharedRef<const FUniqueNetId> StatsUser, const FOnlineStatsQueryUserStatsComplete& Delegate = FOnlineStatsQueryUserStatsComplete());

	TSharedPtr<const FUniqueNetId> GetUniqueId(int LocalUserNum) const;
};
}
}
