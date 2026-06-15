#pragma once

#include "TrialsProvider.h"
#include "AnonymousTrialsProvider.generated.h"

UCLASS()
class UAnonymousTrialsProvider final : public UTrialsProvider {
	GENERATED_BODY()

protected:
	std::shared_ptr<TrialsClient> GetTrialsClient() const override;
};