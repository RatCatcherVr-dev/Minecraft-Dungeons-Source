// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "DungeonsNavModifierComponent.h"


void UDungeonsNavModifierComponent::CalcAndCacheBounds() const {
	Super::CalcAndCacheBounds();
}


void UDungeonsNavModifierComponent::RecalculateBounds() {
	CalcAndCacheBounds();
}