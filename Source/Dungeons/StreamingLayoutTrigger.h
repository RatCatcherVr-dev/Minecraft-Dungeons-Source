// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "XCloudAPI.h"

#include "StreamingLayoutTrigger.generated.h"

class USphereComponent;

UCLASS(Blueprintable)
class DUNGEONS_API AStreamingLayoutTrigger : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "XCloud")
	ETakLayout LayoutToShow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "XCloud")
	USphereComponent* X_SphereCollision;


	FTimerHandle LayoutTimerHandle;

	AStreamingLayoutTrigger();

	void BeginPlay() override;

	void CheckOverlappingActors();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "XCloud")
	void ShowTouchControl(ETakLayout Layout) noexcept
	{
#if PLATFORM_XBOXONE
		XCloud::Get()->ShowTouchControl(Layout);
#endif
	}



	UFUNCTION(BlueprintCallable, Category = "XCloud")
	void HideTouchControl() noexcept
	{
#if PLATFORM_XBOXONE
		XCloud::Get()->HideTouchControl();
#endif
	}
};
