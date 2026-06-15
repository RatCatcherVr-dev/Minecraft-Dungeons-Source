#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/actor/DynamicBeam.h"
#include "GrowComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UGrowComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGrowComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void Inflate();
	void Deflate();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FORCEINLINE void SetMaxScale(float scale) { scaleMax = scale; }
	FORCEINLINE float GetMaxScale() const { return scaleMax; }

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnRep_Sign();
	
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float increment = .9f;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Dungeons")
	float scaleMax = 1.2f;

	UPROPERTY(ReplicatedUsing=OnRep_Sign)
	float sign = 1.f;
};
