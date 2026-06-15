#include "Dungeons.h"
#include "StorableItem.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/Conversion.h"
#include "game/GameBP.h"
#include "game/component/EquipmentComponent.h"
#include "game/util/SimpleMovementComponent.h"
#include "game/component/MapPinComponent.h"
#include "lovika/LovikaLevelActor.h"
#include "game/component/PickupStorableComponent.h"
#include "game/component/InteractableComponent.h"
#include "client/renderer/block/BlockGraphicsPack.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Engine/EngineTypes.h"
#include "RuntimeMeshComponent.h"
#include "game/actor/ProjectileManager.h"

#include "game/component/AutoAimRangedAttackComponent.h"

namespace storableItem {
	FVector getLocationAbove(const ABaseCharacter* character, const float sphereRadius) {
		auto newLocation = character->GetActorLocation();
		newLocation.Z += character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2 + sphereRadius * 2;
		return newLocation;
	}
}

AStorableItem::AStorableItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, stored(false)
	, AliveFrames(0)
{
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	
	Sphere->BodyInstance.bLockXRotation = true;
	Sphere->BodyInstance.bLockYRotation = true;
	Sphere->SetIsReplicated(false);

	Sphere->SetMobility(EComponentMobility::Movable);
	Sphere->SetSimulatePhysics(true);
	Sphere->SetAngularDamping(10000.0f);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Sphere->SetEnableGravity(true);
	Sphere->SetSphereRadius(72.0f);
	
	RootComponent = Sphere;
	Sphere->bEditableWhenInherited = true;

	PrimaryActorTick.bCanEverTick = true;

	bAlwaysRelevant = false;
	SetReplicates(false);
	AActor::SetReplicateMovement(true);

	MapPinComponent = CreateDefaultSubobject<UMapPinComponent>(TEXT("MapPinComponent"));

	SimpleMovementComponent = CreateDefaultSubobject<USimpleMovementComponent>(TEXT("Simple Movement Component"));
	SimpleMovementComponent->SetIsReplicated(true);
	
	ItemDropEffect = CreateDefaultSubobject<UItemDropEffectComponent>(TEXT("Item Drop Effect"));
}

USphereComponent* AStorableItem::GetCollisionComponent() const {
	return static_cast<USphereComponent*>(GetRootComponent());
}

void AStorableItem::BeginPlay() {
	Super::BeginPlay();
	InstanceTracker< AStorableItem >::AddInstance(this->GetWorld(), this);

	if(HasAuthority()) {
		ServerSpawnTimeSeconds = GetWorld()->GetTimeSeconds();
	}

	if (MapPinComponent)
	{
		const auto& ThisItemType = GetItemType();
		bool bInvalidMapPin =  (!(ThisItemType.isGear() || ThisItemType.isToken()) && !ThisItemType.isPermanent());
		
		if (!MapPinComponent->HasAnyMapPinClasses() || bInvalidMapPin)
		{
			//No map pin markers for this, remove map pin to reduce processing costs ( id prefer not to create it in the first place if its not being used, but at this point theres too many dependants)
			MapPinComponent->RemoveFromRoot();
			MapPinComponent->DestroyComponent();
			MapPinComponent = nullptr;
		}
	}


	DisableTime_Current = DisableTimer;

	Game = actorquery::getFirstActor<AGameBP>(GetWorld());
	if (SimpleMovementComponent) 
	{
		SimpleMovementComponent->bUpdateOnlyIfRendered = true; //D11.SC only update this movement component if its on the screen
		if (Rotate)
		{
			SetActorRotation(FRotator(0, FMath::RandRange(0.f, 360.f), 0.f));
			SimpleMovementComponent->SetRotationSpeed(RotationSpeed, PendelSpan);
		}
	}

	if (HasAuthority()) {
		SetActorTickEnabled(true);
	}

	if(ShouldPlayDropSound()) {
		Play(DropSound);	
	}

	if (auto clicky = FindComponentByClass<UInteractableComponent>()) {
		clicky->OnInteracted.AddDynamic(this, &AStorableItem::HandleInteraction);
	}

	AttachmentCollisionHandle = FTraceHandle(); //reset handle
}

void AStorableItem::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	InstanceTracker< AStorableItem >::RemoveInstance(this->GetWorld(), this);
}

void AStorableItem::FellOutOfWorld(const class UDamageType& dmgType) {
	if (HasValuableType()) {
		TeleportToSafeLocation();
	} else if (!ReturnToPool()) {
		Super::FellOutOfWorld(dmgType);
	}	
}

void AStorableItem::OutsideWorldBounds() {
	if (HasValuableType()) {
		TeleportToSafeLocation();	
	} else if (!ReturnToPool()) {
		Super::OutsideWorldBounds();
	}
}

void AStorableItem::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) {
	if (HitNormal.Z > 0 && FMath::Abs(GetVelocity().Z) <= UPhysicsSettings::Get()->BounceThresholdVelocity) {
		OnLandedInternal();
	}
}

void AStorableItem::SetInstanceClass(UClass* instanceClass) {
	InstanceClass = instanceClass;
}

void AStorableItem::EnableMovementComponentTick(bool val)
{
	if (SimpleMovementComponent) 
		SimpleMovementComponent->SetComponentTickEnabled(val);
}

void AStorableItem::SetTimeToPool(float timeToPool)
{
	if (HasAuthority()) {
		TimeToPool = timeToPool;
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AStorableItem::TimeToPoolFinished, TimeToPool, false);
		OnRep_OnTimeToPoolSet();
	}
}

void AStorableItem::TimeToPoolFinished()
{
	DestroyOrReturnToPool();
}

void AStorableItem::OnRep_OnTimeToPoolSet()
{
	OnTimeToPoolSetBP();
}

void AStorableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AStorableItem, ServerSpawnTimeSeconds);
	DOREPLIFETIME(AStorableItem, ItemData);
	DOREPLIFETIME(AStorableItem, TimeToPool);
}

bool AStorableItem::ShouldPlayDropSound() const {
	if(!HasAuthority()) {
		//Since running out of range of a networked actor causes them to be destroyed, but we want these not to play a sound we add this dodgy heuristic.
		if(auto* gameState = GetWorld()->GetGameState()) {
			const float delta = gameState->GetServerWorldTimeSeconds() - ServerSpawnTimeSeconds;
			return delta < 2.f;
		}
	}
	
	return true;
}

void AStorableItem::HandleInteraction(ACharacter* interactor) {
	if (const auto playerController = Cast<ABasePlayerController>(interactor->GetController())) {
		auto storableComponent = playerController->GetPickupStorableComponent();
		auto rangedComponent = playerController->GetRangedAttackComponent();
		
		if (ThrowableType == EThrowableType::None || rangedComponent->CanAddThrowable(ThrowableType)) {
			if (HasAuthority()) {
				//D11.KS - Don't allow main players to pick up all duped loot
				if (!lockItemToOwner || Cast<AActor>(interactor) == GetOwner()) {
					storableComponent->Store(this);
				}
			}
			else {
				storableComponent->ServerStore(this);
			}
		}
		else {
			OnPickupDenied();
		}
	}
}

void AStorableItem::Tick(float DeltaSeconds) {

	CheckAttachmentCollision();

	++AliveFrames;

	if (DisableTimer > 0.0f && DisableTime_Current > 0.0f) {
		DisableTime_Current -= DeltaSeconds;
		if (DisableTime_Current < 0.0f)
		{
			SetActorTickEnabled(false);
		}
		return;
	}

	if (AliveFrames % 10 == 0) {
		CheckInDestroyingMaterial();	
	}
}

void AStorableItem::CheckAttachmentCollision()
{
	auto world = GetWorld();
	FTraceDatum traceData;

	if (world->QueryTraceData(AttachmentCollisionHandle, traceData))
	{
		if (traceData.OutHits.Num() > 0)
		{
			//single trace so should only be one in here
			const FHitResult& Hit(traceData.OutHits[0]);

			if (Hit.Component.IsValid() || Hit.Actor.IsValid())
			{
				FAttachmentTransformRules Rules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
				if (Hit.Component.IsValid())
				{
					if (Hit.Component.Get()->IsA(URuntimeMeshComponent::StaticClass()))
					{
						//D11.SC - Hit a runtime generated mesh of the world, network/xp[lay has issues replicating these instances across, so dont bother attaching or they will end up lost if UE cant work out its equivilent on the client (its only required for moving UE objects anyway)
						return;
					}
					AttachToComponent(Hit.Component.Get(), Rules);
				}
				else if (Hit.Actor.IsValid())
				{
					AttachToActor(Hit.Actor.Get(), Rules);
				}

				FVector newTotemLocation = Hit.ImpactPoint;
				newTotemLocation.Z += 100;
				SetActorLocation(newTotemLocation);
			}
		}

		AttachmentCollisionHandle = FTraceHandle(); //reset handle
	}
}

void AStorableItem::OnPlayerInVisibleRangeChange(bool isVisible)
{
	Super::OnPlayerInVisibleRangeChange(isVisible);

	//If we arent in range of a player, disable the simple movement component
	if (SimpleMovementComponent && Sphere)
	{
		//if we are in the middle of simulating our fall, dont bother it will get triggered when we land
		if(!Sphere->IsSimulatingPhysics())
		{
			SimpleMovementComponent->SetComponentTickEnabled(isVisible);
		}
	}
}

void AStorableItem::Serialize(FArchive& Ar) {
	ItemIdName = ItemId.GetSerializedId();
	Super::Serialize(Ar);
}

#if WITH_EDITOR
void AStorableItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
	const UProperty* PropertyThatChanged = PropertyChangedEvent.Property;
	if (PropertyThatChanged && PropertyThatChanged->GetFName() == GET_MEMBER_NAME_CHECKED(AStorableItem, ItemId)) {
		ItemIdName = ItemId.GetSerializedId();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AStorableItem::DestroyOrReturnToPool() {
	if (!ReturnToPool()) {
		Destroy();
	}
}

bool AStorableItem::ReturnToPool() {	
	if (AProjectileActorManager::TryPush_Storable(this)) {
		return true;
	}
	return false;
}

void AStorableItem::OnLandedInternal() {
	if (HasValuableType()) {
		if (IsInUnsafeLocation()) {
			TeleportToSafeLocation();
		} else {
			DisablePhysics();			
			OnLanded();
		}
	} else {
		if (IsInUnsafeLocation()) {
			DestroyOrReturnToPool();
		} else {
			DisablePhysics();			
			OnLanded();
		}
	}

	if (!bHidden && !IsPendingKillPending())
	{
		AttachItemToFloor();
	}

	//If we arent in range of a player, disable the simple movement component
	if (SimpleMovementComponent && !mPlayerVisible)
	{
		SimpleMovementComponent->SetComponentTickEnabled(false);
	}

}

void AStorableItem::AttachItemToFloor()
{
	/* Attach this to the floor */

	FCollisionQueryParams CollParams;
	auto world = GetWorld();
	
	FVector StartPos = GetActorLocation();
	FVector EndPos(StartPos.X, StartPos.Y, StartPos.Z - 10000.0f);

	auto& CharList = InstanceTracker< ABaseCharacter >::GetList(world);

	AActor* pOwner = GetOwner();
	if(pOwner)
		CollParams.AddIgnoredActor(pOwner->GetUniqueID());
	
	for (auto* CharVal : CharList)
	{
		CollParams.AddIgnoredActor(CharVal->GetUniqueID());
	}

	AttachmentCollisionHandle = world->AsyncLineTraceByChannel(EAsyncTraceType::Single, StartPos, EndPos, ECollisionChannel::ECC_Pawn, CollParams);
}

bool AStorableItem::HasValuableType() const {
	const auto& itemType = GetItemRegistry().Get(ItemData.GetItemId());
	return itemType.isGear()
		|| itemType.isPermanent()
		|| itemType.isToken()
		|| game::item::type::DiamondDust.getId() == ItemData.GetItemId()
		|| game::item::type::Conduit.getId() == ItemData.GetItemId()
		|| game::item::type::EyeOfEnder.getId() == ItemData.GetItemId();
}

bool AStorableItem::IsInUnsafeLocation() const {
	if (lockItemToOwner) {
		if (const auto* owner = Cast<ABaseCharacter>(GetOwner())) {
			return !owner->IsLocationReachable(GetActorLocation());
		}
	}
	else {
		for (auto player : actorquery::getActors<ABasePlayerController>(GetWorld())) {
			if (ABaseCharacter* playerPawn = Cast<ABaseCharacter>(player->GetPawn())) {
				if (playerPawn->IsLocationReachable(GetActorLocation())) {
					return false;
				}
			}
		}
		return true;
	}
	return false;
}

void AStorableItem::TeleportToSafeLocation() {
	const auto* target = GetTeleportTarget();
	auto* collisionComponent = GetCollisionComponent();
	if (target && collisionComponent) {
		const auto newLocation = storableItem::getLocationAbove(target, collisionComponent->GetScaledSphereRadius());
		collisionComponent->SetAllPhysicsLinearVelocity(FVector::ZeroVector, false);	
		collisionComponent->SetWorldLocation(newLocation, false, nullptr, ETeleportType::ResetPhysics);				
	} else {
		DestroyOrReturnToPool();
	}
}

ABaseCharacter* AStorableItem::GetTeleportTarget() const {
	if (auto* owner = Cast<ABaseCharacter>(GetOwner())) {
		return owner;	
	}
	return actorquery::getFirstActor<APlayerCharacter>(GetWorld());
}

void AStorableItem::DisablePhysics() {
	Sphere->SetSimulatePhysics(false);
#if !PLATFORM_WINDOWS
	// D11.DB - We don't care about this on console but it is required in order
	//			for the player to be able to interact with items with the mouse on PC.
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
#endif
	Sphere->SetEnableGravity(false);
}

void AStorableItem::OnPickup_Implementation() {
	Play(PickupSound);
	if (PickupEffect) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupEffect, GetActorLocation());
	}
	ReceiveOnPickup();
}

void AStorableItem::OnPickupDenied_Implementation() {
	Play(PickupDeniedSound);
	ReceiveOnPickupDenied();
}

void AStorableItem::HandleLockedOwnerDropOut(ULocalPlayer* localPlayer)
{
	if(auto owningCharacter = Cast<APlayerCharacter>(GetOwner()))
	{
		if(owningCharacter->GetNetOwningPlayer() == localPlayer)
		{
			SetActorHiddenInGame(true);
			Destroy();
		}
	}
}

void AStorableItem::CheckInDestroyingMaterial() {
	if (!Game) {
		return;
	}
	auto blockSource = Game->BlockSourceWithLoadedMaterials();
	if (!blockSource) {
		return;
	}
	const auto collisionComponent = GetCollisionComponent();
	const auto sphereBottom = collisionComponent->GetComponentLocation() - FVector(0.f, 0.f, collisionComponent->GetScaledSphereRadius());
	const FVector offset(0.f, 0.f, 20.f);
	const auto overlapPos = conversion::ueToBlock(sphereBottom + offset);

	if (blockSource->getBlockID(overlapPos) != blockSource->getBlockID(PreviousOverlap)) {
		OverlapMaterial = BlockGraphicsHelper::getBlock(GetWorld(), overlapPos, blockSource->getBlockID(overlapPos)).getMaterialType();
		if (OverlapMaterial == EMaterialTypeEnum::Lava
		 || OverlapMaterial == EMaterialTypeEnum::Fire)
		{
			DestroyOrReturnToPool();
		}
		PreviousOverlap = overlapPos;
	}
}

EItemRarity AStorableItem::GetRarity() const {
	return ItemData.Rarity;
}

void AStorableItem::ReInitialiseItem() {
	DisableTime_Current = DisableTimer;
	AttachmentCollisionHandle = FTraceHandle(); //reset handle
	AliveFrames = 0;
	stored = false;
	if(movementComponent())	
		movementComponent()->StopMovementImmediately();
	Sphere->ComponentVelocity = FVector::ZeroVector;
	Sphere->SetSimulatePhysics(true);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Sphere->SetEnableGravity(true);
	ReInitialiseItemBP();
	DetachAllSceneComponents(GetParentComponent(), FDetachmentTransformRules(EDetachmentRule::KeepWorld, false));
}

void AStorableItem::ApplyDropEffect(AActor* fromWhat, const bool doCustomImpulse) const {
	if (ItemDropEffect) {
		ItemDropEffect->Apply(fromWhat, doCustomImpulse);
	}
}

USoundCue* AStorableItem::GetStorableDropSoundForItemId(const FSerializableItemId& itemId) {
	const auto& itemType = GetItemRegistry().Get(itemId);
	if(auto storableClass = itemType.getStorableClass()){
		return storableClass.GetDefaultObject()->DropSound;
	}	
	return nullptr;
}

USoundCue* AStorableItem::GetStorablePickupSoundForItemId(const FSerializableItemId& itemId) {
	const auto& itemType = GetItemRegistry().Get(itemId);
	if (auto storableClass = itemType.getStorableClass()) {
		return storableClass.GetDefaultObject()->PickupSound;
	}
	return nullptr;
}

void AStorableItem::MulticastApplyDropEffect_Implementation(AActor* fromWhat) {
	ApplyDropEffect(fromWhat);
}

void AStorableItem::Play(USoundCue* sound) const {
	if (sound) {
		UGameplayStatics::PlaySoundAtLocation(this, sound, GetActorLocation());
	}
}


const ItemType& AStorableItem::GetItemType() const {
	return GetItemRegistry().Get(ItemId);
}
