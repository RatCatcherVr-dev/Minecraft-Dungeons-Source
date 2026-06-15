#pragma once

#include "Dungeons.h"
#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "game/item/instance/AItemInstance.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "../ItemType.h"

#include "TomeOfDuplicationInstance.generated.h"

class AMobCharacter;

UCLASS()
class DUNGEONS_API ATomeOfDuplicationInstance : public AItemInstance {
	GENERATED_BODY()
public:
	ATomeOfDuplicationInstance();

	void Activate(const FPredictionKey& predictionKey) override;
	void OnSetupWithValidOwner() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StorableLifeSpan = 10.f;

	bool CanActivate() const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDropItem(FSerializableItemId itemToSpawn);

	const FSerializableItemId& GetAlternativeItemId() const override;

private:
	void OnItemCollected(const FItemId&);
	bool CanItemIdBeConsidered(const FItemId&);

	TOptional<FItemId> LastItemCollected;
	FSerializableItemId LastItemSerializableItem;
};