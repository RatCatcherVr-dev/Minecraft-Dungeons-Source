#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RandomMaterialComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API URandomMaterialComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URandomMaterialComponent();

	void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<UMaterialInstance*> materials;
};
