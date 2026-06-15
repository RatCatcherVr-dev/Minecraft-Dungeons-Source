#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonsReplicationGraphDebug.generated.h"

UCLASS()
class ADungeonsReplicationGraphDebug : public AActor
{
	GENERATED_BODY()

public:
	ADungeonsReplicationGraphDebug();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	void TestDraw(FVector const& pos, FVector const& extent);
};