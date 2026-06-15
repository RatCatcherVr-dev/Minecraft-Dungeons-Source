#include "Dungeons.h"
#include "CorpseAttractorComponent.h"
#include "game/util/ActorQuery.h"
#include "util/Random.h"
#include "game/util/Tags.h"
#include "HealthComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "RagdollOnDeathComponent.h"
#include "DungeonsGameMode.h"
#include <UnrealNetwork.h>
#include "game/abilities/effects/calculations/HealingModCalculations.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

UCorpseAttractorComponent::UCorpseAttractorComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.2f;
	bReplicates = true;
}

void UCorpseAttractorComponent::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		ADungeonsGameMode* gameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode());
		gameMode->ActorDeath.AddUObject(this, &UCorpseAttractorComponent::OnActorDeath);
	}
}

void UCorpseAttractorComponent::EndPlay(EEndPlayReason::Type Reason) {
	Super::EndPlay(Reason);

	for (auto mob : CurrentlyActiveMobs) {
		if (!mob.IsValid()) continue;
		if (auto ragdoll = mob->FindComponentByClass<URagdollOnDeathComponent>()) {
			ragdoll->DissolveSkin(removeTimeSeconds / 2.f);
		}
		if (GetOwner()->HasAuthority()) {
			mob->SetLifeSpan(removeTimeSeconds);
		}
	}
}

void UCorpseAttractorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCorpseAttractorComponent, CurrentlyActiveMobs);
}

void UCorpseAttractorComponent::TickCorpse(AMobCharacter* corpse, const FVector& targetLocation) {
	static Random rnd;

	const auto owner = GetOwner();
	// D11.DH
	// Characters are now being deleted over time to reduce the spikes when they are destroyed so the state of the character as a corpse isn't necessarily valid
	if (owner)
	{
		const auto ownerCharacter = Cast<ABaseCharacter>(owner);

			const auto ownerLocation = owner->GetActorLocation();
			auto meshComponent = corpse->GetMesh();
			if (meshComponent)
			{
				if (corpse->ActorHasTag(tags::collected)) {
					return;
				}

				const auto corpseLocation = meshComponent->GetComponentToWorld().GetLocation();
				const auto corpseVelocity = meshComponent->GetComponentVelocity();
				const auto direction = (targetLocation - corpseLocation).GetSafeNormal();

				const auto distance = FVector::Dist2D(targetLocation, corpseLocation);

				if (distance > 100.f && FMath::Abs(corpseVelocity.Z) > 10.f) {
					return;
				}

				if (distance > 500.f) {
					// make them go towards the cauldron in a zig-zaggy way
					const auto noisyDirection = direction.RotateAngleAxis(
						rnd.nextInt(1, 3) * 15.f * (rnd.nextBoolean() ? -1.f : 1.f),
						FVector::UpVector
					);

					meshComponent->SetAllPhysicsLinearVelocity(noisyDirection * kickAmount, false);

					const auto angularVelocity = FVector{ rnd.nextFloat(-1.f, 1.f), rnd.nextFloat(-1.f, 1.f), rnd.nextFloat(-1.f, 1.f) } *1000.f;
					meshComponent->SetAllPhysicsAngularVelocityInRadians(angularVelocity, false);
				}
				else if (distance > 100.f) {

					meshComponent->SetAllPhysicsLinearVelocity(direction * finalAmount, false);
				}
				else if (corpseLocation.Z - ownerLocation.Z > 200.f) {
					meshComponent->SetAllPhysicsLinearVelocity(direction * finalAmount, false);
				}
				else {
					meshComponent->SetAllPhysicsLinearVelocity(direction * kickAmount, false);
					auto ragdoll = corpse->FindComponentByClass<URagdollOnDeathComponent>();
					if (ragdoll)
					{
						ragdoll->DissolveSkin(removeTimeSeconds / 2.f);
					}
					corpse->Tags.Add(tags::collected);

					if (GetOwnerRole() == ROLE_Authority && ownerCharacter) {
						FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
						auto abilitySystem = ownerCharacter->GetAbilitySystemComponent();
						FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UCorpseAttractorHealGameplayEffect>(abilitySystem, 1.f);
						spec.SetSetByCallerMagnitude(effects::HealthName, mobHealthReward);
						spec.GetContext().AddInstigator(owner, owner);
						abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
					}
				}
		}
	}
}


void UCorpseAttractorComponent::OnActorDeath(AActor* Actor, AActor*, AActor*) {
	if (auto mobCharacter = Cast<AMobCharacter>(Actor)) {
		if (actorquery::getActorDistance(Actor, GetOwner()) <= radius) {
			auto ragdollDeath = mobCharacter->FindComponentByClass<URagdollOnDeathComponent>();
			if (ragdollDeath && ragdollDeath->TriggersOnDeath()) {
				CurrentlyActiveMobs.Emplace(mobCharacter);
				OnRep_CurrentlyActiveMobs();
			}
		}
	}
}

void UCorpseAttractorComponent::OnRep_CurrentlyActiveMobs() {
	TSet<TWeakObjectPtr<AMobCharacter>> tmp(CurrentlyActiveMobs);
	TSet<TWeakObjectPtr<AMobCharacter>> newMobs = tmp.Difference(LocalMobs);
	TSet<TWeakObjectPtr<AMobCharacter>> removedMobs = LocalMobs.Difference(tmp);

	bool hasAuthority = GetOwnerRole() == ENetRole::ROLE_Authority;

	if (hasAuthority) {
		for (auto mob : removedMobs) {
			if (!mob.IsValid()) continue;
			mob->SetLifeSpan(removeTimeSeconds);
		}
	}

	//New mobs should not time out.
	for (auto mob : newMobs) {
		if (!mob.IsValid()) continue;
		auto ragdoll = mob->FindComponentByClass<URagdollOnDeathComponent>();
		ragdoll->UnDissolveSkin();
		if (hasAuthority) {
			mob->SetLifeSpan(0.f);
		}
	}
	LocalMobs.Empty();
	LocalMobs.Append(CurrentlyActiveMobs);
}

void UCorpseAttractorComponent::TickComponent(float deltaTime, ELevelTick, FActorComponentTickFunction*) {
	const auto owner = GetOwner();
	const auto ownerLocation = owner->GetActorLocation();
	const auto targetLocation = ownerLocation + FVector::UpVector * 500.f;
	
	for (auto corpse : LocalMobs) {
		if (corpse.IsValid()) {
			TickCorpse(corpse.Get(), targetLocation);
		}
		
	}

	if (GetOwnerRole() == ROLE_Authority) {
		CurrentlyActiveMobs.RemoveAllSwap([](const TWeakObjectPtr<AMobCharacter> v) { return !v.IsValid() ||  v->ActorHasTag(tags::collected); });
		OnRep_CurrentlyActiveMobs();
	}
}

UCorpseAttractorHealGameplayEffect::UCorpseAttractorHealGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UDifficultyHealingModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);


	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Cauldron.CorpseEater"), 0, 100);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}