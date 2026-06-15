#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_CustomParticle.generated.h"

UCLASS()
class DUNGEONS_API UAnimNotifyState_CustomParticle : public UAnimNotifyState {
	GENERATED_UCLASS_BODY()

public:

	UAnimNotifyState_CustomParticle();

	// Particle System to Spawn
	UPROPERTY(EditAnywhere, Category = "AnimNotify", meta = (DisplayName = "Particle System"))
	UParticleSystem* PSTemplate = nullptr;

	// Location offset from the socket
	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FVector LocationOffset;

	// Rotation offset from socket
	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FRotator RotationOffset;

	// Location offset from the socket
	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FVector Scale = FVector(1, 1, 1);

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	FLinearColor Color = FColor::White;

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	float Lifetime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "AnimNotify")
	float RotationSpeed = 0.5f;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void PostLoad() override;
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;
	virtual FString GetNotifyName_Implementation() const override;

private:
	// Cached version of the Rotation Offset already in Quat form
	FQuat RotationOffsetQuat;

	static const FName COLOR_PARAMETER;
	static const FName LIFETIME_PARAMETER;
	static const FName ROTATION_SPEED_PARAMETER;

};
