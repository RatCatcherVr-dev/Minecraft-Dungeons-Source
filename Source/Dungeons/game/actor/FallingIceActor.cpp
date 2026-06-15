#include "FallingIceActor.h"
#include "game/util/ActorQuery.h"
#include "AbilitySystemComponent.h"
#include "character/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/item/instance/TotemOfShielding.h"
#include "character/player/PlayerCharacter.h"

// ----- Falling Ice Gameplay Effect ----- //
UFallingIceDamageGameplayEffect::UFallingIceDamageGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Item"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

// ----- Falling Ice Actor ----- //
AFallingIceActor::AFallingIceActor() {
	PrimaryActorTick.bCanEverTick = true;

	mMovementComponent = CreateDefaultSubobject<USimpleMovementComponent>(TEXT("MovementComponent"));
	mMovementComponent->SetComponentTickEnabled(false);
	mMovementComponent->SetIsReplicated(true);

	mBoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	mBoxCollider->SetGenerateOverlapEvents(false);
	mBoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AFallingIceActor::OnHitboxOverlap);
	mBoxCollider->bEditableWhenInherited = true;

	RootComponent = mBoxCollider;

	SetReplicates(true);
	SetReplicateMovement(true);
	NetUpdateFrequency = 20.0f;
}

void AFallingIceActor::BeginPlay() {
	InstanceTracker<AFallingIceActor>::AddInstance(GetWorld(), this);
	Super::BeginPlay();
}

void AFallingIceActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	InstanceTracker<AFallingIceActor>::RemoveInstance(GetWorld(), this);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AFallingIceActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AFallingIceActor::OnSummonPlayed() {
	// Now that our FX have finished, turn our collider on
	mBoxCollider->SetGenerateOverlapEvents(true);

	// Start drop timer
	GetWorld()->GetTimerManager().SetTimer(mDropTimerHandle, this, &AFallingIceActor::DropIceblock, mDropDelay);
}

void AFallingIceActor::DropIceblock() {
	mMovementComponent->Velocity = FVector::DownVector * mDropSpeed;
	mMovementComponent->SetComponentTickEnabled(true);
	mMovementComponent->UpdateComponentVelocity();

	// Start timeout timer
	GetWorld()->GetTimerManager().SetTimer(mDropTimeoutHandle, this, &AFallingIceActor::IceblockCleanup, mDropTimeout);
}

void AFallingIceActor::OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (GetOwner() && GetOwner()->IsA<APlayerCharacter>() && OtherActor->IsA<ATotemOfShielding>()) {
		// Player iceblocks should pass through totem shields.
		return;
	}

	OnIceblockCollision();
}

void AFallingIceActor::OnIceblockCollision() {
	if (!GetOwner()) {
		return;
	}

	const ABaseCharacter* ownerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (!ownerCharacter) {
		return;
	}

	// Create our pushback
	FPushback push;
	push.pushbackStrength = 5.0f;
	push.enablePushback = true;

	// Loop all potential targets in explosion range
	if (UAbilitySystemComponent* ownerAbilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>()) {

		// Create specs
		FGameplayEffectSpec damageSpec = effects::CreateGameplayEffectSpec<UFallingIceDamageGameplayEffect>(ownerAbilitySystem, effects::HealthName, 0, GetOwner(), this, GetActorLocation(), 1.f);

		auto stunSpec = FGameplayEffectSpec(Cast<const UGameplayEffect>(StunEffect->GetDefaultObject()), ownerAbilitySystem->MakeEffectContext());
		stunSpec.SetSetByCallerMagnitude(effects::DurationName, bOverwriteStunDuration ? mStunDuration : game::item::type::IceWand.getDurationSeconds());

		auto stunImmunitySpec = effects::CreateGameplayEffectSpec<UTemporaryStunimmunityGameplayEffect>(ownerAbilitySystem, 1.f);
		stunImmunitySpec.SetSetByCallerMagnitude(FName("Duration"), 6.f);

		// Search our targetable actors.
		bool glowSquid = false;
		int count = 0;
		const auto targetableActors = actorquery::getNearbyInstanceTrackedActors<ABaseCharacter>(GetWorld(), GetActorLocation(), mExplosionOuterRange).FilterByPredicate([&](ABaseCharacter* target) { return ownerCharacter->CanDamageTarget(target); });
		for (auto target : targetableActors) {
			if (UAbilitySystemComponent* targetAbilitySystem = target->FindComponentByClass<UAbilitySystemComponent>()) {
				if (target->IsAlive()) {
					// Get our distance from the epicenter of the damage.
					// Because this is used to calculate damage, getting the non-squared distance.
					float dist2D = actorquery::getActorDistance2D(this, target);
					bool insideInnerRadius = dist2D <= mExplosionInnerRange;

					// Apply a stun and a more potent stun immunity
					if (ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(stunSpec, targetAbilitySystem).WasSuccessfullyApplied()) {
						ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(stunImmunitySpec, targetAbilitySystem);
					}

					// Calculate Damage
					float scaledDamage = mExplosionDamage;
					if (!insideInnerRadius) {
						float percentileDistanceFromInnerRadius = (dist2D - mExplosionInnerRange / mExplosionOuterRange - mExplosionInnerRange);
						float multiplier = 1 - Math::max(0, Math::min(1, percentileDistanceFromInnerRadius));
						scaledDamage *= multiplier;
					}

					// Create and apply damage spec
					damageSpec.SetSetByCallerMagnitude(effects::HealthName, -scaledDamage);
					ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(damageSpec, targetAbilitySystem);

					// Add our pushbacks and ragdolls
					pushback::pushback(push, *this, *target);
					effects::StorePushbackInNormal(damageSpec, pushback::getLaunchVector(push, *this, *target, 1.0f, 1.5f));

					// D11.SSN
					if (const auto mob = Cast<AMobCharacter>(target)) {
						if (mob->IsNotAlive()) {
							count++;
							if (mob->EntityType == EntityType::GlowSquid) {
								glowSquid = true;
							}
						}
					}
				}
			}
		}

		// D11.SSN
		if (auto* playerCharacter = Cast<APlayerCharacter>(GetOwner())) {
			playerCharacter->IceWandKillCount = count;
			playerCharacter->OnRep_IceWandKillCount();
			if (glowSquid) {
				playerCharacter->OnIceWandGlowSquid();
			}
		}
	}

	// Explode our actor
	CreateExplosionEffect(ownerCharacter);
	IceblockCleanup();
}

void AFallingIceActor::CreateExplosionEffect(const ABaseCharacter* ownerCharacter) {
	auto abilitySystem = ownerCharacter->FindComponentByClass<UAbilitySystemComponent>();

	FGameplayCueParameters params;
	params.AbilityLevel = 1.f;
	params.NormalizedMagnitude = 1.f;
	params.Instigator = ownerCharacter;
	params.EffectCauser = this;
	params.Location = GetActorLocation();

	abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Explosion.FallingIce")), params);
}

void AFallingIceActor::IceblockCleanup() {
	if (IsPendingKill()) {
		return;
	}

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Destroy();
}
