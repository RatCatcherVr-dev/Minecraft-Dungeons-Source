#include "Dungeons.h"
#include "TridentInstance.h"
#include "game/actor/item/TridentItem.h"
#include "game/item/ItemType.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/AutoAimRangedAttackComponent.h"
#include "game/actor/character/player/BasePlayerController.h"

bool ATridentInstance::CanActivate() const {
	return Super::CanActivate() && GetTridentCount() < 3;
}

void ATridentInstance::Activate(const FPredictionKey& predictionKey) {
	if (!HasAuthority())
		return;
	
	const APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner());
	const auto count = GetTridentCount();
	const auto* capsule = owner->FindComponentByClass<UCapsuleComponent>();
	const auto capsuleRadius = capsule->GetScaledCapsuleRadius();
	const auto capsuleHalfHeight = capsule->GetScaledCapsuleHalfHeight();
	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParameters.Owner = GetOwner();
	const auto& type = game::item::type::Trident;

	FRotator transformRotation = owner->GetMesh()->GetSocketRotation("J_BackTridentSocket");
	transformRotation.Add(20 * GetTridentCount(), 20 * GetTridentCount(), 20 * GetTridentCount());

	FVector transformLocation = owner->GetMesh()->GetSocketLocation("J_BackTridentSocket");
	
	auto item = GetWorld()->SpawnActor<ATridentItem>(ClassToSpawn, transformLocation, transformRotation, spawnParameters);
	if (item != nullptr) {
		item->AttachToComponent(owner->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true), "J_BackTridentSocket");
		Super::Activate(predictionKey);
		const APlayerCharacter* character = Cast<APlayerCharacter>(owner);
		item->EnableInput(Cast<APlayerController>(character->GetPlayerController()));

		if (UAutoAimRangedAttackComponent* rangedComponent = character->GetPlayerController()->GetRangedAttackComponent()) {
			rangedComponent->AddThrowable(item);
		}
	}
}

unsigned int ATridentInstance::GetTridentCount() const {
	unsigned int count = 0;
	TArray<AActor*> attached;
	GetOwner()->GetAttachedActors(attached); 
	for (auto&& actor : attached) {
		if (Cast<ATridentItem>(actor)) {
			++count;
		}
	}
	return count;
}
