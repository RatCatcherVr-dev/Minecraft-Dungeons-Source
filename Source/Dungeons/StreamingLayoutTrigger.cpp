// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#include "StreamingLayoutTrigger.h"

#include "TimerManager.h"
#include "Components/SphereComponent.h"

AStreamingLayoutTrigger::AStreamingLayoutTrigger()
{
	X_SphereCollision = CreateDefaultSubobject<USphereComponent>(FName("LayoutCollision"));
}

void AStreamingLayoutTrigger::BeginPlay()
{
#if PLATFORM_XBOXONE
	if (!XCloud::Get()->IsStreaming())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(LayoutTimerHandle, this, &AStreamingLayoutTrigger::CheckOverlappingActors, 1.f, false, 0.2f);

	X_SphereCollision->OnComponentBeginOverlap.AddDynamic(this,
		&AStreamingLayoutTrigger::OnOverlapBegin);
#endif
	Super::BeginPlay();
}

void AStreamingLayoutTrigger::CheckOverlappingActors()
{
	TArray<AActor*> OverlappingActors;
	X_SphereCollision->GetOverlappingActors(OverlappingActors, TSubclassOf<AActor>());

	for (const auto& Actor : OverlappingActors)
	{
		if (Actor != this)
		{
			if (APawn* Player = Cast<APawn>(Actor))
			{
				if (Player->IsLocallyControlled())
				{
					HideTouchControl();
					ShowTouchControl(LayoutToShow);
				}
			}
		}
	}
}

void AStreamingLayoutTrigger::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
#if PLATFORM_XBOXONE
	if (OtherActor && OtherActor != this &&
		Cast<APawn>(OtherActor) && Cast<APawn>(OtherActor)->IsLocallyControlled())
	{
		HideTouchControl();
		ShowTouchControl(LayoutToShow);
	}
#endif
}


