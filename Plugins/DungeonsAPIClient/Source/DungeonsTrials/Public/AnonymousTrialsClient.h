#pragma once

#include "CoreMinimal.h"
#include "TrialsClient.h"

class DUNGEONSTRIALS_API AnonymousTrialsClient final : public TrialsClient {

protected:
	shared_ptr<minecraft::api::MinecraftClient> GetClient() override;
	void Request() override;
};
