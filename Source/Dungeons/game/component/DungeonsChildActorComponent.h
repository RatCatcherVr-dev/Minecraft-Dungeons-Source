#pragma once

#include "Components/ChildActorComponent.h"
#include "DungeonsChildActorComponent.generated.h"

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UDungeonsChildActorComponent : public UChildActorComponent {
	GENERATED_BODY()

public:
	void OnRegister() override;
	void OnUnregister() override;
};
