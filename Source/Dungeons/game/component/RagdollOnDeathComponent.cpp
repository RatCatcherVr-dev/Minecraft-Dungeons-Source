#include "Dungeons.h"
#include "RagdollOnDeathComponent.h"
#include "game/component/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include <Animation/AnimInstance.h>
#include "MassComponent.h"
#include "util/FloatRange.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/util/DungeonsEffectLibrary.h"
#include "game/Game.h"
#include "game/mission/MissionDefs.h"
#include "game/mission/MissionDef.h"

DECLARE_STATS_GROUP(TEXT("HealthComponent"), STATGROUP_RagdollDeathComponent, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("OnRagdollDeath"), STAT_RagdollDeath, STATGROUP_RagdollDeathComponent);

URagdollOnDeathComponent::URagdollOnDeathComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	DLCDissolveMultiplier.Add(EDLCName::Oceans, .5f);
}

void URagdollOnDeathComponent::BeginPlay() {
	Super::BeginPlay();
	MeshComponent()->SetEnableGravity(true);
	cachedMeshCollisionType = MeshComponent()->GetCollisionEnabled();
	cachedMeshCollisionResponse = MeshComponent()->GetCollisionResponseToChannels();

	auto* owner = Cast<ABaseCharacter>(GetOwner());
	if (TriggerOnDeath) {
		owner->OnDeath.AddUObject(this, &URagdollOnDeathComponent::Ragdoll);
		owner->OnRevive.AddUObject(this, &URagdollOnDeathComponent::UnRagdoll);
	}
}

void URagdollOnDeathComponent::Ragdoll() 
{
	SCOPE_CYCLE_COUNTER(STAT_RagdollDeath)

	USkeletalMeshComponent* meshComponent = MeshComponent();

	//D11.SC disable override physics asset
	if (meshComponent->PhysicsAssetOverride) {
		meshComponent->SetPhysicsAsset(nullptr);
	}
	
	GetOwner()->SetActorTickEnabled(false);
	
	if (!ragdollEnabled) {
		meshComponent->SetVisibility(false);
		return;
	}

	meshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	auto* owner = Cast<ABaseCharacter>(GetOwner());
	if (auto* abilitySystem = owner->GetAbilitySystemComponent()) {
		if (owner->IsUnderwater()) {
			MeshComponent()->SetEnableGravity(false);
			underwater = true;
		}
		else {
			MeshComponent()->SetEnableGravity(true);
			underwater = false;
		}
	}

	if (IsComponentTickEnabled()) {
		SetComponentTickEnabled(false);
		meshComponent->SetAllBodiesBelowSimulatePhysics(PhysicsBlendRootBoneName, false, PhysicsBlendIncludeRootBone);
		meshComponent->SetAllBodiesBelowPhysicsBlendWeight(PhysicsBlendRootBoneName, 0.f, false, PhysicsBlendIncludeRootBone);
	}
	// physics
	
	//Disable occluding geometry silhouetting
	owner->EnableOcclusionSilhouetting = false;

	meshComponent->PutAllRigidBodiesToSleep();
	meshComponent->SetSimulatePhysics(true);
	meshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	meshComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	meshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	meshComponent->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(ECustomTraceChannels::PlayerPawn), ECR_Overlap);
	meshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	meshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	meshComponent->SetGenerateOverlapEvents(0);
	meshComponent->SubInstances.Empty();

	if (auto bodyInstance = meshComponent->GetBodyInstance()) {
		bodyInstance->bUseCCD = true;
	}

	meshComponent->DetachFromParent(true);
	IsDoingRagdolling = true;
	
	// Animations
	if ( meshComponent->AnimScriptInstance )		// D11.Fred : Getting crash reports here on xbox/ps4, following the pattern of ClearAnimScriptInstance
		meshComponent->AnimScriptInstance->EndNotifyStates();
	meshComponent->bPauseAnims = true;

	if (CharacterMovementComponent())
	{
		CharacterMovementComponent()->SetMovementMode(EMovementMode::MOVE_None);
		CharacterMovementComponent()->Deactivate();
	}

	float dissolveDelayMultiplier = 1.f;
	if (auto game = actorquery::getGame(GetWorld())) {
		if (game->settings().levelName != ELevelNames::Invalid)
		{
			const MissionDef& mission = missions::get(game->settings().levelName);
			if (mission.getRequiredDLC().IsSet())
			{
				EDLCName DLCName = mission.getRequiredDLC().GetValue();
				if (DLCDissolveMultiplier.Contains(DLCName))
				{
					dissolveDelayMultiplier = DLCDissolveMultiplier[DLCName];
				}
			}
		}
	}
	
	Desaturate(dissolveDelay * dissolveDelayMultiplier);
	GetWorld()->GetTimerManager().SetTimer(visibilityHandle, this, &URagdollOnDeathComponent::HideParticles, (dissolveDelay + dissolveDuration) * .5f * dissolveDelayMultiplier, false);
	GetWorld()->GetTimerManager().SetTimer(dissolveHandle, FTimerDelegate::CreateUObject(this, &URagdollOnDeathComponent::DissolveSkin, dissolveDuration), dissolveDelay * dissolveDelayMultiplier, false);


	if (StoredRagdollImpulse.IsSet()) {
		LaunchRagdoll(StoredRagdollImpulse.GetValue());
		StoredRagdollImpulse.Reset();
	}

	OnRagdollChanged.Broadcast();
}

void URagdollOnDeathComponent::UnRagdoll() {

	GetOwner()->SetActorTickEnabled(true);

	auto* meshComponent = MeshComponent();
	if (!ragdollEnabled) {
		meshComponent->SetVisibility(true);
		return;
	}

	meshComponent->SetSimulatePhysics(false);
	MeshComponent()->SetCollisionEnabled(cachedMeshCollisionType);
	MeshComponent()->SetCollisionResponseToChannels(cachedMeshCollisionResponse);
	meshComponent->SetGenerateOverlapEvents(0);

	meshComponent->AttachTo(CapsuleComponent(), NAME_None, EAttachLocation::SnapToTargetIncludingScale, true);
	FRotator targetRotation = CapsuleComponent()->GetComponentTransform().GetRotation().Rotator();
	targetRotation.Yaw = meshComponent->GetComponentTransform().GetRotation().Rotator().Yaw - 90.f;
	meshComponent->SetWorldRotation(targetRotation);
	meshComponent->bPauseAnims = false;

	if (CharacterMovementComponent())
	{
		CharacterMovementComponent()->SetMovementMode(EMovementMode::MOVE_Walking);
		CharacterMovementComponent()->Activate();
	}

	IsDoingRagdolling = false;

	//Enable occluding geometry silhouetting
	auto owner = Cast<ABaseCharacter>(GetOwner());
	owner->EnableOcclusionSilhouetting = true;

	ShowParticles();
	UnDissolveSkin();
	Saturate();

	OnRagdollChanged.Broadcast();
}

void URagdollOnDeathComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsBlendStartTimeSeconds <= 0.f) {
		return;
	}

	float weight = 1.0f - FMath::Clamp((GetWorld()->GetTimeSeconds() - PhysicsBlendStartTimeSeconds) / PhysicsBlendTimeSeconds, 0.f, 1.f);
	MeshComponent()->SetAllBodiesBelowPhysicsBlendWeight(PhysicsBlendRootBoneName, weight * PhysicsBlendMultiplier, false, PhysicsBlendIncludeRootBone);

	if (weight <= 0.0f) {
		MeshComponent()->SetAllBodiesBelowSimulatePhysics(PhysicsBlendRootBoneName, false, PhysicsBlendIncludeRootBone);
		SetComponentTickEnabled(false);
		StoredRagdollImpulse.Reset();
	}
}

bool URagdollOnDeathComponent::IsRagdolling() const {
	return IsDoingRagdolling;
}

void URagdollOnDeathComponent::DissolveSkin(float duration) {
	UDungeonsEffectLibrary::DissolveActor(GetOwner(), duration);
}

void URagdollOnDeathComponent::UnDissolveSkin() {
	if (dissolveHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(dissolveHandle);
	}
	else {
		TArray<UMeshComponent*> componentsToDissolve;
		GetOwner()->GetComponents(componentsToDissolve, true);

		for (auto component : componentsToDissolve) {

			int index = 0;
			for (auto&& material : component->GetMaterials()) {
				auto materialDynamic = Cast<UMaterialInstanceDynamic>(material);
				if (materialDynamic == nullptr) {
					materialDynamic = UMaterialInstanceDynamic::Create(material, this);
					component->SetMaterial(index, materialDynamic);
				}
				materialDynamic->SetScalarParameterValue("DissolveDuration", 0);
				materialDynamic->SetScalarParameterValue("DissolveStart", 0);
				index++;
			}
		}
	}
}

void URagdollOnDeathComponent::Desaturate(float duration) {
	int index = 0;
	auto meshComponent = MeshComponent();
	for (auto&& material : meshComponent->GetMaterials()) {
		auto materialDynamic = Cast<UMaterialInstanceDynamic>(material);
		if (materialDynamic == nullptr) {
			materialDynamic = UMaterialInstanceDynamic::Create(material, this);
			meshComponent->SetMaterial(index, materialDynamic);
		}
		materialDynamic->SetScalarParameterValue("DesaturateDuration", duration);
		materialDynamic->SetScalarParameterValue("DesaturateStart", GetWorld()->GetTimeSeconds());
		index++;
	}
}

void URagdollOnDeathComponent::Saturate() {
	int index = 0;
	auto meshComponent = MeshComponent();
	for (auto&& material : meshComponent->GetMaterials()) {
		auto materialDynamic = Cast<UMaterialInstanceDynamic>(material);
		if (materialDynamic == nullptr) {
			materialDynamic = UMaterialInstanceDynamic::Create(material, this);
			meshComponent->SetMaterial(index, materialDynamic);
		}
		materialDynamic->SetScalarParameterValue("DesaturateDuration", 0);
		materialDynamic->SetScalarParameterValue("DesaturateStart", 0);
		index++;
	}
}

void URagdollOnDeathComponent::HideParticles() {
	for (auto particleSystem : GetOwner()->GetComponentsByClass(UParticleSystemComponent::StaticClass())) {
		if (particleSystem->IsActive()) {
			particleSystemsToActivate.Add(particleSystem);
			
			particleSystem->Deactivate();
		}
	}
}

void URagdollOnDeathComponent::ShowParticles() {
	if (visibilityHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(visibilityHandle);
		particleSystemsToActivate.Empty();
		return;
	}

	for (auto particleSystem : particleSystemsToActivate) {
		particleSystem->Activate();
	}
	particleSystemsToActivate.Empty();
}

void URagdollOnDeathComponent::LaunchOrStoreRagdollImpulse(const FVector& impulse) {
	if (impulse.IsZero()) {
		return;
	}

	const auto* healthComponent = HealthComponent();
	if (healthComponent && healthComponent->IsAlive()) {
		StoredRagdollImpulse = impulse;
	}
	else {
		StoredRagdollImpulse.Reset();

		//D11.SC disable override physics asset
		USkeletalMeshComponent* meshComponent = MeshComponent();
		if (meshComponent->PhysicsAssetOverride)
		{
			meshComponent->SetPhysicsAsset(nullptr);
		}

		LaunchRagdoll(impulse);
	}
}

void URagdollOnDeathComponent::ApplyPhysicsBlendWithRadialImpulse(const FName bone, const FVector impactVector, float force) {
	const auto* healthComponent = HealthComponent();
	if (healthComponent && healthComponent->IsAlive()) {
		auto* meshComponent = MeshComponent();
		PhysicsBlendStartTimeSeconds = GetWorld()->GetTimeSeconds();
		SetComponentTickEnabled(true);
		meshComponent->SetAllBodiesBelowSimulatePhysics(PhysicsBlendRootBoneName, true, PhysicsBlendIncludeRootBone);
		meshComponent->SetAllBodiesBelowPhysicsBlendWeight(PhysicsBlendRootBoneName, PhysicsBlendMultiplier, false, PhysicsBlendIncludeRootBone);

		const float radius = 150.f;
		const FVector offset = FVector(-15.f, 0, 0);

		FVector loc = meshComponent->GetBoneLocation(bone);
		loc += impactVector.GetSafeNormal().ToOrientationRotator().RotateVector(offset);

		meshComponent->AddRadialImpulse(loc, radius, force * PhysicsBlendMultiplier, ERadialImpulseFalloff::RIF_Linear, true);
	}
}

void URagdollOnDeathComponent::LaunchRagdoll(const FVector& impactVector) const {
	const float scalarStrength = impactVector.Size();

	if (underwater) {
		MeshComponent()->SetAllPhysicsAngularVelocityInRadians(FVector(0.f, 0.f, scalarStrength * 52.f * angularForceMultiplier * angularForceMultiplier), false);
		MeshComponent()->SetAllPhysicsLinearVelocity(impactVector.GetSafeNormal() * scalarStrength * linearForceMultiplier, false);
	}
	else {
		MeshComponent()->SetAllPhysicsAngularVelocityInRadians(FVector(0.f, 0.f, scalarStrength * 52.f), false);
		MeshComponent()->SetAllPhysicsLinearVelocity(impactVector.GetSafeNormal() * scalarStrength, false);
	}
}

UCapsuleComponent* URagdollOnDeathComponent::CapsuleComponent() const {
	if (auto character = Cast<ACharacter>(GetOwner())) {
		character->GetCapsuleComponent();
	}

	return GetOwner()->FindComponentByClass<UCapsuleComponent>();
}


USkeletalMeshComponent* URagdollOnDeathComponent::MeshComponent() const {
	if (auto character = Cast<ACharacter>(GetOwner())) {
		character->GetMesh();
	}

	return GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
}

UCharacterMovementComponent* URagdollOnDeathComponent::CharacterMovementComponent() const {
	if (auto character = Cast<ACharacter>(GetOwner())) {
		character->GetCharacterMovement();
	}

	return GetOwner()->FindComponentByClass<UCharacterMovementComponent>();
}

UMassComponent* URagdollOnDeathComponent::MassComponent() const {
	return GetOwner()->FindComponentByClass<UMassComponent>();
}

UHealthComponent* URagdollOnDeathComponent::HealthComponent() const {
	return GetOwner()->FindComponentByClass<UHealthComponent>();
}