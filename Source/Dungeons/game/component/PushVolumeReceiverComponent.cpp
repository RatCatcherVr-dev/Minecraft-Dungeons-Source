#include "PushVolumeReceiverComponent.h"
#include "AbilitySystemComponent.h"
#include "PlayerCharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "game/util/ActorQuery.h"
#include "TimerManager.h"

UPushVolumeReactiveComponent::UPushVolumeReactiveComponent() {
	SetCollisionProfileName(FName(TEXT("PushReceiver")));
}

void UPushVolumeReactiveComponent::BeginPlay() {
	Super::BeginPlay();

	CachedAbilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	if (CachedAbilitySystem) {
		auto delegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &UPushVolumeReactiveComponent::OnResistancesChanged);
		CachedAbilitySystem->RegisterAndCallGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("Resistance.PushVolume")), delegate, EGameplayTagEventType::AnyCountChange);
	}
}

void UPushVolumeReactiveComponent::ProcessForces() {

}

bool UPushVolumeReactiveComponent::HasInstanceOfInfluence(FPushVolumeInfluenceInstance pushVolume) {
	return CurrentPushVolumes.Contains(pushVolume);
}

bool UPushVolumeReactiveComponent::ApplyPushVolumeInfluence(FPushVolumeInfluenceInstance pushVolume) {
	if (!CanVolumeInstanceBeAppliedToActor(pushVolume) || CurrentPushVolumes.Contains(pushVolume)) {
		return false;
	}

	CurrentPushVolumes.AddUnique(pushVolume);

	if (CurrentPushVolumes.Num() == 1) {
		OnReceiverActivated.Broadcast();
	}

	UpdateCumulativePushForce();
	return true;
}

bool UPushVolumeReactiveComponent::RemovePushVolumeInfluence(FPushVolumeInfluenceInstance pushVolume) {
	if (!CurrentPushVolumes.Contains(pushVolume)) {
		return false;
	}

	CurrentPushVolumes.RemoveSwap(pushVolume);

	if (CurrentPushVolumes.Num() == 0) {
		OnReceiverDeactivated.Broadcast();
	}

	UpdateCumulativePushForce();
	return true;
}

void UPushVolumeReactiveComponent::UpdateCumulativePushForce() {
	FVector vel = FVector::ZeroVector;
	FVector velBeforeResistance = FVector::ZeroVector;
	
	for (FPushVolumeInfluenceInstance volumeInstance : CurrentPushVolumes) {
		float resistance = 1;
		switch (volumeInstance.PushVolumeType) {
			case EPushVolumeType::Wind: {
				if (CachedAbilitySystem) {
					bool foundResistance = false;
					float possibleResistance = UAbilitySystemBlueprintLibrary::GetFloatAttributeFromAbilitySystemComponent(CachedAbilitySystem, UResistanceAttributeSet::WindResistanceMagnitudeAttribute(), foundResistance);
					if (foundResistance) {
						resistance = possibleResistance;
					}
				}
			}
			case EPushVolumeType::Current: {
				if (CachedAbilitySystem) {
					bool foundResistance = false;
					float possibleResistance = UAbilitySystemBlueprintLibrary::GetFloatAttributeFromAbilitySystemComponent(CachedAbilitySystem, UResistanceAttributeSet::CurrentResistanceMagnitudeAttribute(), foundResistance);
					if (foundResistance) {
						resistance = possibleResistance;
					}
				}
			}
			default:
				break;
		}

		vel += volumeInstance.PushVolumeVelocity * resistance;
		velBeforeResistance += volumeInstance.PushVolumeVelocity;
	}

	CachedCumulativePushForce = vel;
	ResistanceIgnoredCachedCumulativePushForce = velBeforeResistance;

	OnPushForceUpdated.Broadcast(CachedCumulativePushForce);

}

void UPushVolumeReactiveComponent::SetVolumeOverlapsEnabled(bool bEnable)
{
	SetCollisionEnabled((bEnable) ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	SetGenerateOverlapEvents(bEnable);
}

FVector UPushVolumeReactiveComponent::GetCumulativePushDirection() {
	FVector pushForce = GetCumulativePushForce();
	return pushForce / pushForce.Size();
}

FVector UPushVolumeReactiveComponent::GetResistanceIgnoredCumulativePushDirection()
{
	FVector pushForce = GetResistanceIgnoredCumulativePushForce();
	return pushForce / pushForce.Size();
}

void UPushVolumeReactiveComponent::OnResistancesChanged(FGameplayTag tag, int32 count) {
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &UPushVolumeReactiveComponent::UpdateCumulativePushForce), 0.1f, false);
}
