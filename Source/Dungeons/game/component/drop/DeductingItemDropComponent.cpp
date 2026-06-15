#include "Dungeons.h"
#include "DeductingItemDropComponent.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "GameplayEffectExtension.h"

UDeductingItemDropComponent::UDeductingItemDropComponent(): DeductAmount(1) {}

void UDeductingItemDropComponent::OnMobDamaged(const FOnAttributeChangeData& data) {
	auto damageType = damageTag::def();
	auto instigator = GetOwner();

	if (data.GEModData) {
		const auto& handle = data.GEModData->EffectSpec.GetEffectContext();
		instigator = handle.GetInstigator();
	}

	ServerDeductItems(GetOwner()->GetActorLocation(), instigator, GetOwner());
}

bool UDeductingItemDropComponent::ServerDeductItems_Validate(const FVector& spawnLocation, AActor* triggeringPlayer, AActor* source) {
	return true;
}

void UDeductingItemDropComponent::ServerDeductItems_Implementation(const FVector& spawnLocation, AActor* triggeringPlayer, AActor* source) {
	const auto count = DeductAmount >= DropData.MaxAmount ? DropData.MaxAmount : DeductAmount;
	if (count > 0) {
		const FItemDropSource dropSource(spawnLocation, source, Cast<APlayerCharacter>(triggeringPlayer));
		const FItemDrop drop(DropData.Category, DeductAmount, DeductAmount);
		if (const auto dropActual = FNetworkedItemDropData::FromFItemDrop(drop)) {
			DropItems(dropSource, dropActual.GetValue(), source);
		}

		DecreaseRemainingDropAmount(count);
	}	
}

void UDeductingItemDropComponent::DecreaseRemainingDropAmount(const int count) {
	DropData.MaxAmount -= count;
	DropData.MinAmount = FMath::Max(0, DropData.MinAmount - count);
}
