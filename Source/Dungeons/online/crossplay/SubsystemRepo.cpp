#include "Dungeons.h"
#include "SubsystemRepo.h"
#include "util/EnumUtil.h"

namespace online {
namespace Crossplay {

namespace internal {
constexpr auto GenericOrder = { SubsystemType::Dungeons, SubsystemType::PS4, SubsystemType::Steam, SubsystemType::Null };
}
	
void SubsystemRepo::Register(SubsystemType type, IOnlineSubsystem& subsystem) {
	check(!IsRegistered(type) && "Trying to register already registered subsystem");
	Subsystems.Add(type, { &subsystem });
}

bool SubsystemRepo::IsRegistered(SubsystemType type) const {
	return Subsystems.Contains(type);
}

bool SubsystemRepo::IsActive(SubsystemType type) const {
	if (!IsRegistered(type)) {
		return false;
	}
	return Subsystems.FindChecked(type).IsActive;
}

IOnlineSubsystem* SubsystemRepo::GetActive(SubsystemType type) const {
	return IsActive(type) ? Subsystems[type].Subsystem : nullptr;
}

IOnlineSubsystem* SubsystemRepo::Get(SubsystemType type) const {
	return Subsystems.Contains(type) ? Subsystems[type].Subsystem : nullptr;
}

int32 SubsystemRepo::NumActive() const {
	return GetAllActive().Num();
}

IOnlineSubsystem* SubsystemRepo::GetFirstActiveSubsystem() const {
	return GetFirstActiveSubsystem(internal::GenericOrder);
}

IOnlineSubsystem* SubsystemRepo::GetFirstActiveSubsystem(const std::initializer_list<SubsystemType>& pickOrder) const {
	return GetActive(GetFirstActiveType(pickOrder));
}

SubsystemType SubsystemRepo::GetFirstActiveType(const std::initializer_list<SubsystemType>& pickOrder) const {
	for (auto type : pickOrder) {
		if (IsActive(type)) {
			return type;
		}
	}
	check(!"There is no active subsystem!");
	return SubsystemType::Null;
}

TArray<IOnlineSubsystem*> SubsystemRepo::GetAllActive() const {
	TArray<IOnlineSubsystem*> active;
	for (auto type : internal::GenericOrder) {
		if (IsActive(type)) {
			active.Add(GetActive(type));
		}
	}
	return active;
}

void SubsystemRepo::Activate(SubsystemType type) {
	check(!IsActive(type) && "Trying to activate an already activated sub system.");
	Subsystems.FindChecked(type).IsActive = true;
}

void SubsystemRepo::Deactivate(SubsystemType type) {
	check(IsActive(type) && "Trying to deactivate an already deactivated sub system.");
	Subsystems.FindChecked(type).IsActive = false;
}

SubsystemType SubsystemRepo::GetSubsystemType(FName type) const {
	return ::EnumValueFromString(SubsystemType, type.ToString()).Get(SubsystemType::Null);
}

FName SubsystemRepo::GetStringFromType(SubsystemType type) const {
	return FName(*GetEnumValueToStringStripped(type));
}

void SubsystemRepo::ForAllSubsystems(std::function<void(IOnlineSubsystem*)> callback) const
{
	for (auto& subsystem : Subsystems)
	{
		callback(subsystem.Value.Subsystem);
	}
}

}
}
