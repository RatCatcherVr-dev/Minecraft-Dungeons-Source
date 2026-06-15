#pragma once

#include "CoreMinimal.h"
#include "ItemDropEffectComponent.generated.h"

/**
 * Can be added to SpawnableItem to provide an effect when the item is dropped.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUNGEONS_API UItemDropEffectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UItemDropEffectComponent();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Apply(AActor* DropSource, bool doCustomImpulse = false) const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	bool bEnableEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	bool bAllowDropSourceEffectMultipliers;

	/** Radius of the cone a random item drop velocity vector is generated within */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float DropConeRadius;

	/** Min initial velocity of an item drop (units/s) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float DropVelocityMin;

	/** Max initial velocity of an item drop (units/s) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float DropVelocityMax;

	/** Fix vector to bump dropped items even further */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	FVector BumpAmplificationVelocity;

	void ImpulseCustom(AActor* Item) const;

private:
	//D11.KS
	void Apply(AActor* Item, AActor* DropSource) const;
	bool ShouldRotate(AActor* Item) const;
	FVector GetVelocity(AActor* DropSource) const;
	float GetMultiplier(AActor* DropSource, const char* FieldName) const;
	FVector GetBaseVelocity(AActor* DropSource) const;

	static float GetConeAngle(const FVector& Velocity);
};
