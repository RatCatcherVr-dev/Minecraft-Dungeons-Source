#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RelocateComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API URelocateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URelocateComponent();

	void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

	FVector RelocateLocation;

private:	
	int CheckCount = 0;	
};
