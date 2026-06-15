#include "ThrowablePropActor.h"
#include "Components/InputComponent.h"
#include "UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "../character/player/BasePlayerController.h"
#include "../character/player/PlayerCharacter.h"
#include "IConsoleManager.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/item/ItemSlot.h"
#include "game/item/ArrowItemSlot.h"
#include "Components/ShapeComponent.h"
#include "game/component/EquipmentComponent.h"
#include "game/component/AutoAimRangedAttackComponent.h"
#include "game/ActorShake.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

AThrowablePropActor::AThrowablePropActor(const class FObjectInitializer& OI) : Super(OI)
{
	PrimaryActorTick.bCanEverTick = true;
	EffectDamage = 200;
	EffectTime = 3.0f;
	SetReplicates(true);
}

void AThrowablePropActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AThrowablePropActor, IsActivated)
	DOREPLIFETIME(AThrowablePropActor, EffectDamage)
	DOREPLIFETIME(AThrowablePropActor, EffectRadius)
	DOREPLIFETIME(AThrowablePropActor, EffectTime)
	DOREPLIFETIME(AThrowablePropActor, EffectTimerHandler)
}

void AThrowablePropActor::ThrowInPlayerForwardDir()
{
	DisableInput(nullptr);

	Throw(GetOwner()->GetActorForwardVector());
}

void AThrowablePropActor::ThrowServer_Implementation(const FVector& dir)
{
	//ThrowServerImplementation(dir);
}

bool AThrowablePropActor::ThrowServer_Validate(const FVector& dir) {
	return true;
}

void AThrowablePropActor::Throw(const FVector& dir)
{
	ThrowServer(dir);

	// D11.DB - Relocated from Activate() as Activate() is never called on clients.
	//D11.KS - Use owning player controller instead.
}

void AThrowablePropActor::OnRep_IsActivated()
{
	// Replicated to client on changing IsActivated
	if (IsActivated) {
		GetRootComponent()->SetVisibility(true, true);
		PendingActivatedUpdate = true;
	}
}

void AThrowablePropActor::UpdateParentDead()
{
	if (IsActivated) {
		return;
	}
	const auto* parent = GetAttachParentActor();
	if (parent == nullptr) {
		Destroy();
		return;
	}
	const auto* hc = parent->FindComponentByClass<UHealthComponent>();
	if (hc != nullptr && hc->IsNotAlive()) {
		Activate(FVector(0, 0, 500));
	}
}

void AThrowablePropActor::Activate(const FVector& dir)
{
	IsActivated = true;
	OnRep_IsActivated();

	SetReplicateMovement(true);

	DisableInput(nullptr);

	RemoveThrowable();
}

void AThrowablePropActor::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() == nullptr) {
		Destroy();
		return;
	}

	auto playerOwner = Cast<APlayerCharacter>(GetOwner());
	playerOwner->OnDeath.AddUObject(this, &AThrowablePropActor::Fallen);
	playerOwner->OnPlayerDown.AddUObject(this, &AThrowablePropActor::Fallen);
	playerOwner->OnDestroyed.AddDynamic(this, &AThrowablePropActor::OnOwnerDestroyed);

	APlayerController* owningPlayerController = playerOwner->GetPlayerController();

	if (owningPlayerController == nullptr)
	{
		Destroy();
		return;
	}

	if (HasAuthority()) {
		UShapeComponent* collisionMesh = FindComponentByClass<UBoxComponent>();
		if (!collisionMesh) {
			collisionMesh = FindComponentByClass<USphereComponent>();
		}

		this->NetUpdateFrequency = 60.0f;
		this->MinNetUpdateFrequency = 25.0f;
		this->NetPriority = 5;
		if (AffectedByGravity) {
			collisionMesh->SetSimulatePhysics(true);
			collisionMesh->SetEnableGravity(true);
		}
		collisionMesh->SetMobility(EComponentMobility::Movable);
		collisionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SetRootComponent(collisionMesh);

		if (const auto* owner = GetOwner()) {
			auto equipmentComponent = owner->FindComponentByClass<UEquipmentComponent>();
			auto ArrowSlots = equipmentComponent->GetSlotsOfType(SlotType);
			if (ArrowSlots.IsValidIndex(0) && ArrowSlots[0]->IsA<UArrowItemSlot>()) {
				Slot = Cast<UArrowItemSlot>(ArrowSlots[0]);
				Slot->EquipFixedAmount(FInventoryItemData(game::item::type::Arrow.getId(), 1), 1);
			}
		}
	}

	if (!GetWorld()->IsServer()) {
		EnableInput(owningPlayerController);
	}

	if(playerOwner->GetPlayerController())
	{
		OwnerRangedAttackComponent = playerOwner->GetPlayerController()->GetRangedAttackComponent();
	}

	if (!DenialShake) {
		DenialShake = UActorShake::StaticClass();
	}
}

void AThrowablePropActor::Tick(float DeltaSeconds)
{
	if (PendingActivatedUpdate) {
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		OnActivated();
		PendingActivatedUpdate = false;
	}
}

void AThrowablePropActor::OnOwnerDestroyed(AActor* destroyedActor) {
	OnFallen();
}

void AThrowablePropActor::Fallen()
{
	OnFallen();
}

void AThrowablePropActor::OnFallen()
{
	if (HasAuthority()) {
		DisableInput(nullptr);
		if (Slot)
		{
			Slot->Consume();
		}
	}
	Activate(FVector(0, 0, 0));
}

FVector AThrowablePropActor::GetMouseClickedWorldPosition(ABasePlayerController* controller) const
{
	FVector2D mousePosition;

	FHitResult hitResult;
	const bool hitSuccess = controller->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType((ECollisionChannel)ECustomTraceChannels::PlayerPlane), false, hitResult);

	return hitSuccess ? hitResult.ImpactPoint : FVector::ZeroVector;
}

bool AThrowablePropActor::IsFirstInStack() const
{
	if (GetOwner() == nullptr) {
		return true;
	}

	if (OwnerRangedAttackComponent)
	{
		const auto& heldThrowables = OwnerRangedAttackComponent->GetHeldThrowables();
		return heldThrowables.Num() > 0 ? heldThrowables[heldThrowables.Num() - 1] == this : false;
	}

	return false;
}

uint16 AThrowablePropActor::DealDamageWithinRadius(const int radius, AActor* owner)
{
	const auto& tridentLocation = GetActorLocation();

	uint16 count = 0;
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);
	for (auto actor : actorquery::getNearbyActors<ABaseCharacter>(this, radius)) {
		if (const auto mob = Cast<AMobCharacter>(actor)) {
			if (mob->IsAlive() && mob->IsTargetable()) {
				DealDamageToMob(mob, tridentLocation, owner, EffectDamage);
				if (mob->IsNotAlive()) {
					count++;
				}
			}
		}
		else if (const auto player = Cast<APlayerCharacter>(actor)) {
			DealDamageToPlayer(player, owner, EffectDamage);
		}
	}
	return count;
}

void AThrowablePropActor::Explode()
{
	if (CVarDebugDrawItems.GetValueOnGameThread()) {
		DrawDebugSphere(GetWorld(), GetActorLocation(), EffectRadius, 50, FColor::Green, false, 5.0f);
	}

	check(HasAuthority());

	const auto owner = GetOwner();
	ExecuteEffectGameplayCue(owner);
	DealDamageWithinRadius(EffectRadius, owner);
	OnEffectExecuted();

	Destroy();
}

void AThrowablePropActor::Emplace()
{
	if (CVarDebugDrawItems.GetValueOnGameThread()) {
		DrawDebugSphere(GetWorld(), GetActorLocation(), EffectRadius, 50, FColor::Green, false, 5.0f);
	}

	check(HasAuthority());

	const auto owner = GetOwner();
	ExecuteEffectGameplayCue(owner);
	OnEffectExecuted();
}

void AThrowablePropActor::ExecuteEffectGameplayCue(const AActor* owner) {
	auto abilitySystem = owner->FindComponentByClass<UAbilitySystemComponent>();

	abilitySystem->ExecuteGameplayCue(EffectTag, GetEffectGameplayCue(owner));
}

FGameplayCueParameters AThrowablePropActor::GetEffectGameplayCue(const AActor* owner)
{
	FGameplayCueParameters params;
	params.AbilityLevel = 1.f;
	params.NormalizedMagnitude = 1.f;
	params.Instigator = owner;
	params.EffectCauser = this;
	params.Location = GetActorLocation();

	return params;
}

void AThrowablePropActor::DealDamageToMob(const AMobCharacter* mob, const FVector& ItemLocation, AActor* owner, const float scaledDamage)
{
	if (mob->IsAlive()) {
		const auto push = CreatePushback(mob, ItemLocation);
		pushback::pushback(push, *this, *mob);

		const auto mobAbilitySystem = mob->GetAbilitySystemComponent();

		auto mobDamageSpec = GetGameplayEffectSpec(mobAbilitySystem, owner, scaledDamage); //effects::CreateGameplayEffectSpec<UTridentDamageGameplayEffect>(mobAbilitySystem, effects::HealthName, -scaledDamage, owner, this, owner->GetActorLocation(), 1.f);
		//Set any pushback to be applied to ragdolls (modified with a bonus amount and extra strength)
		effects::StorePushbackInNormal(mobDamageSpec, pushback::getLaunchVector(push, *this, *mob, 1.5f, 1.0f));
		mobAbilitySystem->ApplyGameplayEffectSpecToSelf(mobDamageSpec);
	}
}

FPushback AThrowablePropActor::CreatePushback(const AActor* entity, const FVector& ItemLocation) const
{
	const auto launchMagnitude = CalculateLaunchMagnitude(entity, ItemLocation);

	FPushback push;
	push.pushbackStrength = launchMagnitude * BasePushbackMagnitude;
	push.pushbackStrength = FMath::Max(push.pushbackStrength, 2.0f);
	push.pushbackZFactor = 2.0f;
	push.enablePushback = true;

	return push;
}

float AThrowablePropActor::CalculateLaunchMagnitude(const AActor* entity, const FVector& ItemLocation)
{
	const auto distToEffectCenter = entity->GetActorLocation() - ItemLocation;
	auto launchMagnitude = distToEffectCenter.Size() / 900.0f;
	launchMagnitude = 1.0f - launchMagnitude;
	return launchMagnitude;
}

void AThrowablePropActor::DealDamageToPlayer(const APlayerCharacter* player, AActor* owner, const float scaledDamage)
{
	if (player->IsAlive()) {
		const auto playerAbilitySystem = player->GetAbilitySystemComponent();

		auto playerDamageSpec = GetGameplayEffectSpec(playerAbilitySystem, owner, scaledDamage);
		effects::StorePushbackInNormal(playerDamageSpec, pushback::getLaunchVector(CreatePushback(player, this->GetActorLocation()), *this, *player, 1.5f, 1.0f));
		playerAbilitySystem->ApplyGameplayEffectSpecToSelf(playerDamageSpec);
	}
}

void AThrowablePropActor::RemoveThrowable() {
	if (OwnerRangedAttackComponent)
	{
		OwnerRangedAttackComponent->RemoveThrowable(this);
	}
}

void AThrowablePropActor::Consume() {
	if (Slot) {
		Slot->Consume();
	}
}