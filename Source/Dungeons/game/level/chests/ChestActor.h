#pragma once

#include "CoreMinimal.h"
#include "game/level/chests/ChestType.h"
#include "ChestActor.generated.h"

UCLASS()
class DUNGEONS_API AChestActor : public AActor {
	GENERATED_BODY()

public:
	void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(EditDefaultsOnly)
	EChestType mChestType;

	UPROPERTY(EditAnywhere, Replicated)
	bool bOpened;

	UPROPERTY(EditAnywhere, Replicated)
	bool bDiscovered;


public:

	bool getIsOpened() const;

	bool getIsDiscovered() const;

	EChestType getChestType() const;
};
