#include "Dungeons.h"
#include "GrowComponent.h"
#include "HealthComponent.h"

UGrowComponent::UGrowComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
}

void UGrowComponent::BeginPlay() {
	Super::BeginPlay();
}

void UGrowComponent::OnRep_Sign() {
	SetComponentTickEnabled(true);
}

void UGrowComponent::Inflate() {
	sign = 1.f;
	OnRep_Sign();
}

void UGrowComponent::Deflate() {
	sign = -1.f;
	OnRep_Sign();
}

void UGrowComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGrowComponent, scaleMax);
	DOREPLIFETIME(UGrowComponent, sign);
}

void UGrowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto owner = GetOwner();

	if (const auto healthComponent = owner->FindComponentByClass<UHealthComponent>()) {
		if (healthComponent->IsAlive()) {
			auto transform = owner->GetActorTransform();
			const auto scale = transform.GetScale3D();

			// assuming everything is scaled uniformly
			if ((sign < 0.f && scale.X > 1.f) || (sign > 0.f && scale.X < scaleMax)) {
				const auto newScale = scale.X + DeltaTime * increment * sign;
				transform.SetScale3D(FVector { FMath::Clamp(newScale, 1.f, scaleMax) });
				owner->SetActorTransform(transform);

				if (const auto sphereComponent = owner->FindComponentByClass<USphereComponent>()) {
					sphereComponent->SetRelativeScale3D(FVector(1.f / newScale));
				}
			} else {
				if (sign > 0.f) {
					SetComponentTickEnabled(false);
				}
			}
		}
	}
}