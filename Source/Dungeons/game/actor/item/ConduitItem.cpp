#include "Dungeons.h"
#include "ConduitItem.h"
#include "AIController.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "UnrealNetwork.h"
#include "game/util/Pushback.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "StatTracker.h"
#include "DungeonsGameInstance.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/ArrowItemSlot.h"
#include "Kismet/KismetMathLibrary.h"
#include "game/component/PlayerCharacterMovementComponent.h"
#include "game/component/ReplicatedInteractableComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "game/item/ItemUtil.h"
#include "../DarkConduitInteraction.h"
#include "../ConduitButton.h"

AConduitItem::AConduitItem(const class FObjectInitializer& OI) : Super(OI) {
	IsHeldByPlayer = true;
	PrimaryActorTick.bCanEverTick = true;
	EffectRadius = 750;
	EffectTime = 1.0f;
	SetReplicates(true);
	EffectTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Conduit"));
	SlotType = ESlotType::Conduit;
	ThrowType = EThrowableType::Conduit;

	TArray<AConduitButton*>& conduitButtons = InstanceTracker<AConduitButton>::GetList(GetWorld());
	for (AConduitButton* conduitButton : conduitButtons) {
		conduitButton->OnConduitHeld();
	}

	LaunchOverlapDelegate.BindUObject(this, &AConduitItem::LaunchOverlapDone);
}

void AConduitItem::FellOutOfWorld(const class UDamageType& dmgType) {
	TeleportToSafeLocation();
}

void AConduitItem::OutsideWorldBounds() {
	TeleportToSafeLocation();
}

void AConduitItem::Tick(float DeltaSeconds)
{
	if (PendingActivatedUpdate)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		OnActivated();
		PendingActivatedUpdate = false;
	}

	//TODO : Rate limit 
	if (IsHeldByPlayer)
	{
		FVector conduitPos = GetActorLocation();
		TArray<ADarkConduitInteraction*>& darkConduitInteractions = InstanceTracker<ADarkConduitInteraction>::GetList(GetWorld());

		for (ADarkConduitInteraction* darkConduitInteraction : darkConduitInteractions) {
			const float DistSq = FVector::DistSquaredXY(conduitPos, darkConduitInteraction->GetActorLocation());
			darkConduitInteraction->TriggerOnInConduitRangeChanged(DistSq);
		}
	}

	if (GetInPlayerVisibleRange()) {
		UpdateMaterials();
	}
}

void AConduitItem::ThrowInMouseDir()
{
	if (APlayerCharacter* owningPlayerCharacter = Cast<APlayerCharacter>(GetOwner()))
	{
		if (APlayerController* owningPlayerController = owningPlayerCharacter->GetPlayerController())
		{
			DisableInput(nullptr);

			const auto controller = Cast<ABasePlayerController>(owningPlayerController);
			const auto mouseWorldPos = GetMouseClickedWorldPosition(controller);
			auto dir = mouseWorldPos - GetActorLocation();
			dir.Z = 0;

			Throw(dir.SizeSquared() > 1e-6 ? dir.GetUnsafeNormal() : GetOwner()->GetActorForwardVector());
		}
	}
}

void AConduitItem::Activate(const FVector& dir)
{
	Super::Activate(dir);

	LaunchImpulse = dir;
	if (!IsFirstInStack())
	{
		LaunchImpulse = dir.RotateAngleAxis(FMath::FRandRange(-30, 30), FVector(0, 0, 1)) * FMath::FRandRange(0.7, 1.3);
	}
	if (HasAuthority())
	{
		auto collisionMesh = Cast<UBoxComponent>(GetRootComponent());

		static FName TraceTagName(TEXT("AConduitItemLaunch"));
		FCollisionQueryParams QueryParams(TraceTagName, false, GetOwner());

		auto* world = GetWorld();
		world->AsyncOverlapByChannel(collisionMesh->GetComponentLocation(), FQuat::Identity, (ECollisionChannel)ECustomTraceChannels::TerrainOnly, collisionMesh->GetCollisionShape(-10.0f), QueryParams, FCollisionResponseParams::DefaultResponseParam, &LaunchOverlapDelegate);
		world->GetTimerManager().SetTimer(EffectTimerHandler, this, &AConduitItem::Emplace, EffectTime);
	
		auto comp = Cast<UReplicatedInteractableComponent>(GetComponentByClass(UReplicatedInteractableComponent::StaticClass()));
		comp->SetTextDesc(game::item::type::Conduit.getNameText(), game::item::type::Conduit.getDescriptionText());

		TArray<AConduitButton*>& conduitButtons = InstanceTracker<AConduitButton>::GetList(GetWorld());
		for (AConduitButton* conduitButton : conduitButtons) {
			conduitButton->OnConduitDropped();
		}
	}
}

void AConduitItem::ThrowServer_Implementation(const FVector& dir)
{
	IsHeldByPlayer = false;
	OnRep_IsHeldByPlayer();
	Activate(600 * dir);
	if (Slot) Slot->Consume();
}

void AConduitItem::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) {
	if (HitNormal.Z > 0 && FMath::Abs(GetVelocity().Z) <= UPhysicsSettings::Get()->BounceThresholdVelocity) {
		OnLandedInternal();
	}
}

void AConduitItem::OnLandedInternal() {
	if (IsInUnsafeLocation()) {
		TeleportToSafeLocation();
	}
}

bool AConduitItem::IsInUnsafeLocation() const {
	if (const auto* owner = Cast<ABaseCharacter>(GetOwner())) {
		return !owner->IsLocationReachable(GetActorLocation());
	}
	return false;
}

void AConduitItem::TeleportToSafeLocation() {
	const ABaseCharacter* target = GetTeleportTarget();
	UBoxComponent* collisionComponent = GetCollisionComponent();
	if (target && collisionComponent) {
		auto newLocation = target->GetActorLocation();
		newLocation.Z += target->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 + 100.0f;
		collisionComponent->SetAllPhysicsLinearVelocity(FVector::ZeroVector, false);
		collisionComponent->SetWorldLocation(newLocation, false, nullptr, ETeleportType::ResetPhysics);
	}
	else {
		Destroy();
	}
}

ABaseCharacter* AConduitItem::GetTeleportTarget() const {
	if (auto* owner = Cast<ABaseCharacter>(GetOwner())) {
		return owner;
	}
	return actorquery::getFirstActor<APlayerCharacter>(GetWorld());
}

UBoxComponent* AConduitItem::GetCollisionComponent() const {
	return static_cast<UBoxComponent*>(GetRootComponent());
}

void AConduitItem::LaunchOverlapDone(const FTraceHandle& TraceHandle, FOverlapDatum & TraceData)
{
	auto collisionMesh = Cast<UBoxComponent>(GetRootComponent());

	//D11.SC if we are overlapping the terrain on launch, move the box to the player center and launch from there to prevent the physics intersection spamming out
	if(TraceData.OutOverlaps.Num() > 0)
	{
		if (auto* owner = Cast<ABaseCharacter>(GetOwner())) {
			auto newLocation = owner->GetActorLocation();
			newLocation.Z += owner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			collisionMesh->SetWorldLocation(newLocation, false, nullptr, ETeleportType::ResetPhysics);
		}
	}

	collisionMesh->SetSimulatePhysics(true);
	collisionMesh->AddImpulse(LaunchImpulse, NAME_None, true);
}

void AConduitItem::GiveConduitToCharacter(ACharacter* Character)
{
	FInventoryItemData inventoryItemData = FInventoryItemData(game::item::type::Conduit.getId());
	AStorableItem* ConduitStorable = game::item::util::spawnStorableItem(*GetWorld(), Character->GetActorLocation(), inventoryItemData, {}, Character, true);
	ConduitStorable->HandleInteraction(Character);
}

void AConduitItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AConduitItem, IsHeldByPlayer);
}

void AConduitItem::OnRep_IsHeldByPlayer()
{
	//Turn off all interactions once not held
	if (!IsHeldByPlayer)
	{
		TArray<ADarkConduitInteraction*>& darkConduitInteractions = InstanceTracker<ADarkConduitInteraction>::GetList(GetWorld());
		for (ADarkConduitInteraction* darkConduitInteraction : darkConduitInteractions) {
			darkConduitInteraction->TriggerOnInConduitRangeChanged(false);
		}
	}
}