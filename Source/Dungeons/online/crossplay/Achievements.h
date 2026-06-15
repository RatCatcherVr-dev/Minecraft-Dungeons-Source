#pragma once
#include "SubOSS.h"

namespace online {
namespace Crossplay {

class Achievements : public SubOSS {
public:
	Achievements(const SubsystemRepo&);
	void WriteAchievements(const FUniqueNetId& PlayerId, FOnlineAchievementsWriteRef& WriteObject, const FOnAchievementsWrittenDelegate& Delegate = FOnAchievementsWrittenDelegate());
	void QueryAchievements(const FUniqueNetId& PlayerId, const FOnQueryAchievementsCompleteDelegate& Delegate = FOnQueryAchievementsCompleteDelegate());
	EOnlineCachedResult::Type GetCachedAchievements(const FUniqueNetId& PlayerId, TArray<FOnlineAchievement>& OutAchievements);
	TSharedPtr<const FUniqueNetId> GetUniqueId(int LocalUserNum) const;

#ifdef HAS_ACHIEVEMENT_WRITTEN_TRACKER_DELEGATE
	FOnAchievementWrittenTracker OnAchievementWrittenTrackerDelegates;
#endif
private:
	IOnlineSubsystem* GetFirstActive() const;
	
};
}
}
