#include "ShulkerBullet.h"
#include "game/util/SimpleMovementComponent.h"
#include "game/util/ActorQuery.h"
#include "AbilitySystemComponent.h"
#include "game/util/Pushback.h"
#include "character/BaseCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/FallDamageEffect.h"
#include "PowerStation.h"
#include "game/Enchantments/Levitation.h"
#include "world/entity/MobTags.h"
#include "util/CharacterQuery.h"

AShulkerBullet::AShulkerBullet() {
	PrimaryActorTick.bCanEverTick = true;

	mMovementComponent = CreateDefaultSubobject<USimpleMovementComponent>(TEXT("MovementComponent"));
	mMovementComponent->SetComponentTickEnabled(false);
	mMovementComponent->SetIsReplicated(true);

	mTrackingComponent = CreateDefaultSubobject<UMoveToTargetMovementComponent>(TEXT("MoveToTargetComponent"));
	mTrackingComponent->SetComponentTickEnabled(false);

	mBoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	mBoxCollider->SetGenerateOverlapEvents(true);
	mBoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AShulkerBullet::OnHitboxOverlap);
	mBoxCollider->bEditableWhenInherited = true;

	RootComponent = mBoxCollider;

	SetReplicates(true);
	SetReplicateMovement(true);
	NetUpdateFrequency = 20.0f;
}

void AShulkerBullet::BeginPlay() {
	InstanceTracker<AShulkerBullet>::AddInstance(GetWorld(), this);
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(mTimeoutHandle, this, &AShulkerBullet::Cleanup, mTimeout);
}

void AShulkerBullet::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	InstanceTracker<AShulkerBullet>::RemoveInstance(GetWorld(), this);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AShulkerBullet::Tick(float DeltaSeconds)
{
	SetTarget(GetNearestTarget());
}

void AShulkerBullet::OnSummonPlayed() {
	mBoxCollider->SetGenerateOverlapEvents(true);
}

void AShulkerBullet::SetTarget(AActor* target) {
	if (mTrackingTarget != target)
	{
		mTrackingTarget = target;
		Cast<UMoveToTargetMovementComponent>(mTrackingComponent)->SetTarget(mTrackingTarget);
		mTrackingComponent->SetComponentTickEnabled(true);
	}
}

void AShulkerBullet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AShulkerBullet::Cleanup() {
	if (IsPendingKill()) {
		return;
	}

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Destroy();
}

AActor* AShulkerBullet::GetNearestTarget()
{
	float closestDistanceSquared;
	const auto predicate = [&](const AActor* v) {
		if (auto ps = Cast<APowerStation>(v)) {
			return ps->GetPowerStationState() == EPowerStationState::Inactive;
		}
		return false;
	};
	
	AActor* target = mTrackingTarget;

	float distanceToTargetSqrd = targetPlayerDistance * targetPlayerDistance + 1;
	if (mTrackingTarget)
	{
		distanceToTargetSqrd = (this->GetActorLocation() - mTrackingTarget->GetActorLocation()).SizeSquared();
	}

	if (APowerStation* powerStations = Cast<APowerStation>(actorquery::getClosestActor(this, targetPowerStations, APowerStation::StaticClass(), closestDistanceSquared, false, predicate))) {
		target = powerStations;
	}
	else if (distanceToTargetSqrd > targetPlayerDistance * targetPlayerDistance)
	{
		if (AActor* closestPlayer = actorquery::getClosestPlayer(this, targetPlayerDistance, closestDistanceSquared)) {
			target = closestPlayer;
		}
	}

	return target;
}

void AShulkerBullet::OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (!GetOwner() || GetOwner() == OtherActor || OtherComp->ComponentHasTag(TEXT("Passthrough")))
	{
		return;
	}

	const ABaseCharacter* hitCharacter = Cast<ABaseCharacter>(OtherActor);
	if (!hitCharacter) {
		Cleanup();
		return;
	}

	if (!hitCharacter->IsTargetable() || !hitCharacter->IsAlive()) {
		return; // No collision with pets. Or the dead
	}

	if (auto mob = Cast<AMobCharacter>(OtherActor))
	{
		if (hasMobTag(mob->EntityType, MobTags::HashTag_Miniboss) ||
			hasMobTag(mob->EntityType, MobTags::HashTag_Ancient) ||
			!characterquery::is::movable(mob))
		{
			Cleanup(); //collide with bosses but don't affect them
			return;
		}
	}

	// Create our pushback
	FPushback push;
	push.pushbackStrength = LaunchStrength;
	push.enablePushback = true;

	if (UAbilitySystemComponent* targetAbilitySystem = hitCharacter->FindComponentByClass<UAbilitySystemComponent>()) {

			// Add pushback
			pushback::pushback(push, FVector(0, 0, 0), *hitCharacter);
			
			// Create specs
			FGameplayEffectSpec LeviSpec = effects::CreateGameplayEffectSpec<ULevitationGameplayEffect>(targetAbilitySystem, 1);
			LeviSpec.SetSetByCallerMagnitude(effects::DurationName, levitationDuration);
			LeviSpec.SetSetByCallerMagnitude(ULevitationGameplayEffect::EffectMagnitudeName, -LevitationStrength);
			auto handle = targetAbilitySystem->ApplyGameplayEffectSpecToTarget(LeviSpec, targetAbilitySystem);

			if (handle.WasSuccessfullyApplied())
			{
				FGameplayEffectSpec FallSpec = effects::CreateGameplayEffectSpec<ULevitationFallGameplayEffect>(targetAbilitySystem, 1);
				FallSpec.SetSetByCallerMagnitude(effects::DurationName, fallDuration);
				FallSpec.SetSetByCallerMagnitude(ULevitationGameplayEffect::EffectMagnitudeName, LevitationDropDamagePercentage);
				targetAbilitySystem->ApplyGameplayEffectSpecToTarget(FallSpec, targetAbilitySystem);
			}

			FGameplayEffectSpec ImmunitySpec = effects::CreateGameplayEffectSpec<ULevitationImmunityGameplayEffect>(targetAbilitySystem, 1);
			ImmunitySpec.SetSetByCallerMagnitude(effects::DurationName, levitationDuration*2);
			targetAbilitySystem->ApplyGameplayEffectSpecToTarget(ImmunitySpec, targetAbilitySystem);
	}
	Cleanup();
}
