#pragma once

#include "CoreMinimal.h"
#include "TrialsClient.h"

class DUNGEONSTRIALS_API TrialsClientManager {

public:
	std::shared_ptr<TrialsClient> Authenticated();
	std::shared_ptr<TrialsClient> Anonymous();

private:
	std::shared_ptr<TrialsClient> Client = nullptr;
	std::shared_ptr<TrialsClient> AnonymousClient = nullptr;
};
