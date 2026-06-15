#include "Stats.h"

namespace online {
namespace Crossplay {
Stats::Stats(const SubsystemRepo& subsystems) : SubOSS(subsystems){ }

void Stats::UpdateStats(const TSharedRef<const FUniqueNetId> LocalUserId, const TArray<FOnlineStatsUserUpdatedStats>& UpdatedUserStats, const FOnlineStatsUpdateStatsComplete & Delegate) {
	if (Subsystems.IsActive(SubsystemType::PS4) || Subsystems.IsActive(SubsystemType::Steam)) {
		return;
	}

	if (const auto* subsystem = GetDungeonsSubsystem()) {
		check(IsCompatibleNetId(subsystem, *LocalUserId));
		subsystem->GetStatsInterface()->UpdateStats(LocalUserId, UpdatedUserStats, Delegate);
	}
}

void Stats::QueryStats(const TSharedRef<const FUniqueNetId> LocalUserId, const TSharedRef<const FUniqueNetId> StatsUser, const FOnlineStatsQueryUserStatsComplete & Delegate) {
	if (Subsystems.IsActive(SubsystemType::PS4) || Subsystems.IsActive(SubsystemType::Steam)) {
		return;
	}

	if (const auto* subsystem = GetDungeonsSubsystem()) {
		check(IsCompatibleNetId(subsystem, *LocalUserId));
		subsystem->GetStatsInterface()->QueryStats(LocalUserId, StatsUser, Delegate);
	}
}

TSharedPtr<const FUniqueNetId> Stats::GetUniqueId(int LocalUserNum) const {
	if (const auto* subsystem = GetDungeonsSubsystem()) {
		return GetDungeonsSubsystem()->GetIdentityInterface()->GetUniquePlayerId(LocalUserNum);
	}
	return {};
}

}
}
