// © 2020 Mojang Synergies AB. TM Microsoft Corporation.
#pragma once

#include "game/item/instance/AItemInstance.h"
#include "MysteryTokenInstance.generated.h"

UCLASS(Blueprintable)
class DUNGEONS_API AMysteryTokenInstance final : public AItemInstance
{
	GENERATED_BODY()

public:

	void Activate(const FPredictionKey& predictionKey) override final;
};