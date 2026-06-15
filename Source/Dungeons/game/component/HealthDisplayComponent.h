#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.h"
#include "HealthDisplayComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UHealthDisplayComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthDisplayComponent();

	void OnAttributeHealthChange(const FOnAttributeChangeData& data);

private:
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	FName uniformName;
};
