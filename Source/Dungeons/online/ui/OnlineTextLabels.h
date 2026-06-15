#pragma once

#include "Internationalization.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ui/dialog/BlockingMessage.h"
#include "online/OnlineFunctionTypes.h"
#include "StringTableCoreFwd.h"
#include "OnlineTextLabels.generated.h"

UCLASS()
class DUNGEONS_API UOnlineTextLabels : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FText GetPrivilegeError(int32 PrivilegeError);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FText GetSignInError(ELoginResult result);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FText GetLinkAccountError(ELoginResult result);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FText GetAuthenticationErrorTitle(EBlockingMessageType blockingMessageType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FText GetAuthenticationErrorMessage(EBlockingMessageType blockingMessageType);

};

namespace online {
	namespace ui {
		inline FText lobbyName() { return NSLOCTEXT("MapName", "lobby_name", "Camp"); }
		inline FText year1ChallengeTemplate() { 
			return NSLOCTEXT("OneYearEventTracker", "1year_cape_challenge_in_progress", "{0} of {1} seasonal trials completed");
		}
		inline FText year1ChallengeComplete() { 
			return NSLOCTEXT("OneYearEventTracker", "1year_cape_challenge_completed", "Cape awarded!"); 
		}
		inline FText eventTrackerInProgress() { 
			return NSLOCTEXT("OneYearEventTracker", "event_tracker_in_progress", "{0} of {1} {2}");
		}
		inline FText eventTrackerComplete() { 
			return NSLOCTEXT("OneYearEventTracker", "event_tracker_complete", "{0} awarded!"); 
		}
		inline FText eventTrackerClaimable() { 
			return NSLOCTEXT("OneYearEventTracker", "event_tracker_complete", "{0} claimable!"); 
		}

		// TEMPORARY SOLUTION TO SHOW STATUS TO USER
		inline FText eventTrackerConnection() { 
			return NSLOCTEXT("OneYearEventTracker", "event_tracker_no_connection", "Check your internet connection - {0}"); 
		}
	}

}
