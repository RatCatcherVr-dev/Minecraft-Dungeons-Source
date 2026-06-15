// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/component/SoulComponent.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include <AbilitySystemComponent.h>
#include "SoulHealer.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"


USoulHealGameplayEffect::USoulHealGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;
	FSetByCallerFloat healing;
	healing.DataName = FName(TEXT("Healing"));
	

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healing;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.SoulHealer.Target"), 0, 400);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

ASoulHealer::ASoulHealer() {
	PowerEffects = { UHealingIncrease::StaticClass() };
}

float ASoulHealer::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::LowestHealthHealed:
	case EItemStats::HighestHealthHealed:
		return GetItemType().getSoulCost() * HealAmountPerSoul;
	}
	return -1;
}

void ASoulHealer::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);

	const auto playerOwner = Cast<APlayerCharacter>(GetOwner());

	UAbilitySystemComponent* abilitySystem = playerOwner->GetAbilitySystemComponent();


	auto filter = [playerOwner](const ABaseCharacter* character) { return playerOwner->IsFriendlyTowards(character); };

	auto friends = actorquery::getNearbyActors<ABaseCharacter>(playerOwner, Radius).FilterByPredicate(filter);
	friends.Emplace(playerOwner);

	//Logic is to find the player in need of healing with the lowest health - but always revert back to owner if no player can be found.
	auto lowestHealthCharacter = GetCharacterLowestHealth(friends);
	auto applicableCharacter = lowestHealthCharacter ? lowestHealthCharacter : playerOwner;

	const float ItemPowerMultiplier = GetPowerEffect()->GetMultiplier(ItemPower);
	const float HealAmount = HealAmountPerSoul * GetSoulActivationCost() * ItemPowerMultiplier;
	const float SoulMagnitude = 1.0f;

	if (applicableCharacter == playerOwner) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		//Apply heal through GameplayEffect and piggyback summoner cue off that to reduce number of RPCS.
		auto spec = effects::CreateGameplayEffectSpec<USoulHealGameplayEffect>(abilitySystem, ItemPower);
		spec.SetSetByCallerMagnitude(FName(TEXT("Healing")), HealAmount);
		auto context = effects::GetDungeonsContextFromSpec(spec);
		context->AdditionalCues.Add(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.SoulHealer.Summoner"));
		abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);
	}
	else {
		if(HasAuthority()) {
			MulticastSpawnSoul(applicableCharacter, playerOwner->GetActorLocation(), HealAmount, SoulMagnitude);	
		}

		abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.SoulHealer.Summoner"), FGameplayCueParameters());
	}
}

void ASoulHealer::OnOrbHealedActor(AActor* actor, float magnitude) {
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(actor);
	
	FGameplayCueParameters params;
	params.NormalizedMagnitude = magnitude;
	params.Instigator = GetOwner();
	params.TargetAttachComponent = actor->GetRootComponent();
	abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.SoulHealer.Target"), params);
}

void ASoulHealer::MulticastSpawnSoul_Implementation(ABaseCharacter* target, const FVector& spawnLocation, float healAmount, float magnitude) {
	//Target might not be replicated to us.
	if(!target) return;
	
	FTransform spawnTransform;
	spawnTransform.SetLocation(spawnLocation);
	auto actor = GetWorld()->SpawnActorDeferred<AHealOnOverlapActor>(ActorClass, spawnTransform, target, Cast<APawn>(GetOwner()), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	actor->TargetHealthComponent = target->FindComponentByClass<UHealthComponent>();
	actor->healAmount = healAmount;
	actor->magnitude = magnitude;
	const float HealerOrbScale = 0.5f + magnitude;
	actor->SetActorScale3D(FVector(HealerOrbScale));
	UGameplayStatics::FinishSpawningActor(actor, spawnTransform);

	if(HasAuthority()) {
		actor->OnHealedActor.AddUObject(this, &ASoulHealer::OnOrbHealedActor);	
	}
}

ABaseCharacter* ASoulHealer::GetCharacterLowestHealth(const TArray<ABaseCharacter*>& characters) {
	ABaseCharacter* candidate = nullptr;
	
	if (characters.Num()) {
		float bestCandidatePercentage = 1.0f;

		for (auto character : characters) {
			if (const auto HC = character->FindComponentByClass<UHealthComponent>()) {
				if(!HC->IsHealthMaxed() && HC->IsAlive()){ //Only target players in need of healing
					const float characterHealthPercentage = HC->GetCurrentHealthPercentage();
					if (characterHealthPercentage < bestCandidatePercentage) {
						candidate = character;
						bestCandidatePercentage = characterHealthPercentage;
					}
				}				
			}			
		}
	}
	
	return candidate;
}
