#include "SubOSS.h"
#include "SubsystemRepo.h"
#include <functional>
#include "online/sessions/OnlineUtil.h"

namespace online {
namespace Crossplay {

bool IsCompatibleNetId(const IOnlineSubsystem* subsystem, const FUniqueNetId& netId) {
	if (subsystem->GetSubsystemName() != netId.GetType()) {
		UE_LOG(LogOnline, Error, TEXT("Incompatabile unique net id, the subsystem is of type '%s' but the net id is '%s'."), *subsystem->GetSubsystemName().ToString(), *netId.GetType().ToString());
		return false;
	}
	return true;
}

SubOSS::SubOSS(const SubsystemRepo& subsystems) : Subsystems(subsystems){ 
}

IOnlineSubsystem* SubOSS::GetDungeonsSubsystem() const {
	return Subsystems.GetActive(SubsystemType::Dungeons);
}

IOnlineSubsystem* SubOSS::GetPS4Subsystem() const {
	return Subsystems.GetActive(SubsystemType::PS4);
}

IOnlineSubsystem* SubOSS::GetFirstActiveSubsystem() const {
	return Subsystems.GetFirstActiveSubsystem();
}

IOnlineSubsystem* SubOSS::GetFirstActiveSubsystem(const std::initializer_list<SubsystemType>& pickOrder) const {
	return Subsystems.GetFirstActiveSubsystem(pickOrder);
}

IOnlineSubsystem* SubOSS::GetSubsystemFromUniqueNetId(const FUniqueNetId& Requestor) const {
	return Subsystems.GetActive(Subsystems.GetSubsystemType(Requestor.GetType()));
}

IOnlineSubsystem* SubOSS::GetSessionSubsystem() const {
	auto* subsystem = IsCrossplayEnabled() ? Subsystems.GetActive(SubsystemType::Dungeons) : Subsystems.GetActive(SubsystemType::PS4);
	return subsystem ? subsystem : Subsystems.GetActive(SubsystemType::Null);
}

IOnlineSubsystem* SubOSS::GetSubsystemFromName(const FName& Type) const
{
	return Subsystems.GetActive(Subsystems.GetSubsystemType(Type));
}

SubsystemType SubOSS::GetFirstActiveType(const std::initializer_list<SubsystemType>& pickOrder) const {
	return Subsystems.GetFirstActiveType(pickOrder);
}

void SubOSS::ClearDelegateHandles(const FString& key, std::function<void(TSharedRef<Child<FDelegateHandle>>&)> callback)
{
	if (!DelegateHandles.Contains(key))
		return;
	TSharedRef<Link<FDelegateHandle>> link = DelegateHandles.FindAndRemoveChecked(key);
	for (auto& child : link->Children)
	{
		callback(child);
	}
}

void SubOSS::AddDelegateHandle(const FString& key, const TSharedRef<Link<FDelegateHandle>>& link) {
	check(DelegateHandles.Find(key) == nullptr);
	DelegateHandles.Add(key, link);
}

TSharedPtr<const FUniqueNetId> SubOSS::GetUniqueNetIdFromName(const FUniqueNetId& Requestor, FName name) const {
	return GetUniqueNetIdFromType(Requestor, Subsystems.GetSubsystemType(name));
}

TSharedPtr<const FUniqueNetId> SubOSS::GetUniqueNetIdFromType(const FUniqueNetId& Requestor, SubsystemType type) const {
	if (Requestor.GetType() != Subsystems.GetStringFromType(type)) {
		auto requestorType = Subsystems.GetSubsystemType(Requestor.GetType());
		if (Subsystems.IsActive(requestorType) && Subsystems.IsActive(type)) {
			FPlatformUserId id = Subsystems.GetActive(requestorType)->GetIdentityInterface()->GetPlatformUserIdFromUniqueNetId(Requestor);
			return Subsystems.GetActive(type)->GetIdentityInterface()->GetUniquePlayerId(id);
		}
		check(!"Requestor subsystem not active!");
		return {};
	}
	return Requestor.AsShared();
}

TSharedPtr<const FUniqueNetId> SubOSS::GetUniqueNetIdFromType(const FUniqueNetId& Requestor, const std::initializer_list<SubsystemType>& pickOrder) const {
	return GetUniqueNetIdFromType(Requestor, GetFirstActiveType(pickOrder));
}

	
}
}
