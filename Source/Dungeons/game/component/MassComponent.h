#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MassComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UMassComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMassComponent();

	UFUNCTION(BlueprintCallable)
	bool IsImmovable() const;

	float GetPushMultiplier() const;

	void SetAdditionalMultiplier(float multiplier);
private:

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float pushMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	bool immovable = false;

	float additionalPushMultiplier = 1.0f;
};
