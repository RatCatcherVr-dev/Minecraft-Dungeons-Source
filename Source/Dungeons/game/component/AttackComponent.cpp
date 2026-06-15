#include "Dungeons.h"
#include "AttackComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "game/component/PlayerCharacterMovementComponent.h"

UAttackComponent::UAttackComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	canAttackFromSecond = 0;
}

void UAttackComponent::BeginPlay() {
	Super::BeginPlay();
	cachedOwner = Cast<ABaseCharacter>(GetOwner());
	check(cachedOwner && "Attack components must be owned by base characters!");

	if (GetOwnerRole() == ROLE_Authority) {
		if (auto healthComponent = GetOwner()->FindComponentByClass<UHealthComponent>()) {
			healthComponent->OnDeath.AddUObject(this, &UAttackComponent::OnDeath);
		}
	}
}

void UAttackComponent::OnDeath() {
	Stop();
}

FPredictionKey UAttackComponent::AttackLocal(AActor* attackTarget) {
	if (!CanAttack(attackTarget)) return FPredictionKey();
	
	UAbilitySystemComponent* abilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	check(abilitySystem && "Attack Component owned by character without AbilitySystemComponent.");


	uint32 seed = static_cast<uint32>(FMath::Rand());

	if (!GetOwner()->HasAuthority()) {
		FSharedPredictionContext context = FSharedPredictionContext::WithNewKey(abilitySystem, outstandingPrediction);
		outstandingPrediction = context.GetKey();
		outstandingPrediction.NewRejectOrCaughtUpDelegate(FPredictionKeyEvent::CreateUObject(this, &UAttackComponent::PredictionKeyRejectedOrCaughtUpCallback, outstandingPrediction.Current));
		AttackServer(attackTarget, seed, outstandingPrediction);

		FUseSpecfiedKeyScopedPredictionWindow window(context);
		AttackCpp(attackTarget, seed, context);
	}
	else {
		AttackCpp(attackTarget, seed);
	}
	return outstandingPrediction;
}

void UAttackComponent::AttackServer_Implementation(AActor* attackTarget, int32 seed /*= 0*/, FPredictionKey key /*= FPredictionKey()*/) {
	UAbilitySystemComponent* abilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	check(abilitySystem && "Attack Component owned by character without AbilitySystemComponent.");
	FSharedPredictionContext context(abilitySystem, key);
	FUseSpecfiedKeyScopedPredictionWindow window(context);
	AttackCpp(attackTarget, seed, context);
}

bool UAttackComponent::AttackServer_Validate(AActor* attackTarget, int32 seed /*= 0*/, FPredictionKey key /*= FPredictionKey()*/) {
	return true;
}

float UAttackComponent::GetAttackRange() const {
	return 0.f;
}

float UAttackComponent::GetAttackRangeSquared() const
{
	const float fRange = GetAttackRange();
	return fRange * fRange;
}

bool UAttackComponent::InAttackOrientation(AActor* attackTarget) const {
	return true;
}

float UAttackComponent::LastAttackTimestampSeconds() const {
	return lastAttackTimestampSeconds;
}

bool UAttackComponent::IsAttackInProgress() const {
	return false;
}

float UAttackComponent::CanAttackFromTimestampSeconds() const {
	return std::max(canAttackFromSecond, cachedOwner->GetCanActFromSecondActive());;
}

float UAttackComponent::GetSecondsUntilNextPossibleAttack() const {
	float worldTime = GetWorld()->GetTimeSeconds();
	float attackTimeStamp = CanAttackFromTimestampSeconds();
	return FMath::Max(0.0f, attackTimeStamp - worldTime);
}

void UAttackComponent::AttackCpp(AActor* attackTarget, int32 seed /*= 0*/, FSharedPredictionContext predictionContext) {
	float worldTime = GetWorld()->GetTimeSeconds();
	
	lastAttackTimestampSeconds = worldTime;
	canAttackFromSecond = attackRateSeconds + worldTime;

	check(cachedOwner && "Cached owner really should have been set by now!");
	cachedOwner->SetSharedCooldown(attackRateSeconds * activeCancelPoint, attackRateSeconds * passiveCancelPoint);
}

bool UAttackComponent::CanAttack(AActor* attackTarget) const {
	float worldTime = GetWorld()->GetTimeSeconds();

	check(cachedOwner && "Cached owner really should have been set by now!");

	bool isFrozen = false;
	bool isUsingElytra = false;
	if (auto player = Cast<APlayerCharacter>(cachedOwner)) {
		isFrozen = player->IsFrozenSolid();

		if (auto movementComp = player->GetPlayerCharacterMovementComponent())
		{
			isUsingElytra = movementComp->IsMovingWithElytra();
		}
	}

	return !isFrozen && !isUsingElytra && canAttackFromSecond <= worldTime && cachedOwner->GetCanActFromSecondActive() <= worldTime;
}

float UAttackComponent::GetCoolDownAsScalarValue() const {
	float secondsRemaining = canAttackFromSecond - GetWorld()->GetTimeSeconds();
	secondsRemaining = FMath::Clamp(secondsRemaining, 0.f, attackRateSeconds);
	return 1.0f - (secondsRemaining / attackRateSeconds);
}

void UAttackComponent::PredictionKeyRejectedOrCaughtUpCallback(FPredictionKey::KeyType key) {
	if (outstandingPrediction.Current == key) {
		outstandingPrediction = FPredictionKey();
	}
}

bool UAttackComponent::BeginAttack(const FAttackAimProvider& targetProvider) {
	if (!pendingAttackHandle.IsValid()) {
		AttackWrapper(targetProvider);
	}

	return IsAttacking();
}

void UAttackComponent::EndAttack(AActor* attackTarget) {
	if (pendingAttackHandle.IsValid()) {
		UWorld* world = GetOwner()->GetWorld();
		check(world && "World must be valid");
		FTimerManager& timerManager = world->GetTimerManager();
		timerManager.ClearTimer(pendingAttackHandle);
		pendingAttackHandle.Invalidate();
	}
}

bool UAttackComponent::IsAttacking() const {
	return pendingAttackHandle.IsValid();
}

void UAttackComponent::Stop()
{
	if (IsInGameThread())
	{
		StopAttack();
	}
	else
	{
		//Push the stop attack to the task graph to be executed from the game thread later
		TWeakObjectPtr<UAttackComponent> WeakThisComponent = this;
		FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThisComponent]()
		{
			if (WeakThisComponent.IsValid())
			{
				WeakThisComponent->StopAttack();
			}
		}, TStatId(), nullptr, ENamedThreads::GameThread);
	}
}

void UAttackComponent::CancelCooldown() {
	canAttackFromSecond = GetWorld()->GetTimeSeconds();
}

void UAttackComponent::AttackWrapper(FAttackAimProvider targetProvider) {
	UWorld* world = GetOwner()->GetWorld();
	check(world && "World must be valid");
	FTimerManager& timerManager = world->GetTimerManager();
	float now = world->GetTimeSeconds();

	check(targetProvider.IsBound() && "Must provide valid delegate!");
	auto targetData = targetProvider.Execute();

	AttackLocal(targetData.target);

	auto attackTime = CanAttackFromTimestampSeconds();

	if (attackTime > now) {
		timerManager.SetTimer(pendingAttackHandle, FTimerDelegate::CreateUObject(this, &UAttackComponent::AttackWrapper, std::move(targetProvider)), attackTime - now, false);
	}
}