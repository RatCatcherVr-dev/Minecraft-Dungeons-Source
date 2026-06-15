#include "BasePushVolume.h"
#include "game/actor/character/BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "game/component/PlayerCharacterMovementComponent.h"
#include <AbilitySystemGlobals.h>
#include <UnrealNetwork.h>

ABasePushVolume::ABasePushVolume() {
	SetReplicates(true);

	bCollideWhenPlacing = true;

	AffectorArea = CreateDefaultSubobject<UBoxComponent>(TEXT("MovementArea"));
	AffectorArea->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	AffectorArea->SetCanEverAffectNavigation(false);
	AffectorArea->SetBoxExtent(FVector{ 750, 200, 200 });
	AffectorArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AffectorArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AffectorArea->SetCollisionResponseToChannel((ECollisionChannel)ECustomTraceChannels::PlayerPawn, ECollisionResponse::ECR_Overlap);
	AffectorArea->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AffectorArea->SetCollisionResponseToChannel((ECollisionChannel)ECustomTraceChannels::PushReceiver, ECollisionResponse::ECR_Overlap);
	AffectorArea->SetCollisionObjectType((ECollisionChannel)ECustomTraceChannels::PushObject);
	SetRootComponent(AffectorArea);

	AffectorArea->OnComponentBeginOverlap.AddDynamic(this, &ABasePushVolume::OnWindAreaBeginOverlap);
	AffectorArea->OnComponentEndOverlap.AddDynamic(this, &ABasePushVolume::OnWindAreaEndOverlap);

	AffectorDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("MovementDirection"));
	AffectorDirection->AttachTo(AffectorArea);

	ImmunityTag = FGameplayTag::RequestGameplayTag("Immunity.PushVolume");

	PrimaryActorTick.bCanEverTick = true;
}

void ABasePushVolume::BeginPlay() {
	Super::BeginPlay();

	//make sure we are set up to hit the things we actually want, barring initial; settings

	if (HasAuthority()) {
		SetPushVolumeTimerActive(bIsTimeActivated);
		ReplicatedIsEnabled = VolumeIsEnabled;
	}

	OnRep_VolumeIsEnabled();
}

void ABasePushVolume::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	SetPushVolumeActive(false);
}

void ABasePushVolume::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABasePushVolume, ReplicatedIsEnabled);
}

void ABasePushVolume::SetPushVolumeExtent(FVector newExtent) {
	AffectorArea->SetBoxExtent(newExtent, true);
}

void ABasePushVolume::SetPushVolumeActive(bool active) {
	if (!HasAuthority()) return;

	if (HasActorBegunPlay()) {
		ReplicatedIsEnabled = active;
	}
	else {
		VolumeIsEnabled = active;
	}
	
	OnRep_VolumeIsEnabled();
}

void ABasePushVolume::ToggleActive() {
	if (HasActorBegunPlay()) {
		SetPushVolumeActive(!ReplicatedIsEnabled);
	}
	else {
		SetPushVolumeActive(!VolumeIsEnabled);
	}
}

void ABasePushVolume::SetPushVolumeTimerActive(bool active) {
	if (!HasAuthority()) return;

	bIsTimeActivated = active;
	if (active) {
		GetWorld()->GetTimerManager().SetTimer(ToggleTimerHandle, this, &ABasePushVolume::ToggleActive, TimeBetweenOnOff, true);
	}
	else {
		if (ToggleTimerHandle.IsValid()) {
			GetWorld()->GetTimerManager().ClearTimer(ToggleTimerHandle);
		}
	}
}

void ABasePushVolume::OnActorImmunityChanged(const FGameplayTag tag, const int32 tagCount, UPushVolumeReactiveComponent* recevier) {
	if (tagCount) {
		// We have immunity
		RemoveInfluence(recevier);
	}
	else {
		// We don't have immunity
		AddInfluence(recevier);
	}
}

void ABasePushVolume::OnWindAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	if (auto* recevierComponent = Cast<UPushVolumeReactiveComponent>(OtherComp)) {
		if (auto* abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor)) {
			abilitySystem->RegisterAndCallGameplayTagEvent(ImmunityTag, FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &ABasePushVolume::OnActorImmunityChanged, recevierComponent));
			if (abilitySystem->GetTagCount(ImmunityTag) <= 0) {
				AddInfluence(recevierComponent);
			}
		}
		else {
			AddInfluence(recevierComponent);
		}
	}
	else if (auto* capsuleComponent = Cast<UCapsuleComponent>(OtherComp))
	{
		if (auto* otherCharacter = Cast<ABaseCharacter>(OtherActor))
		{
			//character entered volume, lets enable its push functionality
			otherCharacter->EnablePushVolumes(true);
		}
	}
}

void ABasePushVolume::OnWindAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (auto* recevierComponent = Cast<UPushVolumeReactiveComponent>(OtherComp)) {
		if (auto* abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor)) {
			abilitySystem->RegisterGameplayTagEvent(ImmunityTag).RemoveAll(this);
		}

		RemoveInfluence(recevierComponent);
	}
}

void ABasePushVolume::AddInfluence(UPushVolumeReactiveComponent* receiverToAdd) {
	FPushVolumeInfluenceInstance newInstance(this, receiverToAdd, GetForce());
	if (receiverToAdd->ApplyPushVolumeInfluence(newInstance)) {
		ActiveInfluences.AddUnique(newInstance);
		OnInfluenceSuccessfullyAddedToActor(receiverToAdd);
	}
}

void ABasePushVolume::RemoveInfluence(UPushVolumeReactiveComponent* receiverToRemove) {
	for (FPushVolumeInfluenceInstance activeInfluence : ActiveInfluences.FilterByPredicate([&](const FPushVolumeInfluenceInstance& candidate) { return candidate.InfluencedComponent == receiverToRemove; })) {
		receiverToRemove->RemovePushVolumeInfluence(activeInfluence);
		ActiveInfluences.RemoveSwap(activeInfluence);
	}
	
	OnInfluenceSuccessfullyRemovedFromActor(receiverToRemove);
}

void ABasePushVolume::OnRep_VolumeIsEnabled() {
	SetActorEnableCollision(ReplicatedIsEnabled);
	ReplicatedIsEnabled ? OnPushVolumeActivated.Broadcast() : OnPushVolumeDeactivated.Broadcast();
}

void ABasePushVolume::OnPlayerInVisibleRangeChange(bool isVisible)
{
	Super::OnPlayerInVisibleRangeChange(isVisible);

	if (ToggleTimerHandle.IsValid())
	{
		if (isVisible)
		{
			GetWorld()->GetTimerManager().UnPauseTimer(ToggleTimerHandle);
		}
		else
		{
			GetWorld()->GetTimerManager().PauseTimer(ToggleTimerHandle);
		}
	}
}
