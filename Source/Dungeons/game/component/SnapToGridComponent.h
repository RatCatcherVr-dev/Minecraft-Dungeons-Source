#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SnapToGridComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API USnapToGridComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USnapToGridComponent();

	void BeginPlay() override;
};
