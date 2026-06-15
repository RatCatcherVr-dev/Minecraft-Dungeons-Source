// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "repgraph/Actor_RepSpatializeStatic.h"
#include "WindPipeBase.generated.h"

/**
 *
 */
UCLASS(Abstract)
class DUNGEONS_API AWindPipeBase : public APropActor_RepAlways
{
	GENERATED_BODY()
public:
	AWindPipeBase();

	UFUNCTION(BlueprintCallable)
	float GetAngle() const { return InternalAngle; };

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Rotate();

	UFUNCTION(BlueprintCallable)
	void SetRotatationDirecition(bool clockwise);

	float GetDeltaAngle(float NewAngle) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* DefaultSceneRootInternal;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UReplicatedInteractableComponent* Interactable;

protected:
	UFUNCTION(BlueprintCallable)
	void SetAngleSpan(float minAngle, float maxAngle);

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedAngle)
	float ReplicatedAngle = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float AngleDelta = 90.f;

	UPROPERTY(BlueprintReadWrite)
	bool DisableRotation = false;

	UFUNCTION(BlueprintImplementableEvent)
	void OnAngleChanged(float NewAngle, float DeltaAngle);

private:
	bool RotateClockwise = true;

	bool bUseMinMax = false;
	//Defines the max angle. When rotated to either end of the span, the rotation direction will change.
	float MaxAngle = 360.f;
	//Defines the min angle. When rotated to either end of the span, the rotation direction will change.
	float MinAngle = 0.f;

	FORCEINLINE float sign() const { return RotateClockwise ? 1.f : -1.f; }

	float InternalAngle = 0.f;

	UFUNCTION()
	void OnRep_ReplicatedAngle();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
};
