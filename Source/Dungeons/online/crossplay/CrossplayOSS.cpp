#include "CrossplayOSS.h"
#include "Identity.h"
#include "Session.h"
#include "ExternalUI.h"
#include "Friends.h"
#include "Achievements.h"
#include "Presence.h"
#include "Stats.h"
#include "UserCloud.h"
#include "SocketSubsystemModule.h"
#include "online/sessions/OnlineUtil.h"
#include "Dungeons/game/actor/character/player/BasePlayerController.h"

namespace online {
namespace Crossplay {
namespace internal {
void activate(bool activate, IOnlineSubsystem* subsystem, std::initializer_list<TSharedPtr<SubOSS>> interfaces) {
	for (const auto& interfacePtr : interfaces) {
		if (activate) {
			interfacePtr->OnActivate(subsystem);
		} else {
			interfacePtr->OnDeactivate(subsystem);
		}
	}
}
}

template<class T>
TSharedPtr<T> OSS::CreateInterface ( SubsystemRepo& repo)
{
	return MakeShared<T>(repo);
}

OSS::OSS() {
	check(IsInGameThread());
 	if (auto* dungeonsSubsystem = IOnlineSubsystem::Get(TEXT("DUNGEONS"))) {
		Subsystems.Register(SubsystemType::Dungeons, *dungeonsSubsystem);
		Subsystems.Activate(SubsystemType::Dungeons);
	}
	if (PLATFORM_PS4) {
		Subsystems.Register(SubsystemType::PS4, *IOnlineSubsystem::Get(PS4_SUBSYSTEM));
		Subsystems.Activate(SubsystemType::PS4);
	}

	if (auto* steamSubsystem = IOnlineSubsystem::Get(TEXT("STEAM"))) {
		Subsystems.Register(SubsystemType::Steam, *steamSubsystem);
		Subsystems.Activate(SubsystemType::Steam);
	}

	if (Subsystems.NumActive() == 0) {
		Subsystems.Register(SubsystemType::Null, *IOnlineSubsystem::Get(TEXT("Null")));
		Subsystems.Activate(SubsystemType::Null);
	}

	IdentityIF = CreateInterface<Identity>(Subsystems);
	SessionIF = CreateInterface<Session>(Subsystems);
	ExternalUIIF = CreateInterface<ExternalUI>(Subsystems);
	FriendsIF = CreateInterface<Friends>(Subsystems);
	AchievementsIF = CreateInterface<Achievements>(Subsystems);
	PresenceIF = CreateInterface<Presence>(Subsystems);
	StatsIF = CreateInterface<Stats>(Subsystems);
	UserCloudIF = CreateInterface<UserCloud>(Subsystems);
}


OSS::~OSS()
{
}

bool OSS::IsDungeonsActive() const {
	check(IsInGameThread());
	return Subsystems.IsActive(SubsystemType::Dungeons);
}

bool OSS::IsPS4Active() const {
	return Subsystems.IsActive(SubsystemType::PS4);
}

bool OSS::IsSteamActive() const {
	return Subsystems.IsActive(SubsystemType::Steam);
}


bool OSS::IsNullActive() const {
	return Subsystems.IsActive(SubsystemType::Null);
}

void OSS::ActivateDungeonsOSS() {
	Subsystems.Activate(SubsystemType::Dungeons);
}

void OSS::DeactivateDungeonsOSS() {
	Subsystems.Deactivate(SubsystemType::Dungeons);
}

void OSS::SetXblActive(bool value) {
	auto* dungeonsOSS = Subsystems.Get(SubsystemType::Dungeons);
	check(dungeonsOSS);
	if (dungeonsOSS)
	{
		if (auto* world = GetWorldForOnline(dungeonsOSS->GetInstanceName())) {
			auto pc = Cast<APlayerControllerBase>(world->GetFirstPlayerController());
			online::SetXblActive(value, pc);
		}
	}
}

bool OSS::SetCrossplaySetting(bool crossplay) {
	if (Subsystems.IsActive(SubsystemType::PS4)) {
		internal::activate(crossplay, Subsystems.GetActive(SubsystemType::Dungeons),
			{ IdentityIF, SessionIF, ExternalUIIF, FriendsIF, AchievementsIF, PresenceIF, StatsIF });
		return true;
	}
	return false;
}

namespace internal {
	void UpdateNetDriverDefinitions(FName Driver, FName Platform) {
#if PLATFORM_PS4
		FSocketSubsystemModule& SSS = FModuleManager::GetModuleChecked<FSocketSubsystemModule>("Sockets");
		GEngine->NetDriverDefinitions[0].DriverClassName = Driver;// FName(TEXT("/Script/OnlineSubsystemPS4.PS4NetDriver"));
		bool res = SSS.SetDefaultSocketSubsystemChecked(Platform);
		check(res);
#endif
	}

	void UpdateDefaultSubsystem(FName SubsystemName) {
#if PLATFORM_PS4
		static const FName OnlineSubsystemModuleName = TEXT("OnlineSubsystem");
		FOnlineSubsystemModule& OSSModule = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>(OnlineSubsystemModuleName);
		OSSModule.SetDefaultSessionSubsystemName(SubsystemName);
#endif
	}
}

void OSS::SetDefaultSubsystem(SubsystemType type) {
	if (!IsPS4Active()) {
		//We should not update the netdriver on other platforms than PS4.
		return;
	}
	switch (type) {
	case SubsystemType::PS4: {
		internal::UpdateDefaultSubsystem(FName("PS4"));
		break;
	}
	case SubsystemType::Dungeons: {
		internal::UpdateDefaultSubsystem(FName("Dungeons"));
		break;
	}
	default: {
		check(!"Netdriver can only be set to Dungeons or PS4");
		break;
	}
	}
}

void OSS::SetNetDriver(SubsystemType type) {
	if (!IsPS4Active()) {
		//We should not update the netdriver on other platforms than PS4.
		return;
	}
	switch (type) {
	case SubsystemType::PS4: {
		internal::UpdateNetDriverDefinitions(FName(TEXT("/Script/OnlineSubsystemPS4.PS4NetDriver")), FName(TEXT("PS4")));
		break;
	}
	case SubsystemType::Dungeons: {
		internal::UpdateNetDriverDefinitions(FName(TEXT("/Script/OnlineSubsystemDungeons.DungeonsNetDriver")), FName(TEXT("Dungeons")));
		break;
	}
	default: {
		check(!"Netdriver can only be set to Dungeons or PS4");
		break;
	}
	}
}

void OSS::AddOnConnectionStatusChangedDelegate_Handle(FOnConnectionStatusChangedDelegate Delegate) {
	Subsystems.GetFirstActiveSubsystem({SubsystemType::PS4, SubsystemType::Dungeons, SubsystemType::Null})->AddOnConnectionStatusChangedDelegate_Handle(Delegate);
}

void OSS::SetUsingMultiplayerFeatures(const FUniqueNetId & UniqueId, bool bUsingMP) {
	check(IsInGameThread());
	for (auto* subsystem : Subsystems.GetAllActive()) {
		const auto ossType = Subsystems.GetSubsystemType(subsystem->GetSubsystemName());
		auto relevantID = IdentityIF->GetUniqueNetIdFromType(UniqueId, ossType);
		if (relevantID.IsValid()) {
			subsystem->SetUsingMultiplayerFeatures(*relevantID, bUsingMP);
		}
	}
}

FText OSS::GetOnlineServiceName() {
	//Getting the online service name of the subsystem set as default
	return IOnlineSubsystem::Get()->GetOnlineServiceName();
}

FName OSS::GetInstanceName()
{
	return IOnlineSubsystem::Get()->GetInstanceName();
}

TSharedPtr<Identity> OSS::GetIdentityIF() const {
	check(IsInGameThread());
	return IdentityIF;
}

TSharedPtr<Session> OSS::GetSessionIF() const {
	check(IsInGameThread());
	return SessionIF;
}

TSharedPtr<ExternalUI> OSS::GetExternalUIIF() const {
	check(IsInGameThread());
	return ExternalUIIF;
}

TSharedPtr<Friends> OSS::GetFriendsIF() const {
	check(IsInGameThread());
	return FriendsIF;
}

TSharedPtr<Achievements> OSS::GetAchievementsIF() const {
	check(IsInGameThread());
	return AchievementsIF;
}

TSharedPtr<Presence> OSS::GetPresenceIF() const {
	check(IsInGameThread());
	return PresenceIF;
}

TSharedPtr<Stats> OSS::GetStatsIF() const {
	check(IsInGameThread());
	return StatsIF;
}

TSharedPtr<UserCloud> OSS::GetUserCloudIF() const {
	check(IsInGameThread());
	return UserCloudIF;
}

}
}
