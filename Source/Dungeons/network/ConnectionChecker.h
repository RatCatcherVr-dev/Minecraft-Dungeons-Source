#pragma once

#if PLATFORM_SWITCH
#include <nn/oe.h>
#include "CoreDelegates.h"
#endif
#include "ObjectMacros.h"
#include "OnlineSubsystemTypes.h"

#include "ConnectionChecker.generated.h"

UCLASS()
class UConnectionChecker : public UObject
{
	GENERATED_BODY()

public:
	UConnectionChecker();
	~UConnectionChecker();

	void Init();
	EOnlineServerConnectionStatus::Type CheckConnection();
};

