#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DistancelimitComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UDistancelimitComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDistancelimitComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
