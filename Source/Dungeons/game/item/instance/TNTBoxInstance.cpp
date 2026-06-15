#include "Dungeons.h"
#include "TNTBoxInstance.h"
#include "game/actor/item/TNTBoxItem.h"
#include "game/item/ItemType.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/AutoAimRangedAttackComponent.h"
#include "game/actor/character/player/BasePlayerController.h"

bool ATNTBoxInstance::CanActivate() const {
	return Super::CanActivate() && GetTntCount() < 3;
}

void ATNTBoxInstance::Activate(const FPredictionKey& predictionKey) {
	if (!HasAuthority())
		return;
	
	const auto owner = GetOwner();
	const auto count = GetTntCount();
	const auto* capsule = owner->FindComponentByClass<UCapsuleComponent>();
	const auto capsuleHalfHeight = capsule->GetScaledCapsuleHalfHeight();
	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	spawnParameters.Owner = owner;
	const auto& type = game::item::type::TNTBox;

	auto item = GetWorld()->SpawnActor<ATNTBoxItem>(ClassToSpawn, owner->GetActorLocation() + FVector(0, 0, (capsuleHalfHeight + 80.0f) + count * 120), FRotator::ZeroRotator, spawnParameters);
	if (item != nullptr) {
		item->AttachToActor(owner, FAttachmentTransformRules(EAttachmentRule::KeepWorld, true));
		Super::Activate(predictionKey);
		const APlayerCharacter* character = Cast<APlayerCharacter>(owner);
		item->EnableInput(Cast<APlayerController>(character->GetPlayerController()));

		if (UAutoAimRangedAttackComponent* rangedComponent = character->GetPlayerController()->GetRangedAttackComponent()) {
			rangedComponent->AddThrowable(item);
		}
	}
}

unsigned int ATNTBoxInstance::GetTntCount() const {
	unsigned int count = 0;
	TArray<AActor*> attached;
	GetOwner()->GetAttachedActors(attached); 
	for (auto&& actor : attached) {
		if (Cast<ATNTBoxItem>(actor)) {
			++count;
		}
	}
	return count;
}