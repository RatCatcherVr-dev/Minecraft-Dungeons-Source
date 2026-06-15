#pragma once

#include <Kismet/BlueprintFunctionLibrary.h>
#include "DungeonsGameInstance.h"
#include "online/friends/DungeonsFriendsCommon.h"
#include "DungeonsFriendsUtil.generated.h"

UCLASS(BlueprintType)
class UPlayerInfoUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsPlayerHost(const FBlueprintGameSession& Session, const FBlueprintFriend& Friend) {
		return Session.HostUniqueNetId == Friend.UniqueNetId;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FString GetHostDisplayName(const FBlueprintGameSession& Session) {
		return Session.HostDisplayName;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FString GetInviteSenderName(const FBlueprintGameSession& Session) {
		return Session.InviteSenderName;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool JoinSession(UObject* WorldContext, const FBlueprintGameSession& Session) {
		if (auto instance = Cast<UDungeonsGameInstance>(WorldContext->GetWorld()->GetGameInstance())) {
			instance->AcceptInvite(Session.SearchResult);
			return true;
		}

		return false;
	}
};
