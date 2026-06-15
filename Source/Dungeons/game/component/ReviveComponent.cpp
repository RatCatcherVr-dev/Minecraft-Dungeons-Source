#include "Dungeons.h"
#include "ReviveComponent.h"
#include <Engine.h>
#include "game/actor/character/player/BasePlayerController.h"
#include "AbilitySystemComponent.h"
#include "ClientEventHub.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "DungeonsGameInstance.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "online/sessions/OnlineUtil.h"
#include "online/seasons/LiveOps.h"

UReviveFriendGameplayEffect::UReviveFriendGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FAttributeBasedFloat healthChange;
	healthChange.BackingAttribute.AttributeToCapture = UHealthAttributeSet::MaxHealthAttribute();
	healthChange.BackingAttribute.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	healthChange.Coefficient = 0.25f;
	healthChange.AttributeCalculationType = EAttributeBasedFloatCalculationType::AttributeMagnitude;

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthChange;
	healthInfo.ModifierOp = EGameplayModOp::Type::Override;
	Modifiers.Add(healthInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Revive")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Revive.Downed"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

UReviveComponent::UReviveComponent(): mReviveTime(3.0f) {
	bReplicates = true;
}

void UReviveComponent::BeginPlay() {
	Super::BeginPlay();

	RevivingPlayer = Cast<APlayerCharacter>(GetOwner());
}

float UReviveComponent::GetProgress() const {
	return FMath::Clamp((GetWorld()->GetTimeSeconds() - StartTimeStamp) / mReviveTime, 0.f, 1.f);
}

bool UReviveComponent::IsCurrentlyRevivingLocal() const {
	return TargetedPlayer != nullptr;
}

bool UReviveComponent::IsTargetingPlayer(const APlayerCharacter* player) const {
	return player == TargetedPlayer;
}

void UReviveComponent::ExecuteRevive(TWeakObjectPtr<APlayerCharacter> target) {
	if (target.IsValid()) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		//This means we wont treat as a cancel for the player that succeeds.
		target->OnPlayerRevive.RemoveAll(this);

		auto abilitySystem = RevivingPlayer->GetAbilitySystemComponent();
		const auto spec = effects::CreateGameplayEffectSpec<UReviveFriendGameplayEffect>(abilitySystem, 1);		
		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, target->GetAbilitySystemComponent());

		target->ApplyInvulnerability();
	}

	ClientEndRevive();
	RevivingPlayer->MulticastPlayAnimationAsDynamicMontage(ReviveSuccessAnimation, "UpperBody", 0.2f);
	RevivingPlayer->RemoveDownedPushImmunity();
	ReviveTimerHandle.Invalidate();
}

UHealthComponent* UReviveComponent::GetRevivingPlayerHealthComponent() const {
	return RevivingPlayer->FindComponentByClass<UHealthComponent>();
}

ABasePlayerController* UReviveComponent::GetRevivingPlayerController() const {
	return Cast<ABasePlayerController>(RevivingPlayer->GetController());
}

void UReviveComponent::OnAnyPlayerAction(const ABasePlayerController*) {
	ServerCancelRevive();
}

void UReviveComponent::ClientEndRevive_Implementation() {
	if (auto playerController = GetRevivingPlayerController()) {
		playerController->OnAnyPlayerAction.RemoveAll(this);
	}

	if (TargetedPlayer) {
		if (!TargetedPlayer->IsAlive()) {
			//Any Action is called too early in the frame, causing us to go back into interaction mode directly. Defer enabling of interaction to next frame.
			if(GetOwnerRole() == ROLE_Authority) {
				TargetedPlayer->GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(TargetedPlayer, &APlayerCharacter::MakePlayerClickable));
			} else {
				TargetedPlayer->MakePlayerClickable();
			}
		}
		// D11.SSN
		if (GetProgress() == 1.f) {
			RevivingPlayer->GetClientEventHubComponent()->ReviveFriend();
		}
	}

	TargetedPlayer = nullptr;
}

void UReviveComponent::ServerStartRevive_Implementation(APlayerCharacter* target) {	
	ServerCancelRevive();
	RevivingPlayer->MulticastPlayAnimationAsDynamicMontage(RevivingAnimation, "FullBody", 0.2f, 0.2f, 1.0f, -1);
	RevivingPlayer->ApplyDownedPushImmunity();

	if (auto healthComponent = GetRevivingPlayerHealthComponent()) {
		healthComponent->OnDeath.AddUObject(this, &UReviveComponent::ServerCancelRevive);
	}

	target->OnPlayerRevive.AddUObject(this, &UReviveComponent::ServerCancelRevive);

	GetWorld()->GetTimerManager().SetTimer(ReviveTimerHandle, FTimerDelegate::CreateUObject(this, &UReviveComponent::ExecuteRevive, TWeakObjectPtr<APlayerCharacter>(target)), mReviveTime, false);
}

bool UReviveComponent::ServerStartRevive_Validate(APlayerCharacter*) {
	return true;
}

void UReviveComponent::ServerCancelRevive_Implementation() {
	if (ReviveTimerHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(ReviveTimerHandle);
		ReviveTimerHandle.Invalidate();

		if (auto healthComponent = GetRevivingPlayerHealthComponent()) {
			healthComponent->OnDeath.RemoveAll(this);
		}

		RevivingPlayer->MulticastStopSlotAnimation("FullBody", 0.2f);
		RevivingPlayer->RemoveDownedPushImmunity();
		ClientEndRevive();
	}
}

bool UReviveComponent::ServerCancelRevive_Validate() {
	return true;
}

void UReviveComponent::Revive(APlayerCharacter* target) { 
	auto playerController = GetRevivingPlayerController();
	check(playerController && "Revive/Downed component currently only works on players.");

	if (TargetedPlayer) {
		ClientEndRevive();
		TargetedPlayer = nullptr;
	}

	StartTimeStamp = GetWorld()->GetTimeSeconds();
	TargetedPlayer = target;
	target->MakePlayerNonClickable();
	//Local Action should cancel the revive
	playerController->OnAnyPlayerAction.AddUObject(this, &UReviveComponent::OnAnyPlayerAction);
	playerController->TriggerControllerMovementCheck();
	RevivingPlayer->RotatePlayerTowardsActor(target);
	
	ServerStartRevive(target);
}

void UReviveComponent::EndPlay(const EEndPlayReason::Type reason)
{
	Super::EndPlay(reason);
	if(GetOwnerRole() == ENetRole::ROLE_Authority) {
		ServerCancelRevive();
	}
}
