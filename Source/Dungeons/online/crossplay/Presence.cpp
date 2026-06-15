#include "Presence.h"
#include "SubsystemRepo.h"
#include <algorithm>

namespace online {
namespace Crossplay {
Presence::Presence(const SubsystemRepo& subsystems) : SubOSS(subsystems) { }

void Presence::ClearOnPresenceReceivedDelegate_Handle(FDelegateHandle& Handle) {
	ClearDelegateHandles("OnPresenceChangeDelegateHandle", [](TSharedRef<Child<FDelegateHandle>>& child) {
		child->Subsystem->GetPresenceInterface()->ClearOnPresenceReceivedDelegate_Handle(child->obj);
	});
}

FDelegateHandle Presence::AddOnPresenceReceivedDelegate_Handle(const FOnPresenceReceivedDelegate& Delegate) {
	FDelegateHandle handle(FDelegateHandle::GenerateNewHandle);
	TSharedRef<Link<FDelegateHandle>> link = MakeShared<Link<FDelegateHandle>>(&handle, 0);

	for (auto* subsystem : GetSubsystemsWithPresence()) {
		link->Children.Add(MakeShared<Child<FDelegateHandle>>(subsystem, subsystem->GetPresenceInterface()->AddOnPresenceReceivedDelegate_Handle(Delegate)));
	}

	if (std::any_of(link->Children.begin(), link->Children.end(), [](TSharedRef<Child<FDelegateHandle>> c) { return !c->obj.IsValid(); })) {
		handle.Reset();
	}

	AddDelegateHandle("OnPresenceChangeDelegateHandle", link);
	return handle;
}

void Presence::SetPresence(const FUniqueNetId & User, const FOnlineUserPresenceStatus & Status) {
	for (const auto subsystem : GetSubsystemsWithPresence()) {
		const auto compatibleUserId = GetUniqueNetIdFromName(User, subsystem->GetSubsystemName());
		subsystem->GetPresenceInterface()->SetPresence(*compatibleUserId, Status);
	}
}

TArray<IOnlineSubsystem*> Presence::GetSubsystemsWithPresence() const {
	return Subsystems.GetAllActive().FilterByPredicate([](const IOnlineSubsystem* subsystem) {
		return subsystem->GetPresenceInterface().IsValid();
	});
}

}
}
