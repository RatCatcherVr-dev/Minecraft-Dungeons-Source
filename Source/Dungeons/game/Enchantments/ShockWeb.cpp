// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "ShockWeb.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include <AbilitySystemComponent.h>
#include "game/actor/item/ProjectileUtil.h"
#include "game/actor/item/Arrow.h"
#include "game/item/instance/FireworkArrow.h"
#include "game/actor/ImpactActionHandler.h"
#include <Components/CapsuleComponent.h>
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/component/ContinousDamageComponent.h"
#include "game/component/BeamTargetUpdaterComponent.h"



AShockWebNodeActor::AShockWebNodeActor(const FObjectInitializer& ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	Rotator = CreateDefaultSubobject<USceneComponent>(TEXT("Rotator"));
	SetRootComponent(Root);
	DamageCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("DamageCollision"));
	DamageCollision->SetupAttachment(Rotator);
	DamageCollision->SetCapsuleHalfHeight(0, false);
	DamageCollision->SetRelativeRotation(FRotator(90, 0, 0));
	DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageDuration = TotalLifeSpan = 20.f;
	DamageComponent->bContinousApplication = true;
	DamageComponent->bRemoveEffectEndOverlap = true;
	//Bias by half block diagonal + 1 to not have blocks block beams.


	BeamUpdater = CreateDefaultSubobject<UActorBeamTargetUpdaterComponent>(TEXT("BeamUpdater"));
}


void AShockWebNodeActor::Tick(float deltaTime) {
	Super::Tick(deltaTime);
	//Trace updates here
	if (Target.IsValid()) {
		FVector origin = GetActorLocation();
		FVector target = Target->GetActorLocation();

		FVector direction = target - origin;
		const float distance = direction.Size();

		if (distance <= Range) {
			auto world = GetWorld();
			FTraceDatum traceData;

			bool hasData = world->QueryTraceData(BeamTrace, traceData);
			bool validHandle = world->IsTraceHandleValid(BeamTrace, false);

			//We do a blocking trace towards terrain here. if that fails the beam is not active.
			if (hasData) {
				ToggleBeam(traceData.OutHits.Num() == 0);
			}
			
			//Do a new trace if we had data or if the handle is invalid
			if (hasData || !validHandle) {
				const auto directionNormalized = direction / distance;
				const auto startPoint = origin + directionNormalized * CurrentTraceBias;
				const auto endPoint = target - directionNormalized * Target->CurrentTraceBias;

				static ECollisionChannel channel = static_cast<ECollisionChannel>(ECustomTraceChannels::TerrainOnly);
				BeamTrace = GetWorld()->AsyncLineTraceByChannel(EAsyncTraceType::Test, startPoint, endPoint, channel);
			}
		}

		FTransform worldTransform(FMath::Lerp(GetActorLocation(), Target->GetActorLocation(), 0.5f));


		DamageCollision->SetCapsuleHalfHeight(distance / 2, false);
		Rotator->SetWorldLocation(FMath::Lerp(GetActorLocation(), Target->GetActorLocation(), 0.5f));
		Rotator->SetWorldRotation(direction.ToOrientationRotator());
	}
	else {
		SetActorTickEnabled(false);
		ToggleBeam(false);
	}
}

void AShockWebNodeActor::Attach(const FImpactInfo & impact) {
	if (auto character = Cast<ABaseCharacter>(impact.ImpactedActor)) {
		AttachToComponent(character->GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		CurrentTraceBias = 0;
		character->OnDeath.AddUObject(this, &AShockWebNodeActor::OnCharacterDeath);
	}
	else {
		AttachToActor(impact.ImpactedActor, FAttachmentTransformRules::KeepRelativeTransform);
		CurrentTraceBias = TraceBiasTerrain;
	}
}

void AShockWebNodeActor::OnCharacterDeath() {
	DetachRootComponentFromParent();
	OnDetached();
}

void AShockWebNodeActor::Expire() {
	LifeSpanExpired();
}

void AShockWebNodeActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShockWebNodeActor, bIsBeamActive);
	DOREPLIFETIME(AShockWebNodeActor, bIsFading);
	DOREPLIFETIME(AShockWebNodeActor, Target);
}

void AShockWebNodeActor::ToggleBeam(bool enabled) {
	if (bIsBeamActive != enabled) {
		bIsBeamActive = enabled;
		OnRep_IsActive();
	}
}

void AShockWebNodeActor::LifeSpanExpired() {
	if(bIsFading) {
		Super::LifeSpanExpired();
	} else {
		bIsFading = true;
		OnRep_IsFading();
		DamageComponent->DisableDamage();
		SetLifeSpan(AutoDestroyDelaySeconds);
	}
}

void AShockWebNodeActor::OnRep_IsActive() {
	if (bIsBeamActive) {
		OnBeamEnabled();
		DamageCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	} else {
		OnBeamDisabled();
		DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}	
}
void AShockWebNodeActor::OnRep_IsFading() {
	if(bIsFading) {
		OnWillDestroy();
	}
}

void AShockWebNodeActor::OnRep_Target() {
	if(Target.IsValid()) BeamUpdater->SetActorTarget(Target.Get());
}


void AShockWebNodeActor::SetTarget(AShockWebNodeActor* target, float range) {
	Range = range;
	Target = target;
	OnRep_Target();

	SetActorTickEnabled(true);
}

UShockWeb::UShockWeb() {
	TypeId = EEnchantmentTypeID::ShockWeb;
	LevelMultiplier = [](int level) -> float {
		return level;
	};
	MultiplierFormatter = valueformat::asConstant;
}

void UShockWeb::OnProjectileLaunch(ABaseProjectile* projectile) {
	if (!projectile->GetInstigatorHadAuthority()) return;

	bool allowedProjectile = !projectile->IsA<AArrow>() || !DeniedProjectiles.Contains(projectile->GetClass());
	if (allowedProjectile && projectile->IsCharged) {
		BroadcastEnchantmentTriggeredEvent();

		if (USpawnActorAction* action = NewObject<USpawnActorAction>(projectile)) {
			action->ActorClass = ActorToSpawn;
			const float spawnOffset =  ActorToSpawn.GetDefaultObject()->WorldSpawnOffset;
			action->TransformGenerator.BindLambda([spawnOffset](const ABaseProjectile* projectile, const FImpactInfo& info) { 
				if (info.ImpactedActor->IsA<ABaseCharacter>()) {
					return FTransform();
				}

				FVector location = projectile->GetActorLocation() - projectile->GetActorRotation().Vector() * spawnOffset;
				
				return FTransform(FRotator(), location, FVector(1,1,1));
			});

			auto abilitySystem = GetAbilitySystemComponent();
			auto spec = abilitySystem-> MakeOutgoingSpec(UShockWebGameplayEffect::StaticClass(), Level, abilitySystem->MakeEffectContext());
			spec.Data->SetSetByCallerMagnitude(effects::HealthName, -DamagePerSecond * Period);
			spec.Data->Period = Period;
			
			action->OnActorSpawned.AddUObject(this, &UShockWeb::OnNodeSpawned, spec);
			projectile->ImpactActionHandler->AddRuntimeAction(action);
		}
	}
}

void UShockWeb::OnNodeSpawned(AActor* node, const FImpactInfo& impact, FGameplayEffectSpecHandle specHandle) {
	SpawnedActors.RemoveAll(RETLAMBDA(it == nullptr));

	if (auto shockNode = Cast<AShockWebNodeActor>(node)) {
	
		shockNode->Attach(impact);

		if (SpawnedActors.Num() == static_cast<int>(LevelMultiplier(Level)) + 1) {
			SpawnedActors[0]->Expire();
			SpawnedActors.RemoveAt(0);
		}

		if (SpawnedActors.Num() > 0) {
			shockNode->SetTarget(SpawnedActors.Last(), Range);
		}

		shockNode->SetGameplayEffectSpec(specHandle);
		shockNode->GetDamageComponent()->SetComponentTickInterval(Period);
		shockNode->TotalLifeSpan = shockNode->DamageDuration = Duration;
		shockNode->Instigator = GetCharacterOwner();

		SpawnedActors.Push(shockNode);
	}
}

UShockWebGameplayEffect::UShockWebGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Damage.Aoe"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Lightning")));

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = URangedItemPowerOnlyModDamageCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Hit.Shockweb"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}