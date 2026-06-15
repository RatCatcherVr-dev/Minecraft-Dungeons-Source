#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InitInvincibleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UInitInvincibleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInitInvincibleComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float delay = 1.0f;

	bool firstTick = true;
};
