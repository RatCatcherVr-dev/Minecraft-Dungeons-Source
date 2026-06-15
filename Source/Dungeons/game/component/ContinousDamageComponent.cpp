#include "ContinousDamageComponent.h"
#include <Components/BoxComponent.h>
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "util/CharacterQuery.h"
#include "game/team/TeamQuery.h"
#include "GameplayEffectTypes.h"

namespace continousdamage { namespace can {

	bool damage(const TOptional<FGameplayEffectSpecHandle>& Spec, const ABaseCharacter* OptionalBaseCharacter, const ABaseCharacter* Damagee) {
		if (Spec && Spec->Data) {
			FGameplayTagContainer tags;
			Spec->Data->GetAllAssetTags(tags);

			const bool canDamageFriends = tags.HasTag(damageTag::damageFriends());
			const auto team = effects::GetDungeonsContextFromSpec(*Spec->Data)->InstigatorTeam;

			return canDamageFriends ||
				teamquery::can::damage(team, Damagee->GetCurrentTeam());
		}
		else if (OptionalBaseCharacter) {
			return characterquery::can::damage(OptionalBaseCharacter)(Damagee);
		}
		return true;
	}
}}

UContinousDamageComponent::UContinousDamageComponent() {
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.2f;
}

void UContinousDamageComponent::SetDamagePerSecond(float dps) {
	DamagePerSecond = dps;
}

void UContinousDamageComponent::SetInstigator(APawn* instigator) {
	Instigator = instigator;
}

void UContinousDamageComponent::SetEffectClass(TSubclassOf<class UGameplayEffect> effectClass) {
	EffectClass = effectClass;
}

void UContinousDamageComponent::BeginPlay() {
	Super::BeginPlay();
	if (GetOwner()->HasAuthority()) {
		if (auto shape = GetOwner()->FindComponentByClass<UShapeComponent>()) {
			shape->OnComponentBeginOverlap.AddDynamic(this, &UContinousDamageComponent::OnComponentBeginOverlap);
			shape->OnComponentEndOverlap.AddDynamic(this, &UContinousDamageComponent::OnComponentEndOverlap);
		}
	}
}

void UContinousDamageComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bContinousApplication) {
		//Damaging a character can cause it to be destroyed and evicted from the collision. This causes CharactersInCollision to change
		//while iterating (so we cannot use a range based for loop).
		TArray<ABaseCharacter*> characters;
		CharactersInCollision.GenerateKeyArray(characters);
		for (auto character : characters) {
			if (character && CanDamage(character)) {
				ApplyDamageEffect(character);
			}
		}
	}
}

void UContinousDamageComponent::DisableDamage() {
	DisableOverlaps();
	RemoveAllDamagedCharacters();
}

void UContinousDamageComponent::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (const auto capsule = Cast<UCapsuleComponent>(OtherComp)) {
		if (auto otherCharacter = Cast<ABaseCharacter>(OtherActor)) {
			if (CanDamage(otherCharacter)) {
				if (!CharactersInCollision.Contains(otherCharacter)) {
					auto handle = ApplyDamageEffect(otherCharacter);
					CharactersInCollision.Emplace(otherCharacter, handle);
				}
			}
		}
	}
}

void UContinousDamageComponent::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (bRemoveEffectEndOverlap) {
		if (const auto capsule = Cast<UCapsuleComponent>(OtherComp)) {
			if (auto otherCharacter = Cast<ABaseCharacter>(OtherActor)) {
				if (CharactersInCollision.Contains(otherCharacter)) {
					auto handle = CharactersInCollision[otherCharacter];
					RemoveDamageEffect(otherCharacter, handle);
					CharactersInCollision.Remove(otherCharacter);
				}
			}
		}
	}
}

FActiveGameplayEffectHandle UContinousDamageComponent::ApplyDamageEffect(ABaseCharacter* targetCharacter) const {
	auto* characterInstigator = Cast<ABaseCharacter>(GetInstigator());

	ensureMsgf(PredefinedSpec || (EffectClass && characterInstigator), TEXT("A precaptured Spec or an Effect and valid instigator must be present."));

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);

	if ((PredefinedSpec || EffectClass) && characterInstigator) {
		auto instigatorAbilitySystem = characterInstigator->GetAbilitySystemComponent();
		auto spec = [&]() {
			if (PredefinedSpec) {
				return *PredefinedSpec->Data.Get();
			}
			return effects::CreateGameplayEffectSpecFromSubClass(instigatorAbilitySystem, EffectClass);
		}();
		FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(spec);
		context->AddInstigator(GetOwner(), characterInstigator);
		context->AddOrigin(GetOwner()->GetActorLocation());
		const auto period = spec.Period;
		spec.SetSetByCallerMagnitude(effects::HealthName, -DamagePerSecond * period);
		effects::StorePushbackInNormal(spec, FVector::ZeroVector);
		if (spec.Def->DurationPolicy == EGameplayEffectDurationType::HasDuration) {
			spec.SetSetByCallerMagnitude(effects::DurationName, EffectDuration);
		}
		return instigatorAbilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetCharacter->GetAbilitySystemComponent());
	}
	return FActiveGameplayEffectHandle();
}

bool UContinousDamageComponent::RemoveDamageEffect(ABaseCharacter* targetCharacter, FActiveGameplayEffectHandle handle) const {
	if (auto targetAbilitySystem = targetCharacter->GetAbilitySystemComponent()) {
		return targetAbilitySystem->RemoveActiveGameplayEffect(handle, 1);
	}
	return false;
}

void UContinousDamageComponent::RemoveAllDamagedCharacters() {
	TArray<ABaseCharacter*> characters;
	CharactersInCollision.GenerateKeyArray(characters);

	for(auto* character : characters) {
		if(character) {
			RemoveDamageEffect(character, CharactersInCollision[character]);
		}
	}

	CharactersInCollision.Empty();
}

bool UContinousDamageComponent::CanDamage(ABaseCharacter* character) const {
	const bool isDamageable = continousdamage::can::damage(PredefinedSpec, Cast<ABaseCharacter>(GetInstigator()), character);
	const bool isDamageableAndTargetable = isDamageable && characterquery::is::targetable(character);
	const bool isDamageableTargetableAndAlive = isDamageableAndTargetable && actorquery::is::alive(character);
	return isDamageableTargetableAndAlive;
}

void UContinousDamageComponent::DisableOverlaps() {
	if (auto shape = GetOwner()->FindComponentByClass<UShapeComponent>()) {
		shape->SetGenerateOverlapEvents(false);
	}
}

ABaseCharacter* UContinousDamageComponent::GetInstigator() const {
	return Cast<ABaseCharacter>(Instigator ? Instigator : GetOwner());
}

void UContinousDamageComponent::SetGameplayEffectSpec(const FGameplayEffectSpecHandle& spec) {
	PredefinedSpec = spec;
}
