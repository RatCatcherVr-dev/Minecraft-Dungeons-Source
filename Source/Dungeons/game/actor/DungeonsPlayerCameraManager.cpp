#include "Dungeons.h"
#include "DungeonsPlayerCameraManager.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "GameSettingsFunctionLibrary.h"
#include "IConsoleManager.h"
#include "character/player/PlayerControllerBase.h"

TAutoConsoleVariable<int32> CVarDebugDrawCameraShake(
	TEXT("Dungeons.DebugDraw.CameraShake"),
	0,
	TEXT("Enables debug drawing of camera shake.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);

namespace {
	FColor DebugDrawCameraShakeInnerRadiusColor(155, 255, 30);
}


float ADungeonsPlayerCameraManager::CalcRadialShakeScaleAtLocation(const FVector& POVLoc, FVector Epicenter, float InnerRadius, float OuterRadius, float Falloff)
{
	if (InnerRadius < OuterRadius)
	{
		float DistPct = ((Epicenter - POVLoc).Size() - InnerRadius) / (OuterRadius - InnerRadius);
		DistPct = 1.f - FMath::Clamp(DistPct, 0.f, 1.f);
		return FMath::Pow(DistPct, Falloff);
	}
	else
	{
		// ignore OuterRadius and do a cliff falloff at InnerRadius
		return ((Epicenter - POVLoc).SizeSquared() < FMath::Square(InnerRadius)) ? 1.f : 0.f;
	}
}


UCameraShake* ADungeonsPlayerCameraManager::PlayCameraShake(TSubclassOf<UCameraShake> ShakeClass, float Scale, ECameraAnimPlaySpace::Type PlaySpace, FRotator UserPlaySpaceRot)
{
	const auto controller = Cast<const APlayerControllerBase>(GetOwningPlayerController());
	if (UGameSettingsFunctionLibrary::IsScreenShakeEnabled(controller))
	{
		return Super::PlayCameraShake(ShakeClass, Scale, PlaySpace, UserPlaySpaceRot);
	}

	return nullptr;
}

void ADungeonsPlayerCameraManager::PlayWorldCameraShakeAtPawn(const UObject* WorldContextObject, TSubclassOf<class UCameraShake> Shake, FVector Epicenter, float Scale, float InnerRadius, float OuterRadius, float Falloff, bool bOrientShakeTowardsEpicenter)
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);

	FColor rndColor(125 + FMath::Rand() % 125, 125 + FMath::Rand() % 125, 125);

	if (CVarDebugDrawCameraShake.GetValueOnGameThread()) {
		float duration = Cast<UCameraShake>(Shake->GetDefaultObject())->OscillationDuration;
		DrawDebugSphere(World, Epicenter, InnerRadius, 32, DebugDrawCameraShakeInnerRadiusColor, false, duration);
		DrawDebugSphere(World, Epicenter, OuterRadius, 32, rndColor, false, duration);
	}

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && PlayerController->PlayerCameraManager != NULL)
		{
			FVector location(PlayerController->GetPawn() ? PlayerController->GetPawn()->GetActorLocation() : PlayerController->PlayerCameraManager->GetCameraLocation());

			if (auto dungeonsCameramanager = Cast<ADungeonsPlayerCameraManager>(PlayerController->PlayerCameraManager)) {
				FVector cameraLoc = PlayerController->PlayerCameraManager->GetCameraLocation();
				//Not code-golfing for a reason here...Don't be tempted to abuse fall-through at the expense of readability.
				switch (dungeonsCameramanager->OffsetType)
				{
				case CameraShakeSamplerOffsetType::Absolute:
				{
					FVector direction = (cameraLoc - location);
					direction.Normalize();
					location += direction * dungeonsCameramanager->Offset;
					break;
				}
				case CameraShakeSamplerOffsetType::Relative:
				{
					FVector direction = (cameraLoc - location);
					location += direction * dungeonsCameramanager->Offset;
					break;
				}
				case CameraShakeSamplerOffsetType::InverseAbsolute:
				{
					FVector direction = location - cameraLoc;
					direction.Normalize();
					location = cameraLoc + direction * dungeonsCameramanager->Offset;
				}
				}
			}

			float ShakeScale = Scale * CalcRadialShakeScaleAtLocation(location, Epicenter, InnerRadius, OuterRadius, Falloff);

			if (ShakeScale <= 0) {
				return;
			}

			if (CVarDebugDrawCameraShake.GetValueOnGameThread()) {
				float duration = Cast<UCameraShake>(Shake->GetDefaultObject())->OscillationDuration;
				DrawDebugPoint(World, location, 10, rndColor, false, duration);
				DrawDebugString(World, location + FVector(0, 30, 0), FString::Printf(TEXT("Scale: %f"), ShakeScale), nullptr, rndColor, duration, true);
			}

			if (bOrientShakeTowardsEpicenter && PlayerController->GetPawn() != NULL)
			{
				FVector CamLoc;
				FRotator CamRot;
				PlayerController->PlayerCameraManager->GetCameraViewPoint(CamLoc, CamRot);
				PlayerController->ClientPlayCameraShake(Shake, ShakeScale, ECameraAnimPlaySpace::UserDefined, (Epicenter - CamLoc).Rotation());
			}
			else
			{
				PlayerController->ClientPlayCameraShake(Shake, ShakeScale);
			}
		}
	}
}
