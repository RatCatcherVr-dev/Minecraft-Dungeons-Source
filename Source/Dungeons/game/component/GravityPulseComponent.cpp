// © 2020 Mojang Synergies AB. TM Microsoft Corporation.


#include "GravityPulseComponent.h"
#include "Dungeons.h"
#include "util/CharacterQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/BaseCharacter.h"
#include <AbilitySystemComponent.h>

UGravityPulseComponent::UGravityPulseComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bHasStarted = false;
	bReplicates = true;
}

void UGravityPulseComponent::BeginPlay()
{
	Super::BeginPlay();

	const auto& owner = GetOwner();
	UpperRadialDistSq2D = FVector::DistSquared2D(owner->GetActorLocation(), owner->GetActorLocation() + owner->GetActorForwardVector() * BaseRadius);
}

void UGravityPulseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const auto& pullingPoint = GetOwner()->GetActorLocation() + PullToOffset;
	for (const auto& character : PullingCharacters) {
		if (character.IsValid()) {
			const auto& target = character->GetActorLocation();
			float lowerDistSq2D = FVector::DistSquared2D(pullingPoint, target);
			float distZFactor = lowerDistSq2D / UpperRadialDistSq2D;

			FVector pullDirection = pullingPoint - target;
			pullDirection.Normalize();
			pullDirection *= PullDistancePerPulse;
			pullDirection.Z *= 1.f + (bScaleZFactorByRadialDistance ? PullZFactor * distZFactor : PullZFactor);
			character->PullCharacter(pullDirection * (DeltaTime * 5.0f), true, FHitResult(), ETeleportType::TeleportPhysics);
		}
	}
}

void UGravityPulseComponent::SetPulseActive(bool isActive)
{
	SetComponentTickEnabled(isActive);
	bHasStarted = isActive;
	if (isActive) {
		OnIntervalExecute();
	}
}

void UGravityPulseComponent::Execution()
{
	if (GetOwner()->HasAuthority()) {
		OnPulse();
	}
}

void UGravityPulseComponent::OnPulse() {
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());
	auto ownerAbilitySystem = characterOwner->GetAbilitySystemComponent();
	const auto hostile = characterquery::is::hostile(characterOwner);
	const auto predicate = [&](const ABaseCharacter* v) {
	 	return hostile(v) && characterquery::is::movable(v) && !characterquery::is::boss(v);
	};
	 
	TArray<ABaseCharacter*> targets = actorquery::getNearbyActors<ABaseCharacter>(characterOwner, BaseRadius).FilterByPredicate(predicate);
	 
	if (targets.Num()) {
	 	for (auto target : targets) {
	 		TryStartPullOnCharacter(target, ownerAbilitySystem);
	 	}
	}
}

void UGravityPulseComponent::TryStartPullOnCharacter(TWeakObjectPtr<ABaseCharacter> character, UAbilitySystemComponent* ownerAbilitySystem) {
	if (!PullingCharacters.Contains(character)) {
		FGameplayCueParameters params;
		character->GetAbilitySystemComponent()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.GravityPulse"), FGameplayCueParameters());

		PullingCharacters.Emplace(character);

		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &UGravityPulseComponent::EndPullOnCharacter, character), 0.5f, false);
	}
}

void UGravityPulseComponent::EndPullOnCharacter(TWeakObjectPtr<ABaseCharacter> character) {
	if (character.IsValid() && PullingCharacters.Contains(character)) {
		PullingCharacters.Remove(character);
	}
}

void UGravityPulseComponent::OnIntervalExecute()
{
	if (ShouldIntervalExecute()) {
		Execution();
		ResetTimer();
	}
}

void UGravityPulseComponent::PlayGravityPulseAudio()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), GravityPulseAudio.sound, GetOwner()->GetActorLocation());
}

void UGravityPulseComponent::ResetTimer()
{
	GetWorld()->GetTimerManager().SetTimer(IntervalTimerHandle, this, &UGravityPulseComponent::OnIntervalExecute, GetExecutionInterval());
	GetWorld()->GetTimerManager().SetTimer(AudioTimerHandle, this, &UGravityPulseComponent::PlayGravityPulseAudio, GetExecutionInterval() - GravityPulseAudio.leadInTime);
}
