#include "Dungeons.h"
#include "ConduitInstance.h"
#include "game/actor/item/ConduitItem.h"
#include "game/item/ItemType.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/AutoAimRangedAttackComponent.h"
#include "game/actor/character/player/BasePlayerController.h"

bool AConduitInstance::CanActivate() const {
	return Super::CanActivate() && GetConduitCount() < 1;
}

void AConduitInstance::Activate(const FPredictionKey& predictionKey)
{
	if (!HasAuthority())
	{
		return;
	}

	const auto owner = GetOwner();
	const unsigned int count = GetConduitCount();
	const auto* capsule = owner->FindComponentByClass<UCapsuleComponent>();
	const auto capsuleHalfHeight = capsule->GetScaledCapsuleHalfHeight();
	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	spawnParameters.Owner = owner;

	auto item = GetWorld()->SpawnActor<AConduitItem>(ClassToSpawn, owner->GetActorLocation() + FVector(0, 0, (capsuleHalfHeight + 80.0f) + count * 120), FRotator::ZeroRotator, spawnParameters);
	if (item != nullptr)
	{
		item->AttachToActor(owner, FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true));
		Super::Activate(predictionKey);
		const APlayerCharacter* character = Cast<APlayerCharacter>(owner);
		item->EnableInput(Cast<APlayerController>(character->GetPlayerController()));

		if (UAutoAimRangedAttackComponent* rangedComponent = character->GetPlayerController()->GetRangedAttackComponent()) {
			rangedComponent->AddThrowable(item);
		}
	}
}

unsigned int AConduitInstance::GetConduitCount() const
{
	unsigned int count = 0;
	TArray<AActor*> attached;
	GetOwner()->GetAttachedActors(attached);
	for (auto&& actor : attached) {
		if (Cast<AConduitItem>(actor)) {
			++count;
		}
	}
	return count;
}