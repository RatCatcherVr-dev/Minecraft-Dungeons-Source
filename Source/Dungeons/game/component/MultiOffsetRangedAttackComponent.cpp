#include "MultiOffsetRangedAttackComponent.h"

#include "game/actor/character/BaseCharacter.h"
#include "game/item/instance/RangedWeaponGearItemInstance.h"


void UMultiOffsetRangedAttackComponent::PerformAttack(TWeakObjectPtr<AActor> attackTarget, ABaseCharacter* attacker, FItemId ammoType, float rangedAttackSpeedMultiplier, int32 seed)
{
	FVector attackerEmitLocation, attackDirection;
	if (USceneComponent* sComp = AttackOriginSceneComponent.GetComponent(GetOwner())) {
		attackerEmitLocation = sComp->GetComponentLocation();
		switch (AttackNormal)
		{
		case EAttackNormal::AttackOrigin:
			attackDirection = sComp->GetForwardVector();
			break;
		case EAttackNormal::OwningActor:
		default:
			attackDirection = GetAttackerBaseEmitVector(attacker);
			break;
		}
	}
	else {
		attackerEmitLocation = overrideOrigin.Get(attacker->GetActorLocation());
		attackDirection = GetAttackerBaseEmitVector(attacker);
	}

	auto overrideDefinition = RangedWeapon ? RangedWeapon->GetProjectileOverrideForItemType(ammoType) : nullptr;
	SpawnProjectileMulticast({ rangedAttackSpeedMultiplier, attackTarget.Get(), ammoType, attackerEmitLocation, attackDirection, AuthorativeArrowCount, seed }, IsCharged);
	OnBeginRangedAttack.Broadcast();
}
