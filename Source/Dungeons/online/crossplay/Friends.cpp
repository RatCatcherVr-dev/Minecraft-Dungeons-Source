#include "Friends.h"
#include "SubsystemRepo.h"
#include <algorithm>
#include "util/Algo.hpp"
#include "DungeonsGameInstance.h"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"

namespace online {
namespace Crossplay {
	Friends::Friends(const SubsystemRepo& subsystems) : SubOSS(subsystems) { }

	void Friends::ClearOnFriendsChangeDelegate_Handle(int num, FDelegateHandle& Handle) {
		ClearDelegateHandles("OnFriendsChangeDelegateHandle", [num](TSharedRef<Child<FDelegateHandle>>& child)
		{
			child->Subsystem->GetFriendsInterface()->ClearOnFriendsChangeDelegate_Handle(num, child->obj);
		});
	}

	FDelegateHandle Friends::AddOnFriendsChangeDelegate_Handle(int num, const FOnFriendsChangeDelegate& Delegate) {
		FDelegateHandle handle(FDelegateHandle::GenerateNewHandle);
		TSharedRef<Link<FDelegateHandle>> link = MakeShared<Link<FDelegateHandle>>(&handle, 0);
		if (auto* subsystem = GetDungeonsSubsystem()) {
			Child<FDelegateHandle> child = { subsystem, subsystem->GetFriendsInterface()->AddOnFriendsChangeDelegate_Handle(num, Delegate) };
			auto sharedChild = MakeShared<Child<FDelegateHandle>>(child);

			if (sharedChild->obj.IsValid()) {
				link->Children.Add(sharedChild);
			}
			else {
				handle.Reset();
			}
		}
		AddDelegateHandle("OnFriendsChangeDelegateHandle", link);
		return handle;
	}

	void ConfigurePS4Friends(const TArray<TSharedRef<FOnlineFriend>>& friends, bool IsCrossplayEnabled){
		for (const auto& Buddy : friends) {
			if (!Buddy->GetPresence().bIsOnline || (IsCrossplayEnabled && !Buddy->GetPresence().bIsPlayingThisGame)) {
				Buddy->SetUserLocalAttribute(TEXT("invitablestatus"), TEXT("BlockedInactive"));
			}
			else {
				Buddy->SetUserLocalAttribute(TEXT("invitablestatus"), IsCrossplayEnabled ? "BlockedCrossplayEnabled" : "Invitable");
			}
		}
	}

	void ConfigureDungeonsFriends(const TArray<TSharedRef<FOnlineFriend>>& friends, bool IsCrossplayEnabled)
	{
		for (const auto& Buddy : friends)
		{
			//Get platform of the friend
			FString Platform = Buddy->GetPresence().GetPlatform();
			sessionSettings::PlatformType PlType = Platform == "" ? sessionSettings::PlatformType::UNKNOWN : sessionSettings::ConvertToPlatform(Platform);

			//Make sure to check if we have a TwoWayFriendship
			FString IsTwoWayFriendString;
			bool IsTwoWayFriend = false;
			if (Buddy->GetUserAttribute(TEXT("IsTwoWayFriend"), IsTwoWayFriendString))
			{
				IsTwoWayFriend = IsTwoWayFriendString == FString("true") ? true : false;
			}

			FString InvitableStatus = "";
			if (!IsCrossplayEnabled && online::getCrossplayOss()->IsPS4Active()) {
				Buddy->SetUserLocalAttribute(TEXT("invitablestatus"), "BlockedCrossplayDisabled");
			}
			else if (IsCrossplayEnabled || sessionSettings::IsSamePlatform(PlType))
			{
				if (Buddy->GetPresence().bIsPlayingThisGame || sessionSettings::CanPlatformRespondToOutOfTitleInvites(PlType)) {
					if (IsTwoWayFriend) {
						InvitableStatus = TEXT("Invitable");
					} else {
						InvitableStatus = TEXT("BlockedOnewayFriend");
					}
				} else {
					InvitableStatus = TEXT("BlockedInactive");
				}
			} else {
				InvitableStatus = TEXT("BlockedDifferentPlatform");
			}
			Buddy->SetUserLocalAttribute(TEXT("invitablestatus"), InvitableStatus);
		}
	}

	bool Friends::ReadFriendsList(int32 LocalUserNum, const FString& ListName, const FOnReadFriendsListComplete& Delegate) {
		const auto subsystems = GetSubsystemsWithFriendsInterface();
		bool result = true;
		auto sharedLink = MakeShared<Link<FOnReadFriendsListComplete>>(&Delegate, subsystems.Num());
		for (const auto* subsystem : subsystems) {
			FOnReadFriendsListComplete callback;
			auto child = MakeShared<Child<FOnReadFriendsListComplete>>(subsystem, callback);

			child->obj.BindLambda([Delegate, sharedLink, child](int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorString) {
				FString resErrorString;
				child->Success = bWasSuccessful;
				child->ErrorMessage = ErrorString;
				if (--sharedLink->ChildrenNotReturned > 0) {
					return;
				}

				bool success = algo::all_of(sharedLink->Children, RETLAMBDA(it->Success));
				for (auto& c : sharedLink->Children) {
					resErrorString += c->ErrorMessage;
				}
				Delegate.ExecuteIfBound(LocalUserNum, success, ListName, resErrorString);
			});
			sharedLink->Children.Add(child);
		}
		for (auto& child : sharedLink->Children) {
			result &= child->Subsystem->GetFriendsInterface()->ReadFriendsList(LocalUserNum, ListName, child->obj);
		};
		return result;
	}

	bool Friends::GetFriendsList(int32 LocalUserNum, const FString& ListName, TArray< TSharedRef<FOnlineFriend> >& OutFriends) {
		const auto subsystems = GetSubsystemsWithFriendsInterface();
		return std::all_of(subsystems.begin(), subsystems.end(), [&LocalUserNum, &ListName, &OutFriends, DungeonsSubsystem = GetDungeonsSubsystem(), PS4Subsystem = GetPS4Subsystem()](const auto* subsystem) {
			TArray<TSharedRef<FOnlineFriend>> friends;
			bool result = subsystem->GetFriendsInterface()->GetFriendsList(LocalUserNum, ListName, friends);
			bool crossplayEnabled = online::IsCrossplayEnabled();

			if (DungeonsSubsystem == subsystem) {
				FNamedOnlineSession* namedSession = online::getCrossplayOss()->GetSessionIF()->GetNamedSession(DungeonsGameSessionName);
				if (namedSession && namedSession->SessionInfo.IsValid()) {		
					crossplayEnabled = namedSession->SessionInfo->GetSessionId().GetType() == FName(TEXT("Dungeons"));
				}
				ConfigureDungeonsFriends(friends, crossplayEnabled);
			}
			else if (PS4Subsystem == subsystem) {
				ConfigurePS4Friends(friends, crossplayEnabled);
			}

			OutFriends.Append(friends);
			return result;
		});
	}

	bool Friends::SendInvite(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName, const FOnSendInviteComplete& Delegate) {
		if (const auto* subsystem = GetDungeonsSubsystem()) {
			ensure(!FriendId.IsValid() || IsCompatibleNetId(subsystem, FriendId));
			return subsystem->GetFriendsInterface()->SendInvite(LocalUserNum, FriendId, ListName, Delegate);
		}
		return false;
	}

	bool Friends::DeleteFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) {
		if (const auto* subsystem = GetDungeonsSubsystem()) {
			ensure(!FriendId.IsValid() || IsCompatibleNetId(subsystem, FriendId));
			return subsystem->GetFriendsInterface()->DeleteFriend(LocalUserNum, FriendId, ListName);
		}
		return false;
	}

	TSharedPtr<FOnlineFriend> Friends::GetFriend(int32 LocalUserNum, const FUniqueNetId& FriendId, const FString& ListName) {
		if (const auto* subsystem = GetSubsystemFromUniqueNetId(FriendId)) {
			if (const auto myFriend = subsystem->GetFriendsInterface()->GetFriend(LocalUserNum, FriendId, ListName)) {
				return myFriend;
			}
		}
		return nullptr;
	}

	TArray<IOnlineSubsystem*> Friends::GetSubsystemsWithFriendsInterface() const {
		return Subsystems.GetAllActive().FilterByPredicate([](const IOnlineSubsystem* subsystem) {
			return subsystem->GetFriendsInterface().IsValid();
		});
	}

}
}
