#pragma once

#include "GameFramework/Actor.h"
#include "ConduitProp.generated.h"

UCLASS()
class DUNGEONS_API AConduitProp : public AActor {
	GENERATED_BODY()

public:
	AConduitProp(const FObjectInitializer& objectInitializer);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"), Category = "Default")
	bool StartEnabled = true;
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	bool IsEnabled = false;
};