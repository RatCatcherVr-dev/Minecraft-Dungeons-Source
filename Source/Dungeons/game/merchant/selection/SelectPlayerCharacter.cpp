#include "Dungeons.h"
#include "SelectPlayerCharacter.h"
#include "util/Algo.hpp"
#include "DungeonsGameState.h"

void USelectPlayerCharacter::EnsureSelection() {
	if (GetPlayerCharacter() == nullptr) {
		auto players = GetSelectablePlayerCharacters();
		if (players.IsValidIndex(0)) {
			SelectPlayerCharacter(players[0]);
		}
	}
}

void USelectPlayerCharacter::ClearSelection() {
	if (GetPlayerCharacter()) {
		SelectPlayerCharacter(nullptr);
	}
}

bool USelectPlayerCharacter::IsPlayerCharacterSelectable(APlayerCharacter* playerCharacter) const {	
	return true;
}

void USelectPlayerCharacter::OnSetupSession() {
	Super::OnSetupSession();
	if (auto dungeonsgamestate = Cast<ADungeonsGameState>(GetWorld()->GetGameState())) {
		dungeonsgamestate->OnPlayerAddedOrRemovedInternal.AddUObject(this, &USelectPlayerCharacter::OnPlayerAddedOrRemoved);
	}
}

void USelectPlayerCharacter::OnCleanupSession() {
	Super::OnCleanupSession();
	if (auto dungeonsgamestate = Cast<ADungeonsGameState>(GetWorld()->GetGameState())) {
		dungeonsgamestate->OnPlayerAddedOrRemovedInternal.RemoveAll(this);
	}
}

void USelectPlayerCharacter::OnPlayerAddedOrRemoved() {
	SelectionSelectableChanged();	
}

APlayerCharacter* USelectPlayerCharacter::GetPlayerCharacter() const {
	return mSelectedPlayerCharacter;
}

TArray<APlayerCharacter*> USelectPlayerCharacter::GetSelectablePlayerCharacters() const {
	return algo::copy_if(InstanceTracker<APlayerCharacter>::GetList(GetWorld()), RETLAMBDA(IsPlayerCharacterSelectable(it)));
}

void USelectPlayerCharacter::SelectPlayerCharacter(APlayerCharacter* playerCharacter) {
	if(mSelectedPlayerCharacter != playerCharacter){
		mSelectedPlayerCharacter = playerCharacter;
		SelectionChanged();
	} else if (playerCharacter) {
		TryConfirmSelection();
	}
}

bool USelectPlayerCharacter::CanSelectAny() const {
	return GetSelectablePlayerCharacters().Num() > 0;
}

bool USelectPlayerCharacter::HasSelectedAny() const {
	return GetPlayerCharacter() != nullptr;
}
