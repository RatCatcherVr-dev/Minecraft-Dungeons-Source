#include "Dungeons.h"

#include "EquipmentComponent.h"
#include "WalkPickupComponent.h"
#include "UnrealNetwork.h"
#include "game/component/drop/ItemDropComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/component/PickupStorableComponent.h"
#include "game/util/SimpleMovementComponent.h"
#include "game/actor/character/player/BasePlayerState.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/WorldDamageGameplayEffect.h"
#include "game/affector/Affectors.h"



namespace walkPickup
{
int getStackCount(APlayerCharacter* player, const ItemType& itemType) {
	int count = 0;
	// item count for current player
	const auto slotType = itemType.slotType();
	UEquipmentComponent* equipmentComponent = player->GetEquipmentComponent();
	if (slotType != ESlotType::None) {
		TArray<UItemSlot*> slots = equipmentComponent->GetSlotsOfType(slotType);
		if (slots.Num()) {
			UItemSlot* slot = slots[0];
			AItemInstance* item = slot->GetItem();
			if (item)
				count = slot->GetCount();
		}
	}
	return count;
}

void applyEmeraldHealthEffect(AActor* emerald, APlayerCharacter* player, float healthAdd) {
	ensure(healthAdd > 0 && "EmeraldHealthAdd currently only supports > 0");

	if (healthAdd > 0) {
		player->GetHealthComponent()->ServerApplyHeal(healthAdd);
	}
}

}

UWalkPickupComponent::UWalkPickupComponent()
	: m_state(walkPickup::Idle)
	, m_waitingTime(0.0f)
	, m_pickedUp(false)
	, m_moveTime(0.0f)
	, m_Acceleration(0.0f)
	, m_t(0.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
	bReplicates = true;
}

FString UWalkPickupComponent::StateString(walkPickup::State state)
{	
	using namespace walkPickup;
	switch (state)
	{
	case Idle:
		return FString("Idle");
	case Dropping:
		return FString("Dropping");
	case Waiting:
		return FString("Waiting");
	case Moving:
		return FString("Moving");

	default:
		return FString("");
	}
}

void UWalkPickupComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWalkPickupComponent, m_overlapping);
	DOREPLIFETIME(UWalkPickupComponent, ServerComponentVelocity);
	DOREPLIFETIME(UWalkPickupComponent, ServerComponentSpeed);
	DOREPLIFETIME(UWalkPickupComponent, ClosestTargetDistanceSq);
}

void UWalkPickupComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	ensure(SimpleMovementComponent);
	if (!SimpleMovementComponent)
		return;

	if (m_pickedUp)
	{
		return;
	}

	//if our owner is disabled, dont bother doing all of the checks for pull in
	if (mParentStorable.IsValid())
	{
		if (!mParentStorable->IsActorTickEnabled())
		{
			return;
		}
	}

	// wait for spawn animation (m_state == dropping) 
	m_waitingTime += DeltaTime;
	if (!FinishedWaiting())
		return;

	if (m_state == walkPickup::Waiting)
		TriggerPullInActor();
	else if(m_state == walkPickup::Dropping)
		SetState(walkPickup::Idle);

	AActor* pMyOwner = this->GetOwner();
	TArray< APlayerCharacter* > OutPlayerList;

	//D11.KS - In local coop mode, emeralds should fly to you no matter what, this value is large enough to fill this case.
	float pullDistance = (pMyOwner->GetWorld()->GetGameInstance()->GetNumLocalPlayers() > 1) ? 10000.0f : PullInThreshold;

	UActorQuery::GetPlayerCharactersInRangePred(pMyOwner->GetWorld(), pMyOwner->GetActorLocation(), pullDistance, OutPlayerList, true, [&](APlayerCharacter* pActor) {return IsAllowedToPickUp(pActor); });

	if (m_state < walkPickup::Waiting && OutPlayerList.Num())
		SetState(walkPickup::Waiting); 

	// handle the pick up directly-case
	if (!PullInItem && m_state > walkPickup::Waiting)
	{
		if (OutPlayerList.Num() > 0)
		{
			TryPickupStorable(OutPlayerList[0]);
		}
		
	}
	else if (OutPlayerList.Num() > 0)
	{
		m_overlapping = true;

		m_moveTime += DeltaTime;
		m_t += DeltaTime;

		if (UseForceCalculation && PullInMode != EPullInMode::AccelerateWithTime)
		{
			APlayerCharacter* winner = nullptr;
			FVector vel = GetDeltaX(OutPlayerList, winner);
			if (GetRoot())
			{
				GetRoot()->AddForce(100 * vel, NAME_None, true);
			}

			if (winner)
			{
				TryPickupStorable(winner);
			}
			
		}
		else 
		{
			// update 'server' position if authoriry
			if (GetOwner() && GetOwner()->HasAuthority())
			{
				FVector vel = SimpleMovementComponent->Velocity;

				APlayerCharacter* winner = nullptr;
				vel = GetDeltaX(OutPlayerList, winner);

				// replicates
				ServerComponentSpeed = GetSpeed(DeltaTime);

				SimpleMovementComponent->Velocity = vel * ServerComponentSpeed;

				// replicates
				ServerComponentVelocity = vel;

				if (winner)
				{
					TryPickupStorable(winner);
				}
				
			}
			else if (GetOwner())
			{
				FVector vel = SimpleMovementComponent->Velocity;

				vel = ServerComponentVelocity;
				float speed = GetSpeed(DeltaTime);

				SimpleMovementComponent->Velocity = vel * speed;
			}
		}
				
	}

}

void UWalkPickupComponent::ReplicateState(bool old)
{
	if(!m_overlapping) // && old != m_overlapping
		SimpleMovementComponent->Velocity = FVector::ZeroVector;
}

void UWalkPickupComponent::OnRep_ServerComponentVelocity(FVector old)
{
	LastClientVelocity = ClientVelocity;
}

void UWalkPickupComponent::ResetPickup()
{
	m_overlapping = true;
	m_pickedUp = false;
	m_state = walkPickup::Idle;
	m_waitingTime = 0.0f;
	m_moveTime = 0.0f;
	m_Acceleration = 0.0f;
	m_t = 0.0f;

	// run on server only
	if (!GetOwner()->HasAuthority())
		return;

	// count down until enabled
	SetState(walkPickup::Dropping);

}

void UWalkPickupComponent::OnRep_ServerComponentSpeed(float old)
{
	// calculate client side server 'acceleration'
	if (m_t > 0 && ServerComponentSpeed != PullInSpeed)
		m_Acceleration = (ServerComponentSpeed - old) / m_t;
	else
		m_Acceleration = 0;
	UE_LOG(LogDungeons, Verbose, TEXT("a (%f) = SpeedDelta: (%f-%f)/%f"), m_Acceleration, ServerComponentSpeed, old, m_t);
	m_t = 0.0f;

}

// Called when the game starts
void UWalkPickupComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* pMyOwner = this->GetOwner();

	if (auto storable = Cast<AStorableItem>(pMyOwner))
	{
		mParentStorable = storable;
		SimpleMovementComponent = storable->movementComponent();
	}
		
	if (SimpleMovementComponent)
	{
		SimpleMovementComponent->SetUpdatedComponent(pMyOwner->GetRootComponent());
		SimpleMovementComponent->SetWaitTime(EnablePullDelay);
	}
#if WITH_EDITOR
	else
		UE_LOG(LogDungeons, Log, TEXT("No MovementComponent defined on WalkPickupComponent parent: %s cannot move item!"), *GetOwner()->GetActorLabel());
#endif
	// run on server only
	if (!pMyOwner->HasAuthority())
		return;

	// count down until enabled
	SetState(walkPickup::Dropping);
}

void UWalkPickupComponent::EndPlay(const EEndPlayReason::Type type) {
	Super::EndPlay(type);
}

void UWalkPickupComponent::NetSetPhysicsEnabled_Implementation(bool enabled)
{
	auto root = GetRoot();
	ECollisionEnabled::Type collision = enabled ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
	root->SetEnableGravity(enabled);
	root->SetSimulatePhysics(enabled);
	root->SetCollisionEnabled(collision);
}


bool UWalkPickupComponent::SetState(walkPickup::State state)
{
	if (state == m_state)
		return true;

	bool state_in_sequence = (int)state == (m_state + 1) % walkPickup::Count;

	m_state = state; // enforce state_in_sequence?

	// update + replicate if authority
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		m_overlapping = m_state == walkPickup::Moving && FinishedWaiting();
	}

	return state_in_sequence;
}


void UWalkPickupComponent::TriggerPullInActor()
{
	SetState(walkPickup::Moving);
	m_moveTime = 0.0f;

	UE_LOG(LogDungeons, Verbose, TEXT("Triggering pull! %s (waiting %d)"), *GetNameSafe(GetOwner()), !FinishedWaiting());

	if (!UseForceCalculation)
	{
		NetSetPhysicsEnabled(false);
	}
}


UPrimitiveComponent* UWalkPickupComponent::GetRoot()
{
	return Cast<UPrimitiveComponent>(this->GetOwner()->GetRootComponent());
}

float UWalkPickupComponent::GetSpeed(float dt)
{
	float x = 0.0f;
	switch (PullInMode)
	{
	case EPullInMode::AccelerateWithDistance:
		// need some sort of acceleration for client side speed computation!!!
	{
		float dRat = ClosestTargetDistanceSq / FMath::Pow((1.1 / PullInThreshold),2.0f);
		x = FMath::Clamp((1 - dRat), 0.0f, 1.0f) * HALF_PI;
		break;
	}
	case EPullInMode::AccelerateWithTime:
		x = FMath::Clamp(m_moveTime * PullAcceleration, 0.01f, HALF_PI);
		break;
	}

	float speed = PullInSpeed * (1 - cos(x));

	// compensate for out of synch client speed
	if (!GetOwner()->HasAuthority())
	{
		speed += m_Acceleration * m_t;
	}

	return speed;
}

FVector UWalkPickupComponent::GetDeltaX(TArray< APlayerCharacter* >& PlayerList , APlayerCharacter*& out_pickupPlayer)
{
	
	ClosestTargetDistanceSq = FMath::Pow( 1.1 * PullInThreshold, 2.0f );
		
	APlayerCharacter* closestTarget = nullptr;
	FVector pullDirection(FVector::ZeroVector);
	
	if (PlayerList.Num())
	{
		const FVector MyPos = GetOwner()->GetActorLocation();

		for (auto player : PlayerList)
		{
			if (player->GetDungeonsBasePlayerState() == nullptr)
			{
				continue;
			}

			if (player->IsNotAlive())
			{
				continue;
			}

			const FVector delta = player->GetActorLocation() - MyPos;
			const float fDeltaSizeSq = delta.SizeSquared();
			if (fDeltaSizeSq > 0)
			{
				// also remove player from pool..
				pullDirection += (delta / fDeltaSizeSq);
			}

			if (fDeltaSizeSq < ClosestTargetDistanceSq)
			{
				ClosestTargetDistanceSq = fDeltaSizeSq;
				closestTarget = player;
			}

		}
	}
		
	
	if (ClosestTargetDistanceSq < FMath::Pow(CollisionDistance, 2.0f))
		out_pickupPlayer = closestTarget;

	FVector dv = pullDirection.GetSafeNormal();
	return dv;
}

void UWalkPickupComponent::TryPickupStorable(APlayerCharacter* Player)
{
	if (m_pickedUp)
		return;

	if (!Player)
		return;
	
	if (!AllowPickupWhenDead && Player->IsNotAlive())
		return;

	if (mParentStorable.Get())
	{
		//D11.KS - Don't collect all local duped emeralds.
		if (mParentStorable->lockItemToOwner && mParentStorable->GetOwner() != Cast<AActor>(Player))
			return;

		auto* Controller = Cast<ABasePlayerController>(Player->GetController());
		if (Controller && Controller->IsLocalController())
		{
			if (auto healthAdd = affector::get(GetWorld()).GetEmeraldHealthAdd())
			{
				walkPickup::applyEmeraldHealthEffect(this->GetOwner(), Player, healthAdd.GetValue());
			} 
			m_pickedUp = Controller->GetPickupStorableComponent()->Store(mParentStorable.Get());
		}
		else
		{
			mParentStorable->DestroyOrReturnToPool();
		}
	}
}

bool UWalkPickupComponent::IsAllowedToPickUp(APlayerCharacter* player) const
{
	const auto item = Cast<AStorableItem>(GetOwner());
	if (item && item->ItemData.GetItemType().hasTag(ItemTag::Currency)) {
		const auto itemOwner = item->GetOwner();
		return !itemOwner || itemOwner && itemOwner == player;
	}
	return true;
}
