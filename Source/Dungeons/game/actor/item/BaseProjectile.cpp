#include "Dungeons.h"
#include "DungeonsGameMode.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/LifestealExecution.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/component/ArrowVobbleComponent.h"
#include "game/component/HealthComponent.h"
#include "game/component/RangedAttackComponent.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/actor/item/StorableItem.h"
#include "game/actor/item/Soul.h"
#include "game/item/ItemTypeDefs.h"
#include "game/util/SimpleMovementComponent.h"
#include "lovika/LovikaLevelActor.h"
#include <AbilitySystemComponent.h>
#include <GameplayTagContainer.h>
#include <Kismet/KismetMathLibrary.h>
#include <UnrealNetwork.h>
#include <UObjectBaseUtility.h>
#include "game/component/EquipmentComponent.h"
#include "BaseProjectile.h"
#include "game/Enchantments/Dynamo.h"
#include "game/abilities/effects/executions/DynamoRangedCueExecution.h"
#include "game/ArmorProperties/ArmorPropertiesComponent.h"
#include "game/util/DungeonsGearUtilLibrary.h"
#include "game/item/ArrowItemSlot.h"
#include "game/actor/ImpactActionHandler.h"
#include "game/actor/ProjectileManager.h"
#include "util/ProjectileFunctionLibrary.h"
#include "AbilitySystemGlobals.h"
#include "game/component/OxygenComponent.h"


DEFINE_LOG_CATEGORY(LogDungeonsProjectile)

void ABaseProjectileProp::DelayedExplosion(float delay, float radius, UAbilitySystemComponent * instigatorAbilitySystem, const TOptional<FGameplayEffectSpec>& spec, const FPushback & pushback, FGameplayTag cue) {
	OnBeginPulse(delay);
	LifeTime = delay + 0.1f;

	FTimerHandle handle;
	if (spec && instigatorAbilitySystem->IsOwnerActorAuthoritative()) {
		GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &ABaseProjectileProp::Explode, radius, spec.GetValue(), TWeakObjectPtr<UAbilitySystemComponent>(instigatorAbilitySystem), pushback, cue), delay, false);
	} else {
		GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(this, &ABaseProjectileProp::OnExplode), delay, false);
	}

}

void ABaseProjectileProp::ExplodeNative(float radius, FGameplayEffectSpec spec, TWeakObjectPtr<UAbilitySystemComponent> instigatorAbilitySystem, const FPushback& pushback, FGameplayTag cue)
{
	impactaction::ExplodeProjectile(instigatorAbilitySystem.Get(), spec, radius, cue, this, pushback);
}

void ABaseProjectileProp::Explode(float radius, FGameplayEffectSpec spec, TWeakObjectPtr<UAbilitySystemComponent> instigatorAbilitySystem, FPushback pushback, FGameplayTag cue) {
	OnExplode();
	ExplodeNative(radius,  spec, instigatorAbilitySystem, pushback, cue);
}

void ABaseProjectileProp::BeginPlay()
{
	Super::BeginPlay();


	auto* pRootComponent = GetRootComponent();
	if (pRootComponent)
	{
		pRootComponent->SetCanEverAffectNavigation(false);
	}
}

void ABaseProjectileProp::Tick(float DeltaSecs) {
	Super::Tick(DeltaSecs);

	if (LifeTime > 0.0f)
	{
		LifeTime -= DeltaSecs;

		if (!GetAttachParentActor() || LifeTime <= 0.0f)
		{
			//despawn
			AProjectileActorManager::Push_ProjectileProp(this);
		}

	}
}

ABaseProjectile::ABaseProjectile(const FObjectInitializer& ObjectInitializer)
:
Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMovementComponent = CreateDefaultSubobject<UDungeonsProjectileMovementComp>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;
	ProjectileMovementComponent->InitialSpeed = 3000.f;
	ProjectileMovementComponent->MaxSpeed = 3000.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->UpdatedComponent = GetRootComponent();	
	ProjectileMovementComponent->SetCanEverAffectNavigation(false);
	if (ProjectileMovementComponent->UpdatedComponent)
	{
		ProjectileMovementComponent->UpdatedComponent->SetCanEverAffectNavigation(false);
	}

	ImpactActionHandler = CreateDefaultSubobject<UImpactActionHandler>(TEXT("ImpactActionHandler"));
}


void ABaseProjectile::BeginPlay() {
	Super::BeginPlay();

	auto* pRootComponent = GetRootComponent();
	if (pRootComponent)
	{
		pRootComponent->SetCanEverAffectNavigation(false);
	}

	spawnLocation = GetActorLocation();

	for (auto a : GetComponentsByClass(UShapeComponent::StaticClass())) {
		if (UShapeComponent* collisionMesh = Cast<UShapeComponent>(a)) {
			collisionMesh->OnComponentBeginOverlap.AddDynamic(this, &ABaseProjectile::OnOverlapBegin);
			collisionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	OnActorEndOverlap.AddDynamic(this, &ABaseProjectile::OnOverlapEnd);
	
	//Store the Default values
	mDefaultValues.pushback = pushback;
	mDefaultValues.DamageType = DamageType;
	mDefaultValues.IsHoming = IsHoming;
	mDefaultValues.DestroyOnHit = DestroyOnHit;
	mDefaultValues.bShouldHitTerrain = bShouldHitTerrain;
	mDefaultValues.AffectTargetOnce = AffectTargetOnce;
	mDefaultValues.IgnoreFriendlyFire = IgnoreFriendlyFire;
	mDefaultValues.enableDebugVisuals = enableDebugVisuals;
	mDefaultValues.CollisionPhysicsForce = CollisionPhysicsForce;
	mDefaultValues.TimeThreshold = TimeThreshold;
	mDefaultValues.HomingThreshold = HomingThreshold;
	mDefaultValues.VelocityFactor = VelocityFactor;
	mDefaultValues.MaxProjectileLifeTime = MaxProjectileLifeTime;
	mDefaultValues.MaxDist5Degrees = MaxDist5Degrees;
	mDefaultValues.MaxDist40Degrees = MaxDist40Degrees;
	mDefaultValues.damage = damage;
	mDefaultValues.stunMultiplier = stunMultiplier;
	mDefaultValues.pushbackMultiplier = pushbackMultiplier;
	mDefaultValues.WeaponItemPower = WeaponItemPower;

	mDefaultValues.ProjectileMovementComponent_bRotationFollowsVelocity = ProjectileMovementComponent->bRotationFollowsVelocity;
	mDefaultValues.ProjectileMovementComponent_bShouldBounce = ProjectileMovementComponent->bShouldBounce;
	mDefaultValues.ProjectileMovementComponent_InitialSpeed = ProjectileMovementComponent->InitialSpeed;
	mDefaultValues.ProjectileMovementComponent_MaxSpeed = ProjectileMovementComponent->MaxSpeed;
	mDefaultValues.ProjectileMovementComponent_ProjectileGravityScale = ProjectileMovementComponent->ProjectileGravityScale;

}

void ABaseProjectile::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	CurrentProjectileLifetime += DeltaTime;

	if (expired || (MaxProjectileLifeTime > 0.0f && CurrentProjectileLifetime > MaxProjectileLifeTime))
	{
		expired = true;
		AProjectileActorManager::Push_Projectile(this);
		return;
	}

	if (
		FVector::DistSquared2D(GetActorLocation(), spawnLocation) > FMath::Square(ForcedExpireDistance)
	) {		
		expired = true;
		AProjectileActorManager::Push_Projectile(this);
		return;
	}

	if (IsHoming && Target.IsValid() && !ProjectileMovementComponent->IsSinking()) {
		auto targetActor = Target.Get();

		if(!actorquery::is::alive(targetActor))
		{
			//stop!, stop!, hes dead already!
			IsHoming = false;
			Target.Reset();
			return;
		}
		else if (auto* pCharTarget = Cast<ABaseCharacter>(targetActor))
		{
			if (pCharTarget->GetWorldState() == ECharacterWorldState::Disappeared)
			{
				//and like that -poof- he's gone
				IsHoming = false;
				Target.Reset();
				return;
			}
		}

		FVector targetLocation = targetActor->GetTargetLocation() + TargetOffset;
		
		if (HomingThreshold != -1.0f) 
		{			
			if(FVector::DistSquared(targetLocation, GetActorLocation()) <= FMath::Square(HomingThreshold))
			{
				IsHoming = false;
			}
		}

		if (IsHoming) {
			auto newRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), targetLocation);
			auto size = ProjectileMovementComponent->Velocity.Size();
			ProjectileMovementComponent->Velocity = size * newRotation.Vector();
			ProjectileMovementComponent->UpdateComponentVelocity();
		}		

		if (enableDebugVisuals) {
			DrawDebugLine(GetWorld(), GetActorLocation(), targetLocation, FColor::Turquoise, false, 0.1f);
		}
	}
	else if (IsHoming && !Target.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("homing target is no longer valid"));
		IsHoming = false;
	}


	if (enableDebugVisuals) {
		DrawDebugSphere(GetWorld(), GetActorLocation(), 10.0f, 64, FColor::White);
	}
}

void ABaseProjectile::Destroyed()
{
	Super::Destroyed();
}

void ABaseProjectile::PostInitProperties()
{
	Super::PostInitProperties();
	if(!ImpactActionHandler){
		ImpactActionHandler = NewObject<UImpactActionHandler>();
		UE_LOG(LogTemp, Warning, TEXT("ImpactActionHandler missing in PostInitProperties - created a new one...."));
	}
	ImpactActionHandler->PostInit();
}

namespace {
	bool IsFriendlyFire(AActor* instigator, AActor* target) {
		auto instigatorChar = Cast<ABaseCharacter>(instigator);
		auto targetChar = Cast<ABaseCharacter>(target);

		if (instigatorChar && targetChar) {
			return !instigatorChar->CanDamageTarget(targetChar);
		}
		
		return false;
	}

	bool ActorBelongsToFriend(AActor* instigator, AActor* other) {
		if (auto otherOwnerChar = Cast<ABaseCharacter>(other->GetInstigator())) {
			if (auto instigatorChar = Cast<ABaseCharacter>(instigator)) {
				return !instigatorChar->CanDamageTarget(otherOwnerChar);
			}
		}
		return false;
	}
}

bool ABaseProjectile::ShouldIgnoreCollision(class AActor* OtherActor) const {
	if (this == OtherActor
		|| !OtherActor->HasActorBegunPlay()
		|| OtherActor->IsPendingKillOrUnreachable()
		|| OtherActor->GetClass()->IsChildOf(AStorableItem::StaticClass()))
	{
		return true;
	}

	if (GetInstigator() != nullptr && (GetInstigator() == OtherActor || OtherActor->IsOwnedBy(GetInstigator()) || ActorBelongsToFriend(GetInstigator(), OtherActor))) {
		return true;
	}

	if (GetInstigator() != nullptr && IgnoreFriendlyFire && IsFriendlyFire(GetInstigator(), OtherActor)) {
		return true;
	}

	//Use precached health component for characters rather than searching for it
	if (auto targetCharacter = Cast<ABaseCharacter>(OtherActor)) {
		auto hc = targetCharacter->GetHealthComponent();
		if (hc && !hc->ShouldImpactProjectile(DamageType)) {
			return true;
		}
	}
	else
	if (const auto healthComponent = OtherActor->FindComponentByClass<UHealthComponent>()) {
		if (!healthComponent->ShouldImpactProjectile(DamageType)) {
			return true;
		}
	}

	if (DestroyOnHit && WillMiss && numHits >= 1) {
		// D11.DB - We want arrows to continue if Blind causes them to miss, but
		//			we don't want them to hit anything else (unless they have also
		//			been configured to penetrate enemies with DestroyOnHit).
		return true;
	}

	return false;
}

void ABaseProjectile::OnOverlapBegin(UPrimitiveComponent* /*ThisComp*/, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (expired) {
		return;
	}

	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor))
	{
		static FGameplayTag ImmunityDamage = FGameplayTag::RequestGameplayTag("Immunity.Damage");
		bool bIsDamageImune = BaseCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(ImmunityDamage);
		WillMiss = WillMiss || bIsDamageImune;

		if (WillMiss) {
			FGameplayCueParameters params;
			params.Instigator = CachedOwnerGameplayEffectSpec->GetContext().GetInstigator();
			params.Location = SweepResult.Location;
			static FName GameplayCueMiss = TEXT("GameplayCue.Miss");
			BaseCharacter->GetAbilitySystemComponent()->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(GameplayCueMiss), params);
		}
	}

	if (!WillMiss) {
		if (auto owner = GetInstigator()) {

			DungeonsGearUtilLibrary::OnProjectileOverlap(Cast<ABaseCharacter>(owner), OtherActor, OtherActor, GetActorLocation(), OtherActor->GetActorLocation(), this);
		}
	}

	if (ShouldIgnoreCollision(OtherActor))
		return;

	if (!WillMiss) {
		if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor))
		{
			if (auto targetEnchantmentComponent = OtherActor->FindComponentByClass<UEnchantmentComponent>()) {

				DungeonsGearUtilLibrary::OnOverlappedByProjectile(BaseCharacter, this);

				//Dirty way of letting OnOverlappedByProjectile expire the projectile...
				if (expired) return;
			}
		}
	}
	
	UHealthComponent* healthComponent = nullptr;

	bool blocking = false;
	if (auto targetCharacter = Cast<ABaseCharacter>(OtherActor)) {
		healthComponent = targetCharacter->GetHealthComponent();
		blocking = targetCharacter->IsBlocking();
	}
	else
	{
		healthComponent = OtherActor->FindComponentByClass<UHealthComponent>();
	}

	const FVector impactLocation = bFromSweep ? FVector(SweepResult.ImpactPoint) : GetActorLocation();

	FProjectileHitResponse hitResponse;
	ProcessHit(OtherActor, OtherComp, impactLocation, SweepResult.Normal, hitResponse);
	if (hitResponse.bounce)
	{
		SetActorRotation(hitResponse.bounceDirection.ToOrientationQuat());
		numHits++;
		LaunchProjectile(nullptr);
	}
	else if ((healthComponent == nullptr) || blocking) {
		if (bShouldHitTerrain) {
			ProjectileMovementComponent->StopMovementImmediately();
			expired = true;
		}
	}
	else {
		DealDamage(OtherActor, healthComponent);

		IsHoming = false;

		if (DestroyOnHit && !WillMiss) {
			ProjectileMovementComponent->StopMovementImmediately();
			expired = true;
		}
		else {
			numHits++;
		}
	}

	ImpactActionHandler->OnImpact(this, { true, expired, OtherActor, impactLocation});
}

void ABaseProjectile::OnOverlapEnd(AActor* overlappedActor, AActor* otherActor) {
	if (overlappedActor == otherActor)
		return;

	if (otherActor == this)
		std::swap(overlappedActor, otherActor);

	OnOverlapEnded.Broadcast(overlappedActor, otherActor);
}

void ABaseProjectile::DealDamage(AActor* actorToDamage, UHealthComponent* healthComponent) {
	if (PreviouslyHitActors.Contains(actorToDamage) && AffectTargetOnce) {
		return;
	}
	PreviouslyHitActors.Add(actorToDamage);

	if (healthComponent == nullptr) {
		healthComponent = actorToDamage->FindComponentByClass<UHealthComponent>();
	}

	if (GetInstigatorHadAuthority()) {
		if (ADungeonsGameMode* gameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode())) {
			gameMode->OnActorSpawnedProjectileHit(GetInstigator(), this);
		}
		
		if (healthComponent != nullptr) {
			FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::Normal);
			
			auto owner = GetInstigator();
			float initialDamage = GetDamageFactor() * damage;
			float VelocityDamage = bIsUnderwater ? FMath::GetMappedRangeValueClamped(FVector2D(0, ProjectileMovementComponent->InitialSpeed), FVector2D(0, GetDamageFactor() * damage), ProjectileMovementComponent->Velocity.Size()) : initialDamage;
			// Clamp damage to one third max of the initial damage
			VelocityDamage = FMath::Clamp(VelocityDamage, initialDamage * 0.33f, initialDamage);
			float mutableDamage = VelocityDamage;
			auto mutableDamageType = DamageType;

			if (IsValid(owner)) {

				DungeonsGearUtilLibrary::OnBeforeRangedDamageDealt(Cast<ABaseCharacter>(owner), mutableDamage, mutableDamageType, this, actorToDamage, actorToDamage, GetActorLocation(), actorToDamage->GetActorLocation());
			}

			const float appliedStunMultiplier = GetStunMultiplier() * (SpawnRecursionCounter <= 0 ? 1.0f : effects::PROJECTILE_SECONDARY_HIT_STUN_MULTIPLIER);
			
			auto populateSpec = [&, miss = WillMiss](FGameplayEffectSpec& spec) {
				spec.SetSetByCallerMagnitude(effects::HealthName, -mutableDamage);
				spec.CapturedSourceTags = FTagContainerAggregator();
				FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(spec);
				
				context->StunMultiplier = appliedStunMultiplier;

				if (!miss) {
					//Set any pushback to be applied to ragdolls (modified with a bonus amount and extra strength)
					effects::StorePushbackInNormal(spec, pushback.enablePushback ? pushback::getLaunchVector(pushback, *this, *actorToDamage, (IsCharged ? 3.f : 1.5f) * pushbackMultiplier, (IsCharged ? 2.f : 1.f)) : FVector::ZeroVector);
				}

				spec.DynamicAssetTags.AddTag(mutableDamageType);
				if (!IgnoreFriendlyFire) {
					spec.DynamicAssetTags.AddTag(damageTag::damageFriends());
				}
			};

			ABaseCharacter* damageTakerCharacter = Cast<ABaseCharacter, AActor>(actorToDamage);

			const auto* baseCharHC = damageTakerCharacter->FindComponentByClass<UHealthComponent>();
			const auto preDamageHealth = baseCharHC ? baseCharHC->GetCurrentHealth() : 0.f;

			UAbilitySystemComponent* ownerAbilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(owner);
			const auto targetAbilitySystem = damageTakerCharacter->GetAbilitySystemComponent();

			const auto targetIsVoided = targetAbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.VoidStrike")));

			if(ownerAbilitySystem && CachedOwnerGameplayEffectSpec.IsSet()){
				//projectile has an owner ability system - use the 'ApplyGameplayEffectSpecToTarget' pattern
				FGameplayEffectSpec spec = CachedOwnerGameplayEffectSpec.GetValue();
				if (targetIsVoided)
				{
					spec.DynamicAssetTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Void")));
				}
				populateSpec(spec);
				ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);

			} else {
				//projectile has no owner - we assume this is unowned - use fallback logic using 'ApplyGameplayEffectSpecToSelf' pattern.
				auto spec = GetOrCreateNoOwnerGameplayEffectSpec(targetAbilitySystem);		
				if (targetIsVoided)
				{
					spec.DynamicAssetTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Void")));
				}
				populateSpec(spec);
				targetAbilitySystem->ApplyGameplayEffectSpecToSelf(spec);
			}

			const auto postDamageHealth = baseCharHC ? baseCharHC->GetCurrentHealth() : 0.f;

			if (SpawnRecursionCounter <= 0 && !WillMiss) {
				pushback::pushback(pushback, *this, *actorToDamage, pushbackMultiplier);
			}

			if (auto gameInstance = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance())) {
				if (auto playerOwner = Cast<APlayerCharacter>(GetInstigator())) {
					auto equipment = playerOwner->GetEquipmentComponent();
					auto slots = equipment ? equipment->GetSlotsOfType(ESlotType::RangedWeapon) : TArray<UItemSlot*>();
					if (slots.Num() > 0) {
						 analytics::Analytics::GetInstance().FireWeaponUsed(*playerOwner, *slots[0], damageTakerCharacter, damage, preDamageHealth, postDamageHealth);
					}
				}
			}

			if (IsValid(owner)) {
				if (auto rangedAttackComponent = owner->FindComponentByClass<URangedAttackComponent>()) {
					rangedAttackComponent->OnRangedDamageDealt(mutableDamage, this, actorToDamage, actorToDamage, GetActorLocation(), actorToDamage->GetActorLocation());
				}
				DungeonsGearUtilLibrary::OnAfterDealtRangedDamage(Cast<ABaseCharacter>(owner), mutableDamage, this, actorToDamage, actorToDamage, GetActorLocation(), actorToDamage->GetActorLocation());
			}
		}

		//! This should not run if the mob died from hit, e.g applying fire to a mob that is ragdolling will make damage get applied continuously even if dead
		if (healthComponent->IsAlive()) {
			if (auto baseCharacter = Cast<ABaseCharacter>(actorToDamage)) {
				if (auto abilitySystem = baseCharacter->GetAbilitySystemComponent()) {
					for (auto spec : GameplaySpecsToApply) {
						abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
					}
				}

			}
		}
	}
}

const FGameplayEffectSpec& ABaseProjectile::GetOrCreateNoOwnerGameplayEffectSpec(const UAbilitySystemComponent* targetComponent)
{
	if (!CachedNoOwnerGameplayEffectSpec.IsSet()) {
		CachedNoOwnerGameplayEffectSpec = effects::CreateGameplayEffectSpec<UNoOnwerProjectileDamageGameplayEffect>(targetComponent, 1.f);
	}
	return CachedNoOwnerGameplayEffectSpec.GetValue();
}

float ABaseProjectile::GetEnvironmentSpeedMultiplier()
{
	// If the arrow is affected by CustomPhysics, the projectile movement component will take care of the speed
	if (!bAffectedByCustomPhysics) {
		ABaseCharacter* BaseOwner = Cast<ABaseCharacter>(Instigator);
		if (ArrowType == EArrowType::Water && !BaseOwner->IsUnderwater()) {
			return 1.5f;
		}
	}
	return 1.0f;
}

float ABaseProjectile::GetDamage() const {
	return damage;
}

float ABaseProjectile::GetExplosionDamage() const {
	return SerializedExplosionDamage;
}

float ABaseProjectile::GetExplosionRadius() const {
	return SerializedExplosionRadius;
}

void ABaseProjectile::SetDamage(float newDamage) {
	damage = newDamage;
}

float ABaseProjectile::GetStunMultiplier() const {
	return stunMultiplier;
}

void ABaseProjectile::EmptyPreviouslyHitActors() {
	PreviouslyHitActors.Reset();
}

void ABaseProjectile::SetDestroyOnHit(bool destroy) {
	DestroyOnHit = destroy;
}

void ABaseProjectile::ResetProjectile()
{

	if (IsValid(Instigator))
	{
		DungeonsGearUtilLibrary::OnResetRangedProjectile(Cast<ABaseCharacter>(Instigator), this);

		if (auto rangedAttackComponent = Instigator->FindComponentByClass<URangedAttackComponent>()) {
			rangedAttackComponent->ResetProjectile(this);
		}
	}

	numHits = 0;
	Instigator = nullptr;
	Target.Reset();
	expired = false;
	SpawnRecursionCounter = 0;
	CurrentProjectileLifetime = 0.0f;
	ProjectileMovementComponent->StopMovementImmediately();
	ProjectileMovementComponent->SetComponentTickEnabled(false);
	GameplaySpecsToApply.Empty();

	DisableCollisions();

	EmptyPreviouslyHitActors();

	//call any BP overrides
	OnProjectileReset();

	pushback = mDefaultValues.pushback;
	DamageType = mDefaultValues.DamageType;
	IsHoming = mDefaultValues.IsHoming;
	DestroyOnHit = mDefaultValues.DestroyOnHit;
	bShouldHitTerrain = mDefaultValues.bShouldHitTerrain;
	AffectTargetOnce = mDefaultValues.AffectTargetOnce;
	IgnoreFriendlyFire = mDefaultValues.IgnoreFriendlyFire;
	enableDebugVisuals = mDefaultValues.enableDebugVisuals;
	CollisionPhysicsForce = mDefaultValues.CollisionPhysicsForce;
	TimeThreshold = mDefaultValues.TimeThreshold;
	HomingThreshold = mDefaultValues.HomingThreshold;
	VelocityFactor = mDefaultValues.VelocityFactor;
	MaxProjectileLifeTime = mDefaultValues.MaxProjectileLifeTime;
	MaxDist5Degrees = mDefaultValues.MaxDist5Degrees;
	MaxDist40Degrees = mDefaultValues.MaxDist40Degrees;
	damage = mDefaultValues.damage;
	stunMultiplier = mDefaultValues.stunMultiplier;
	IsCharged = mDefaultValues.IsCharged;

	bCanTriggerEnchantments = mDefaultValues.bCanTriggerEnchantments;

	ProjectileMovementComponent->bRotationFollowsVelocity = mDefaultValues.ProjectileMovementComponent_bRotationFollowsVelocity;
	ProjectileMovementComponent->bShouldBounce = mDefaultValues.ProjectileMovementComponent_bShouldBounce;
	ProjectileMovementComponent->InitialSpeed = mDefaultValues.ProjectileMovementComponent_InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = mDefaultValues.ProjectileMovementComponent_MaxSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = mDefaultValues.ProjectileMovementComponent_ProjectileGravityScale;

	DamageFactor = mDefaultValues.DamageFactor;
	DamageFactorMultiplier = mDefaultValues.DamageFactorMultiplier;
	pushbackMultiplier = mDefaultValues.pushbackMultiplier;
	WeaponItemPower = mDefaultValues.WeaponItemPower;

	SourceItemPower.Reset();

	ImpactActionHandler->Reset();
}

void ABaseProjectile::DisableCollisions()
{
	for (auto a : GetComponentsByClass(UShapeComponent::StaticClass())) {
		if (UShapeComponent* collisionMesh = Cast<UShapeComponent>(a)) {
			collisionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

AActor* ABaseProjectile::FindEffectCauser(AActor* pOwner) {
	if (auto characterOwner = Cast<ABaseCharacter>(pOwner)) {
		if (auto master = characterOwner->GetMaster()) {
			return master;
		}
	}

	return this;
}

void ABaseProjectile::LaunchProjectile(AActor* pOwner)
{
	CurrentProjectileLifetime = 0.0f;

	spawnLocation = GetActorLocation();

	for (auto a : GetComponentsByClass(UShapeComponent::StaticClass())) {
		if (UShapeComponent* collisionMesh = Cast<UShapeComponent>(a)) {
			collisionMesh->IgnoreActorWhenMoving(pOwner, true);
		}
	}

	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bInitialVelocityInLocalSpace = true;

	ProjectileMovementComponent->Velocity = FVector(1.f, 0.f, 0.f);

	if (VelocityFactor != 1.f) {
		ProjectileMovementComponent->InitialSpeed *= VelocityFactor;
		ProjectileMovementComponent->MaxSpeed = ProjectileMovementComponent->InitialSpeed;
		ProjectileMovementComponent->Velocity *= VelocityFactor;
	}

	if (ProjectileMovementComponent->InitialSpeed > 0.f)
	{
		ProjectileMovementComponent->Velocity = ProjectileMovementComponent->Velocity.GetSafeNormal() * ProjectileMovementComponent->InitialSpeed;
	}

	ProjectileMovementComponent->SetVelocityInLocalSpace(ProjectileMovementComponent->Velocity);
	ProjectileMovementComponent->SetComponentTickEnabled(true);
	ProjectileMovementComponent->UpdateComponentVelocity();
	ProjectileMovementComponent->StoreInitialVelocity();

	ToggleCharged(IsCharged);
	ToggleMagical(IsMagical());

	if (GetInstigatorHadAuthority()) {
		UAbilitySystemComponent* abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(pOwner);
		if (abilitySystem) {
			const auto hitChance = abilitySystem->GetNumericAttribute(URangedAttributeSet::RangedAttackHitChanceMultiplierAttribute());
			const bool hit = FMath::RandRange(0.0f, 1.0f) < hitChance;
			if (hit) {
				SetGameplayEffectSpec(effects::CreateGameplayEffectSpec<UBaseProjectileDamageGameplayEffect>(abilitySystem, 1.f));
				WillMiss = false;
			}
			else {
				SetGameplayEffectSpec(effects::CreateGameplayEffectSpec<UBlindProjectileDamageGameplayEffect>(abilitySystem, 1.f));
				WillMiss = true;
			}
			CachedOwnerGameplayEffectSpec->GetContext().AddOrigin(pOwner->GetActorLocation());

			CachedOwnerGameplayEffectSpec->GetContext().AddInstigator(pOwner, FindEffectCauser(pOwner));

			FGameplayEffectQuery query = FGameplayEffectQuery::MakeQuery_MatchAllEffectTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.Dynamo.Ranged")));

			CachedOwnerGameplayEffectSpec->SetSetByCallerMagnitude(UDynamoRanged::DynamoRangedStackCountKey, abilitySystem->GetAggregatedStackCount(query));

			if (ApplyEffectOnHit != nullptr) {
				auto effect = Cast<UGameplayEffect>(ApplyEffectOnHit->GetDefaultObject());
				auto context = (const FGameplayEffectContextHandle)abilitySystem->MakeEffectContext();
				context.AddInstigator(GetInstigator(), this);
				GameplaySpecsToApply.Add(FGameplayEffectSpec(effect, context, SourceItemPower.Get(1.f)));
			}

			if (ABaseCharacter* baseCharacter = Cast<ABaseCharacter>(pOwner)) {
				bIsUnderwater = bAffectedByCustomPhysics && baseCharacter->IsUnderwater();
			}
		}

		// #D11.CM - Trigger any enchantment OnProjectileLaunch functions
		if (bCanTriggerEnchantments) {
			DungeonsGearUtilLibrary::OnProjectileLaunched(Cast<ABaseCharacter>(pOwner), this);
		}
	}

	ImpactActionHandler->OnLaunch(this);

	//call any BP overrides
	OnProjectileLaunch(pOwner);
}

void ABaseProjectile::EnableProjectileCollisions(bool Val)
{
	auto collisionType = (Val) ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision;
	for (auto a : GetComponentsByClass(UShapeComponent::StaticClass()))
	{
		if (UShapeComponent* collisionMesh = Cast<UShapeComponent>(a))
		{
			collisionMesh->SetCollisionEnabled(collisionType);
		}
	}
}

int ABaseProjectile::GetNumHits() const {
	return numHits;
}

TOptional<FGameplayEffectSpec> ABaseProjectile::GetCachedOwnerGameplayEffectSpec() const {
	return CachedOwnerGameplayEffectSpec;
}

TOptional<FGameplayEffectSpec> ABaseProjectile::GetCachedNoOwnerGameplayEffectSpec() const {
	return CachedNoOwnerGameplayEffectSpec;
}

FVector ABaseProjectile::GetHomingTargetLocation() {
	return Target->GetActorLocation() + TargetOffset;
}

void ABaseProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseProjectile, Target);
	DOREPLIFETIME(ABaseProjectile, IsCharged);
	DOREPLIFETIME(ABaseProjectile, bCanTriggerEnchantments);
	DOREPLIFETIME(ABaseProjectile, VelocityFactor);
	DOREPLIFETIME(ABaseProjectile, DamageFactor);
	DOREPLIFETIME(ABaseProjectile, DestroyOnHit);
}

bool ABaseProjectile::GetInstigatorHadAuthority() const {
	return InstigatorHadAuthority;
}

bool ABaseProjectile::IsEnchanted() const
{
	return bCanTriggerEnchantments;
}

void ABaseProjectile::ToggleCharged_Implementation(bool charged) {
}

void ABaseProjectile::ToggleMagical_Implementation(bool magical) {

}

UBaseProjectileDamageGameplayEffect::UBaseProjectileDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = URangedDamageModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Ranged")));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Ranged"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();

	FGameplayEffectExecutionDefinition lifesteal;
	lifesteal.CalculationClass = URangedLifestealExecution::StaticClass();
	Executions.Add(std::move(lifesteal));
	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));
	FGameplayEffectExecutionDefinition dynamo;
	dynamo.CalculationClass = UDynamoRangedCueExecution::StaticClass();
	Executions.Add(std::move(dynamo));
}

UBlindProjectileDamageGameplayEffect::UBlindProjectileDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Miss"), 0, 1);
}

UNoOnwerProjectileDamageGameplayEffect::UNoOnwerProjectileDamageGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UWorldDamageModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Ranged")));
	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Ranged"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

void ABaseProjectileProp::AttachedToActorHidden_Implementation(AActor* attachedToActor) {
	UProjectileFunctionLibrary::DeSpawnAttachedArrowProp(this);
}


void ABaseProjectile::SetGameplayEffectSpec(const FGameplayEffectSpec & spec) {
	CachedOwnerGameplayEffectSpec = spec;
}

void ABaseProjectile::PreSave(const ITargetPlatform * TargetPlatform) {
    ensureMsgf(
        ImpactActionHandler,
        TEXT("%s has NULL ImpactActionHandler!"),
        *this->GetClass()->GetName()
    );

	SerializedExplosionDamage = algo::sum_with_init(ImpactActionHandler->GetActions<UExplosionAction>(), [](const UExplosionAction* action) { return action->ExplosionDamage; }, 0.f);

	auto maxelement = algo::max_element_by(ImpactActionHandler->GetActions<UExplosionAction>(), [](const UExplosionAction* action) { return action->ExplosionRadius; });
	SerializedExplosionRadius = maxelement ? maxelement.GetValue()->ExplosionRadius : 0.f;
}
