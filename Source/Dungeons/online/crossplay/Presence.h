#pragma once
#include "SubOSS.h"

namespace online {
namespace Crossplay {

class Presence : public SubOSS {
public:
	Presence(const SubsystemRepo&);

	void ClearOnPresenceReceivedDelegate_Handle(FDelegateHandle & Handle);
	FDelegateHandle AddOnPresenceReceivedDelegate_Handle(const FOnPresenceReceivedDelegate& Delegate);

	void SetPresence(const FUniqueNetId& User, const FOnlineUserPresenceStatus& Status);
	
private:
	TArray<IOnlineSubsystem*> GetSubsystemsWithPresence() const;
};
}
}
