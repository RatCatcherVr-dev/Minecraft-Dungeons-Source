#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterZAdjustmentComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UCharacterZAdjustmentComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UCharacterZAdjustmentComponent();

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ResetPosition();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interpolation")
	float halfTimeSeconds = 0.04f;

protected:
	void BeginPlay() override;

private:
	float GetMeshRelativeZ() const;
	void SetMeshRelativeZ(float Z);
	UCapsuleComponent* GetCapsule() const;
	USkeletalMeshComponent* GetMesh() const;

	mutable UCapsuleComponent* capsule = nullptr;
	mutable USkeletalMeshComponent* mesh = nullptr;

	float previousZ = 0;
	FVector previousPosition;
};
