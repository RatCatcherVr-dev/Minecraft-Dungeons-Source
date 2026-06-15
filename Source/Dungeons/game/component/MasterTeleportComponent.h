#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MasterTeleportComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UMasterTeleportComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMasterTeleportComponent();	

	virtual void TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;
};
