#include "Dungeons.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "AreaBuffComponent.h"
#include <Engine.h>
#include <GameplayEffect.h>
#include "../abilities/effects/GameplayEffectUtil.h"
#include "game/util/Tags.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;
extern TAutoConsoleVariable<int32> CVarDebugDrawEnchantments;

UAreaBuffComponent::UAreaBuffComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	Effect = UAreaBuffGameplayEffect::StaticClass();
}

void UAreaBuffComponent::BeginPlay() {
	Super::BeginPlay();

	const auto ownerCharacter = GetOwnerCharacter();

	if (ownerCharacter->HasAuthority()) {
		OwnerAbilitySystem = ownerCharacter->GetAbilitySystemComponent();

		Sphere = NewObject<USphereComponent>(GetOwner());
		Sphere->SetGenerateOverlapEvents(true);
		Sphere->AttachTo(GetOwner()->GetRootComponent());
		Sphere->SetSphereRadius(AreaSize);
		Sphere->SetCollisionObjectType(ECC_WorldDynamic);
		Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		Sphere->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(ECustomTraceChannels::PlayerPawn), ECR_Overlap);
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &UAreaBuffComponent::OnSphereOverlapBegin);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &UAreaBuffComponent::OnSphereOverlapEnd);
		Sphere->RegisterComponent();

		// Actors already inside sphere when spawning does not trigger Overlap begin, so let's do it manually
		Sphere->UpdateOverlaps();

		if (CVarDebugDrawItems.GetValueOnGameThread() || CVarDebugDrawEnchantments.GetValueOnGameThread()) DrawDebugSphere(GetWorld(), Sphere->GetComponentLocation(), AreaSize, 50, FColor::Green, false, 5.0f);

		if (bBuffOwner) {
			FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Instigator);
			const auto effectToApply = effects::DefaultObject(Effect);
			if (effectToApply->StackingType == EGameplayEffectStackingType::AggregateByTarget && effectToApply->StackLimitCount == 1) {
				FGameplayEffectQuery query;
				query.EffectDefinition = Effect;
				//Make sure we replace the effect with the one applied from us.
				OwnerAbilitySystem->RemoveActiveEffects(query);
			}

			OwnerAbilitySystem->ApplyGameplayEffectSpecToSelf(CreateBuffSpec(Effect));
		}
	}
}

void UAreaBuffComponent::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	if (GetOwner()->HasAuthority()) {
		TArray<AActor*> characterActors;
		Sphere->GetOverlappingActors(characterActors, ABaseCharacter::StaticClass());

		for (auto characterActor : characterActors) {
			if (characterActor == GetOwnerCharacter() && !bBuffOwner) {
				continue;
			}
			const auto character = Cast<ABaseCharacter>(characterActor);
			character->GetAbilitySystemComponent()->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
			TryRemoveBuffedCharacter(character);
			character->OnTeamChanged.RemoveAll(this);
		}

		Sphere->OnComponentBeginOverlap.RemoveDynamic(this, &UAreaBuffComponent::OnSphereOverlapBegin);
		Sphere->OnComponentEndOverlap.RemoveDynamic(this, &UAreaBuffComponent::OnSphereOverlapEnd);
		Sphere->DestroyComponent();

		UnbindListenersForOverlappingCharacters();
	}
}

void UAreaBuffComponent::OnSphereOverlapBegin(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {		
	if (GetOwner()->Role == ROLE_Authority) {
		if (auto otherCharacter = Cast<ABaseCharacter>(OtherActor)) {
			if (IsValidTargetCharacter(otherCharacter)) {
				TryAddBuffedCharacter(otherCharacter);
			}

			OverlappingCharacterHandles.Emplace(otherCharacter, AddListenerForOverlappingCharacter(otherCharacter));
		}
	}
}

bool UAreaBuffComponent::CharacterEffectPersistsThroughDeath(const ABaseCharacter* otherCharacter) const {
	if (const auto* abilitySystem = otherCharacter->GetAbilitySystemComponent()) {
		TArray<FGameplayEffectSpec> specs;
		abilitySystem->GetAllActiveGameplayEffectSpecs(specs);
		return abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("PersistAfterDeath")));
	}
	return false;
}

TArray<TWeakObjectPtr<ABaseCharacter>> UAreaBuffComponent::GetOverlappingCharacters() {
	TArray<AActor*> characterActors;
	Sphere->GetOverlappingActors(characterActors, ABaseCharacter::StaticClass());
	TArray<TWeakObjectPtr<ABaseCharacter>> characters;
	for (auto character : characterActors) {
		characters.Emplace(Cast<ABaseCharacter>(character));
	}
	return characters;
}

void UAreaBuffComponent::OnGameplayEffectRemoved(const FActiveGameplayEffect& activeEffect, ABaseCharacter* target) {
	if (activeEffect.Spec.Def == effects::DefaultObject(Effect)) {
		TryUpdateBuffedCharacter(target);
	}
}

FOverlappingCharacterDelegateHandles UAreaBuffComponent::AddListenerForOverlappingCharacter(ABaseCharacter* character) {
	auto handles = FOverlappingCharacterDelegateHandles();
	
	handles.OnTeamChangedHandle = character->OnTeamChanged.AddUObject(this, &UAreaBuffComponent::TryUpdateBuffedCharacter, character);
	handles.OnGameplayEffectRemovedHandle = character->GetAbilitySystemComponent()->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UAreaBuffComponent::OnGameplayEffectRemoved, character);

	if (auto playerCharacter = Cast<APlayerCharacter>(character)) {
		handles.OnAliveStateChangedHandle = playerCharacter->OnAliveStateChanged.AddUObject(this, &UAreaBuffComponent::TryUpdateBuffedCharacter, character);
	} else {
		handles.OnDeathHandle = character->OnDeath.AddUObject(this, &UAreaBuffComponent::TryUpdateBuffedCharacter, character);
		handles.OnReviveHandle = character->OnRevive.AddUObject(this, &UAreaBuffComponent::TryUpdateBuffedCharacter, character);
	}

	return handles;
}

void UAreaBuffComponent::UnbindListenersForOverlappingCharacters() {
	for (auto handlePairs : OverlappingCharacterHandles) {
		handlePairs.Value.Unbind(handlePairs.Key);
	}
	OverlappingCharacterHandles.Empty();
}

void UAreaBuffComponent::UnbindListenersForOverlappingCharacter(ABaseCharacter* character) {
	if (const auto handles = OverlappingCharacterHandles.Find(character)) {
		handles->Unbind(character);
	}
	OverlappingCharacterHandles.Remove(character);
}

bool UAreaBuffComponent::IsValidTargetCharacter(const ABaseCharacter* otherCharacter) const {
	if (otherCharacter->IsAlive() || CharacterEffectPersistsThroughDeath(otherCharacter)) {
		if (const auto ownerCharacter = GetOwnerCharacter()) {
			if (ownerCharacter->IsFriendlyTowards(otherCharacter) &&
				!otherCharacter->ActorHasTag(tags::cosmetic)) {
				if (otherCharacter->IsA<AMobCharacter>()) {
					return bBuffFriendlyMobs;
				}
				
				if (otherCharacter->IsA<APlayerCharacter>()) {
					return true;
				}		
			}
		}	
	}	

	return false;
}

void UAreaBuffComponent::TryUpdateBuffedCharacter(ABaseCharacter* character) {
	if (IsValidTargetCharacter(character)) {
		TryAddBuffedCharacter(character);	
	} else {
		TryRemoveBuffedCharacter(character);
	}
}

void UAreaBuffComponent::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor->IsA<APlayerCharacter>() || OtherActor->IsA<AMobCharacter>()) {
		if (auto otherCharacter = Cast<ABaseCharacter>(OtherActor)) {
			UnbindListenersForOverlappingCharacter(otherCharacter);

			otherCharacter->GetAbilitySystemComponent()->OnAnyGameplayEffectRemovedDelegate().RemoveAll(this);
			TryRemoveBuffedCharacter(otherCharacter);
			otherCharacter->OnTeamChanged.RemoveAll(this);
		}
	}
}

void UAreaBuffComponent::TryAddBuffedCharacter(ABaseCharacter* character) {
	FGameplayEffectQuery query;
	query.EffectDefinition = Effect;
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Instigator);
	const auto otherCharacterAbilitySystem = character->GetAbilitySystemComponent();
	if (otherCharacterAbilitySystem->GetAggregatedStackCount(query) == 0) {
		OwnerAbilitySystem->ApplyGameplayEffectSpecToTarget(CreateBuffSpec(Effect), otherCharacterAbilitySystem);
	}
}

FGameplayEffectSpec UAreaBuffComponent::CreateBuffSpec(TSubclassOf<UGameplayEffect> effect) const {
	auto spec = FGameplayEffectSpec(Cast<UGameplayEffect>(effect->GetDefaultObject()), OwnerAbilitySystem->MakeEffectContext());
	spec.GetContext().AddSourceObject(this);

	for (auto i = Attributes.CreateConstIterator(); i; ++i) {
		const auto key = FName(*(i->Key.AttributeName));
		const auto value = i->Value;
		spec.SetSetByCallerMagnitude(key, value);
	}

	return spec;
}

void UAreaBuffComponent::TryRemoveBuffedCharacter(ABaseCharacter* character) {
	FGameplayEffectQuery query;
	query.EffectDefinition = Effect;
	query.EffectSource = this;

	auto* characterAbilitySystem = character->GetAbilitySystemComponent();
	characterAbilitySystem->RemoveActiveEffects(query, 1);
}

ABaseCharacter* UAreaBuffComponent::GetOwnerCharacter() const {
	auto ownerCharacter = EffectApplicationOwner.IsValid() ? EffectApplicationOwner.Get() : Cast<ABaseCharacter>(GetOwner());

	ensureMsgf(ownerCharacter, TEXT("Owner character has to be valid - If attached to a non BaseCharacter actor EffectApplicationOwner has to be provided"));

	return ownerCharacter;
}

void FOverlappingCharacterDelegateHandles::Unbind(ABaseCharacter* character) const {
	Unbind(character->OnTeamChanged, OnTeamChangedHandle);

	if (const auto playerCharacter = Cast<APlayerCharacter>(character)) {
		Unbind(playerCharacter->OnAliveStateChanged, OnAliveStateChangedHandle);
	} else {
		Unbind(character->OnDeath, OnDeathHandle);
		Unbind(character->OnRevive, OnReviveHandle);
	}
}

void FOverlappingCharacterDelegateHandles::Unbind(TMulticastDelegate<void> multicastDelegate, TOptional<FDelegateHandle> handle) {
	if (handle.IsSet()) {
		multicastDelegate.Remove(handle.GetValue());
	}
}

UAreaBuffGameplayEffect::UAreaBuffGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackLimitCount = 1;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::NeverRefresh;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
}
