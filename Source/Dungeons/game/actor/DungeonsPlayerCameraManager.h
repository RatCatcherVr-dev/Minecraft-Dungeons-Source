// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "DungeonsPlayerCameraManager.generated.h"

UENUM()
enum class CameraShakeSamplerOffsetType : uint8 {
	Relative,
	Absolute,
	InverseAbsolute
};

/**
 * 
 */
UCLASS()
class DUNGEONS_API ADungeonsPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	/** Static.  Plays an in-world camera shake that affects all nearby players, with radial distance-based attenuation. Will use distance from possessed pawn instead of camera.
	* @param WorldContextObject - World context object.
	* @param Shake - Camera shake asset to use.
	* @param Epicenter - Location to place the effect in world space
	* @param Scale - Additional scaling that will be used (useful for scaling shakes based on attack power).
	* @param InnerRadius - Cameras inside this radius get the full intensity shake.
	* @param OuterRadius - Cameras outside this radius are not affected.
	* @param Falloff - Exponent that describes the shake intensity falloff curve between InnerRadius and OuterRadius. 1.0 is linear.
	* @param bOrientShakeTowardsEpicenter - Changes the rotation of shake to point towards epicenter instead of forward. Useful for things like directional hits.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static void PlayWorldCameraShakeAtPawn(const UObject* WorldContextObject, TSubclassOf<UCameraShake> Shake, FVector Epicenter, float Scale, float InnerRadius, float OuterRadius, float Falloff, bool bOrientShakeTowardsEpicenter = false);
	
	
	static float CalcRadialShakeScaleAtLocation(const FVector& Location, FVector Epicenter, float InnerRadius, float OuterRadius, float Falloff);


	/**
	* How to treat the offset value. Relative is clamped between 0 and 1 where 1 is at the camera and 0 is at the player.
	* Absolute simply adds the specified value in the direction of the camera.
	* InverseAbsolute uses the camera as origin and ads an absolute offset from there.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	CameraShakeSamplerOffsetType OffsetType = CameraShakeSamplerOffsetType::Relative;

	/**
	* Offset of sample point for camera shake. If posititive values are towards camera. 0 is at player.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Offset = 0.f;

public:
	UCameraShake* PlayCameraShake(TSubclassOf<UCameraShake> ShakeClass, float Scale = 1.f, enum ECameraAnimPlaySpace::Type PlaySpace = ECameraAnimPlaySpace::CameraLocal, FRotator UserPlaySpaceRot = FRotator::ZeroRotator) override;
};
