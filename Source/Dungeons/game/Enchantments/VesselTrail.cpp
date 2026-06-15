#include "Dungeons.h"
#include "VesselTrail.h"
#include "game/Conversion.h"
#include "util/CharacterQuery.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/item/power/ItemPowerStats.h"

namespace {
	FName VesselTrailSpeedEffectMagnitude(TEXT("VesselTrailSpeedEffectMagnitude"));
}

UVesselTrailDamageGameplayEffect::UVesselTrailDamageGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	//StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = true;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;
	Period = 0.2f;

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UDifficultyDamageModCalculation::StaticClass();
	healthMagnitude.Coefficient = Period.GetValueAtLevel(1);

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	const auto weakDamageTag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak"));
	InheritableGameplayEffectTags.AddTag(weakDamageTag);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Damage.Medium"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

AVesselBlock::AVesselBlock() {
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.2f;
}

void AVesselBlock::SetDamagePerSecond(float dps) {
	DamagePerSecond = dps;
}

void AVesselBlock::BeginPlay() {
	Super::BeginPlay();
	
	if (const auto characterOwner = Cast<ABaseCharacter>(GetOwner())) {
		if (characterOwner->HasAuthority()) {
			if (auto boxCollision = FindComponentByClass<UBoxComponent>()) {
				boxCollision->OnComponentBeginOverlap.AddDynamic(this, &AVesselBlock::OnComponentBeginOverlap);
				boxCollision->OnComponentEndOverlap.AddDynamic(this, &AVesselBlock::OnComponentEndOverlap);
			}			
		}
	}
}

void AVesselBlock::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (const auto capsule = Cast<UCapsuleComponent>(OtherComp)) {
		if (auto otherCharacter = Cast<ABaseCharacter>(OtherActor)) {
			if (CanDamage(otherCharacter)) {
				if (!CharactersInFire.Contains(otherCharacter)) {
					auto handle = ApplyDamageEffect(otherCharacter);
					CharactersInFire.Emplace(otherCharacter, handle);
				}
			}
		}
	}
}

void AVesselBlock::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (const auto capsule = Cast<UCapsuleComponent>(OtherComp)) {
		if (auto otherCharacter = Cast<ABaseCharacter>(OtherActor)) {
			if (CharactersInFire.Contains(otherCharacter)) {
				auto handle = CharactersInFire[otherCharacter];
				RemoveDamageEffect(otherCharacter, handle);
				CharactersInFire.Remove(otherCharacter);
			}
		}
	}
}

FActiveGameplayEffectHandle AVesselBlock::ApplyDamageEffect(ABaseCharacter* targetCharacter) const {
	if (const auto* characterOwner = Cast<ABaseCharacter>(GetOwner())) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		auto ownerAbilitySystem = characterOwner->GetAbilitySystemComponent();
		auto targetAbilitySystem = targetCharacter->GetAbilitySystemComponent();

		auto spec(effects::CreateGameplayEffectSpec<UVesselTrailDamageGameplayEffect>(ownerAbilitySystem));
		FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(spec);
		context->AddInstigator(GetOwner(), GetOwner());
		context->AddOrigin(GetActorLocation());
		spec.SetSetByCallerMagnitude(effects::HealthName, -DamagePerSecond);
		return ownerAbilitySystem->ApplyGameplayEffectSpecToTarget(spec, targetAbilitySystem);
	}
	return FActiveGameplayEffectHandle();
}

bool AVesselBlock::RemoveDamageEffect(ABaseCharacter* targetCharacter, FActiveGameplayEffectHandle handle) const {
	if (auto targetAbilitySystem = targetCharacter->GetAbilitySystemComponent()) {
		return targetAbilitySystem->RemoveActiveGameplayEffect(handle, 1);
	}
	return false;
}

void AVesselBlock::RemoveAllDamagedCharacters() {
	TArray<ABaseCharacter*> characters;
	CharactersInFire.GenerateKeyArray(characters);

	while (characters.Num()) {
		auto character = characters[0];
		auto handle = CharactersInFire[character];
		RemoveDamageEffect(character, handle);
		characters.Remove(character);
		CharactersInFire.Remove(character);
	}
}

bool AVesselBlock::CanDamage(ABaseCharacter* character) const {
	const auto characterOwner = Cast<ABaseCharacter>(GetOwner());
	const auto hostile = characterOwner ? characterquery::is::hostile(characterOwner) : [](const ABaseCharacter* arg) { return true; };

	const auto predicate = [=](const ABaseCharacter* targetCandidate) {
		return  hostile(targetCandidate) &&
			characterquery::is::targetable(targetCandidate) &&
			actorquery::is::alive(targetCandidate);

		return false;
	};

	return predicate(character);
}

void AVesselBlock::OnDisableFireDamage() {
	DisableOverlaps();
	RemoveAllDamagedCharacters();
}

void AVesselBlock::DisableOverlaps() const {
	if (auto boxCollision = FindComponentByClass<UBoxComponent>()) {
		boxCollision->SetGenerateOverlapEvents(false);
	}
}

void AVesselBlock::Kill(float multiplier) {
	SetLifeSpan(killDelay * multiplier);
	OnKill(killDelay * multiplier);
}

void AVesselBlock::OnKill_Implementation(float) {}

UVesselTrail::UVesselTrail() {
	TypeId = EEnchantmentTypeID::VesselTrail;
}

void UVesselTrail::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (GetOwner() && GetOwner()->HasAuthority()) {
		const auto* ownerCharacter = GetCharacterOwner();
		auto zOffset = 0.0f;
		if (const auto* capsule = ownerCharacter->FindComponentByClass<UCapsuleComponent>()) {
			zOffset = capsule->GetScaledCapsuleHalfHeight();
		}
		const auto& ownerLocation = ownerCharacter->GetActorLocation();
		const auto currentBlockPos = conversion::ueToBlock(FVector(ownerLocation.X, ownerLocation.Y, ownerLocation.Z - zOffset));
		if (currentBlockPos != lastBlockPos) {
			OnExitedBlock(lastBlockPos);
			lastBlockPos = currentBlockPos;
		}
	}
}


void UVesselTrail::OnStart() {
	Super::OnStart();
	if (GetOwner()->HasAuthority()) {
		StartFireSpawning();
	}
}

void UVesselTrail::OnEnd() {
	if(GetOwner()->HasAuthority()){
		if (bShouldSpawnFire) {
			StopFireSpawning();
		}
		RemoveAll();
	}
}

void UVesselTrail::OnExitedBlock(BlockPos blockPos) {
	if (bShouldSpawnFire) {
		if (blocks.Num() >= queueLengthMax) {
			if (const auto block = blocks[0]) {
				block->Kill();
				blocks.RemoveAt(0);
			}
		}
				
		blocks.Push(SpawnFireBlock(blockPos));
	}
}

void UVesselTrail::RemoveAll() {
	for (auto block : blocks) {
		block->Kill();
	}

	blocks.Empty();
}

void UVesselTrail::StartFireSpawning() {
	ApplySpeedEffect();
	bShouldSpawnFire = true;
	if (TrailTimerHandle.IsValid() && GetWorld()->GetTimerManager().IsTimerActive(TrailTimerHandle)) {
		GetWorld()->GetTimerManager().ClearTimer(TrailTimerHandle);
	}
}

void UVesselTrail::StopFireSpawning() {
	RemoveSpeedEffect();
	bShouldSpawnFire = false;
}

AVesselBlock* UVesselTrail::SpawnFireBlock(BlockPos blockPos) const {
	const FTransform transform { conversion::blockToUe(blockPos) };
		
	if (AVesselBlock* fireBlock = GetWorld()->SpawnActorDeferred<AVesselBlock>(VesselBlockClass, transform, GetOwner(), nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn)) {
		fireBlock->SetDamagePerSecond(BaseDamagePerSecond);
		fireBlock->FinishSpawning(transform);
		return fireBlock;
	}
	
	return nullptr;
}

void UVesselTrail::ApplySpeedEffect() {
	if (const auto* characterOwner = GetCharacterOwner()) {
		auto abilitySystem = characterOwner->GetAbilitySystemComponent();

		auto spec(effects::CreateGameplayEffectSpec<UVesselTrailSpeedGameplayEffect>(abilitySystem, Level));		
		SpeedEffectHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UVesselTrail::RemoveSpeedEffect() const {
	if (const auto* characterOwner = GetCharacterOwner()) {
		auto abilitySystem = characterOwner->GetAbilitySystemComponent();

		abilitySystem->RemoveActiveGameplayEffect(SpeedEffectHandle);
	}
}

UVesselTrailSpeedGameplayEffect::UVesselTrailSpeedGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());
}
