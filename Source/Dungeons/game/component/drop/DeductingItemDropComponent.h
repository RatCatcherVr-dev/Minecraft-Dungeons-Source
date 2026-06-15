#pragma once

#include "PredefinedItemDropComponent.h"
#include "DeductingItemDropComponent.generated.h"

/**
 * Generates drops based on its predefined settings.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUNGEONS_API UDeductingItemDropComponent : public UPredefinedItemDropComponent {
	GENERATED_BODY()

public:
	UDeductingItemDropComponent();

	UFUNCTION(Server, Reliable, BlueprintCallable, WithValidation, Category = "Dungeons")
	void ServerDeductItems(const FVector& spawnLocation, AActor* triggeringPlayer, AActor* source);

protected:
	void OnMobDamaged(const FOnAttributeChangeData& data) override;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1"))
	int DeductAmount;

private:
	void DecreaseRemainingDropAmount(int count);
};
