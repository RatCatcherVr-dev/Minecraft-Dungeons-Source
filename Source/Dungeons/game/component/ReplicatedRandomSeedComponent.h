// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "util/Random.h"
#include "ReplicatedRandomSeedComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UReplicatedRandomSeedComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UReplicatedRandomSeedComponent();

	FRandomStream& Stream() { return internalRandomStream; };
	Random& Rand() { return internalRandom; };

	void SetSeed(int32);

private:
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_Seed();

	UPROPERTY(ReplicatedUsing = OnRep_Seed)
	int32 Seed = 0;

	FRandomStream internalRandomStream;
	Random internalRandom;
};
