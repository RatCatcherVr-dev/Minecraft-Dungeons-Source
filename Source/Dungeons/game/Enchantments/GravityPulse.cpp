// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "GravityPulse.h"
#include "util/CharacterQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/BaseCharacter.h"
#include <AbilitySystemComponent.h>

UGravityPulse::UGravityPulse() {
	PrimaryComponentTick.bCanEverTick = true;
	TypeId = EEnchantmentTypeID::GravityPulse;

	LevelMultiplier = [this](int level) -> float {
		return 1.0f + (float)(level-1) * 0.5f;
	};
	MultiplierFormatter = valueformat::asPercentage;
}



FText UGravityPulse::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asEveryRoundedWordSecond(PulseInterval)));
}

void UGravityPulse::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto& pullingPoint = GetOwner()->GetActorLocation();

	for (auto character : PullingCharacters) {
		if (character.IsValid()) {
			FVector pullDirection = pullingPoint - character->GetActorLocation();
			FHitResult res;
			character->PullCharacter(pullDirection * (DeltaTime * 5.0f), true, res, ETeleportType::TeleportPhysics);
		}
	}
}

void UGravityPulse::Execution() {
	if (GetOwner()->HasAuthority()) {
		OnPulse();
	}
}

float UGravityPulse::GetExecutionInterval() const {
	return PulseInterval;
}

void UGravityPulse::OnPulse() {
	auto characterOwner = GetCharacterOwner();
	auto ownerAbilitySystem = characterOwner->GetAbilitySystemComponent();
	const auto hostile = characterquery::is::hostile(characterOwner);
	const auto predicate = [&](const ABaseCharacter* v) { 
		return hostile( v ) && characterquery::is::movable( v ) &&  !characterquery::is::boss(v);
	};

	const float radius = BaseRadius * (characterOwner->IsA(AMobCharacter::StaticClass()) ? 2.0f : LevelMultiplier(Level));
	TArray<ABaseCharacter*> targets = actorquery::getNearbyActors<ABaseCharacter>(characterOwner, radius).FilterByPredicate(predicate);

	PullingCharacters.RemoveAllSwap(RETLAMBDA(!it.IsValid()));

	if (targets.Num()) {
		for (auto target : targets) {
			TryStartPullOnCharacter(target, ownerAbilitySystem);
		}
	}
}

void UGravityPulse::TryStartPullOnCharacter(TWeakObjectPtr<ABaseCharacter> character, UAbilitySystemComponent* ownerAbilitySystem) {
	if (!PullingCharacters.Contains(character)) {
		FGameplayCueParameters params;
		character->GetAbilitySystemComponent()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.GravityPulse"), FGameplayCueParameters());

		PullingCharacters.Emplace(character);

		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &UGravityPulse::EndPullOnCharacter, character), 0.5f, false);
	}
}

void UGravityPulse::EndPullOnCharacter(TWeakObjectPtr<ABaseCharacter> character) {
	if (character.IsValid() && PullingCharacters.Contains(character)) {
		PullingCharacters.Remove(character);
	}
}
