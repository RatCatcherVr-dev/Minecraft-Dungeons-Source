#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "DungeonsGameSession.generated.h"


UCLASS()
class DUNGEONS_API ADungeonsGameSession : public AGameSession
{
	GENERATED_BODY()

	FString ApproveLogin(const FString& Options) override;

public:
	bool KickPlayer(APlayerController* KickedPlayer, const FText& KickReason) override;
	bool IsKicked(const FString& guid);
	void RemoveKick(const FUniqueNetIdWrapper&);
	bool InvitePlayer(const FUniqueNetIdWrapper&);

	void RegisterPlayer(APlayerController* NewPlayer, const TSharedPtr<const FUniqueNetId>& UniqueId, bool bWasFromInvite) override;
	void UnregisterPlayer(FName InSessionName, const FUniqueNetIdRepl& UniqueId) override;

private:
	struct InviteInfo {
		FUniqueNetIdWrapper NetId;
		int64 InviteTime = -1;
		InviteInfo(const FUniqueNetIdWrapper&, int64 timestamp);
	};

	TArray<InviteInfo> Invites;
};
