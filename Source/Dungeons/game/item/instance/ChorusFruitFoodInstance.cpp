#include "ChorusFruitFoodInstance.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/util/LocationQuery.h"
#include "../../component/TeleportComponent.h"
#include "Kismet/KismetMathLibrary.h"

void AChorusFruitFoodInstance::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);

	if (HasAuthority()) {
		if (TeleportChance <= UKismetMathLibrary::RandomFloat()) {
			if (const auto owner = Cast<ABaseCharacter>(GetOwner())) {
				const auto& ownerLocation = owner->GetActorLocation();
				const auto ownerCapsule = owner->FindComponentByClass<UCapsuleComponent>();

				TOptional<FVector> location = locationquery::getRandomLocationAround(*GetWorld(), *owner, ownerLocation, TeleportRadius);
				if (!location) {
					UE_LOG(LogDungeons, Warning, TEXT("Teleport chance triggered but no new location could be found within the range"));
					return;
				}

				location.GetValue().Z += ownerCapsule->GetScaledCapsuleHalfHeight();

				if (UTeleportComponent* teleportComp = owner->FindComponentByClass<UTeleportComponent>()) {
					teleportComp->TeleportToPosition(location.GetValue(), true);

					FGameplayCueParameters params;
					owner->GetAbilitySystemComponent()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Teleport.End"), params);
				}
			}
		}
	}
}