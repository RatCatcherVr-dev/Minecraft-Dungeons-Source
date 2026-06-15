#pragma once
#include "online/OnlineCommon.h"
#include "CoreMinimal.h"
#include "SubsystemRepo.h"

namespace online {
namespace Crossplay {


class Identity;
class Session;
class ExternalUI;
class Friends;
class Achievements;
class Presence;
class Stats;
class UserCloud;
class SubOSS;

class OSS {
public:
	OSS();
	~OSS();

	// Dungeons util
	bool IsDungeonsActive() const;
	bool IsPS4Active() const;
	bool IsSteamActive() const;
	bool IsNullActive() const;

	void ActivateDungeonsOSS();
	void DeactivateDungeonsOSS();

	void SetXblActive(bool value);

	//IOnlineSubsystem
	void SetUsingMultiplayerFeatures(const FUniqueNetId& UniqueId, bool bUsingMP);
	FText GetOnlineServiceName();
	FName GetInstanceName();

	TSharedPtr<Identity> GetIdentityIF() const;
	TSharedPtr<Session> GetSessionIF() const;
	TSharedPtr<ExternalUI> GetExternalUIIF() const;
	TSharedPtr<Friends> GetFriendsIF() const;
	TSharedPtr<Achievements> GetAchievementsIF() const;
	TSharedPtr<Presence> GetPresenceIF() const;
	TSharedPtr<Stats> GetStatsIF() const;
	TSharedPtr<UserCloud> GetUserCloudIF() const;
	
	bool SetCrossplaySetting(bool);
	void SetDefaultSubsystem(SubsystemType type);
	void SetNetDriver(SubsystemType);
	void AddOnConnectionStatusChangedDelegate_Handle(FOnConnectionStatusChangedDelegate);

private:
	template<class T>
	TSharedPtr<T> CreateInterface(SubsystemRepo& repo);

	SubsystemRepo Subsystems;
	TSharedPtr<Identity> IdentityIF;
	TSharedPtr<Session> SessionIF;
	TSharedPtr<ExternalUI> ExternalUIIF;
	TSharedPtr<Friends> FriendsIF;
	TSharedPtr<Achievements> AchievementsIF;
	TSharedPtr<Presence> PresenceIF;
	TSharedPtr<Stats> StatsIF;
	TSharedPtr<UserCloud> UserCloudIF;
};

}
}