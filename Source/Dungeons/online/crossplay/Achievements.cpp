#include "Achievements.h"
#include "SubsystemRepo.h"
#include "online/sessions/OnlineUtil.h"
#include "CrossplayOSS.h"
#include "Identity.h"

namespace online {
namespace Crossplay {
Achievements::Achievements(const SubsystemRepo& subsystems) : SubOSS(subsystems) {
#ifdef HAS_ACHIEVEMENT_WRITTEN_TRACKER_DELEGATE
	GetFirstActive()->GetAchievementsInterface()->OnAchievementWrittenTrackerDelegates.AddLambda([this](bool success, TSharedPtr<const FUniqueNetId> playerID, int achievementID, FString achievementName, float percentage,FString achievementURL, FString achievementTitle, FString achievementDesc) {
		if (this->OnAchievementWrittenTrackerDelegates.IsBound()) {
			this->OnAchievementWrittenTrackerDelegates.Broadcast(success, playerID, achievementID, achievementName, percentage, achievementURL, achievementTitle, achievementDesc);
		}
	});
#endif
}

void Achievements::WriteAchievements(const FUniqueNetId& PlayerId, FOnlineAchievementsWriteRef& WriteObject, const FOnAchievementsWrittenDelegate& Delegate) {
	check(IsCompatibleNetId(GetFirstActive(), PlayerId));
	GetFirstActive()->GetAchievementsInterface()->WriteAchievements(PlayerId, WriteObject, Delegate);
}

void Achievements::QueryAchievements(const FUniqueNetId& PlayerId, const FOnQueryAchievementsCompleteDelegate& Delegate) {
	check(IsCompatibleNetId(GetFirstActive(), PlayerId));
	GetFirstActive()->GetAchievementsInterface()->QueryAchievements(PlayerId, Delegate);
}
	
EOnlineCachedResult::Type Achievements::GetCachedAchievements(const FUniqueNetId& PlayerId, TArray<FOnlineAchievement>& OutAchievements) {
	check(IsCompatibleNetId(GetFirstActive(), PlayerId));
	return GetFirstActive()->GetAchievementsInterface()->GetCachedAchievements(PlayerId, OutAchievements);
}

IOnlineSubsystem* Achievements::GetFirstActive() const {
	return Subsystems.GetFirstActiveSubsystem({ SubsystemType::PS4, SubsystemType::Steam, SubsystemType::Dungeons, SubsystemType::Null });
}

TSharedPtr<const FUniqueNetId> Achievements::GetUniqueId(int LocalUserNum) const {
	return GetFirstActive()->GetIdentityInterface()->GetUniquePlayerId(LocalUserNum);
}



}
}

