#include "Dungeons.h"
#include "TotemOfSpiritProtection.h"
#include "game/component/SoulComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/component/AreaBuffComponent.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/MoveToTargetMovementComponent.h"
#include "game/component/ReviveComponent.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

UTotemOfSpiritProtectionGameplayEffect::UTotemOfSpiritProtectionGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.TotemOfSpiritProtection"), 0, 1);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("ForcedDownState")));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("PersistAfterDeath")));
}


UTotemOfSpiritProtectionReviveGameplayEffect::UTotemOfSpiritProtectionReviveGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthChange;
	healthChange.CalculationClassMagnitude = UNegativeHealthModCalculation::StaticClass();

	FGameplayModifierInfo info;
	info.Attribute = UHealthAttributeSet::HealthAttribute();
	info.ModifierMagnitude = healthChange;
	Modifiers.Add(info);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Revive")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Revive.Downed"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

ATotemOfSpiritProtectionActor::ATotemOfSpiritProtectionActor() {
	Effect = UTotemOfSpiritProtectionGameplayEffect::StaticClass();
}

void ATotemOfSpiritProtectionActor::BeginPlay() {
	Super::BeginPlay();
	if (!HasAuthority()) {
		return;
	}
	auto players = actorquery::getActors<APlayerCharacter>(GetWorld());
	for (auto player : players) {
		player->OnPlayerDown.AddUObject(this, &ATotemOfSpiritProtectionActor::OnPlayerDown, player);
	}
}

void ATotemOfSpiritProtectionActor::PreBuffComponentBeginPlay(UAreaBuffComponent* BuffComponent) {
	BuffComponent->Attributes.Emplace(UHealthAttributeSet::HealthAttribute(), 1.0f);
}

void ATotemOfSpiritProtectionActor::OnPlayerDown(APlayerCharacter* player) {
	if (!ReviveActor) {
		auto characters = OwnerAreaBuffComponent->GetOverlappingCharacters();
		auto players = characters.FilterByPredicate(RETLAMBDA(it->IsA(APlayerCharacter::StaticClass())));
		if (players.Find(player) != INDEX_NONE) {
			MulticastSpawnReviveActor(player, GetActorLocation());
		}
	}
}

void ATotemOfSpiritProtectionActor::MulticastSpawnReviveActor_Implementation(APlayerCharacter* targetPlayer, const FVector& spawnLocation) {
	if (!targetPlayer || ReviveActor)
		return;

	FTransform spawnTransform;
	spawnTransform.SetLocation(spawnLocation);
	ReviveActor = GetWorld()->SpawnActorDeferred<AReviveOnOverlapActor>(ReviveActorClass, spawnTransform, GetOwner(), Cast<APawn>(GetOwner()), ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	ReviveActor->TargetPlayer = targetPlayer;
	ReviveActor->HealthAmount = HealthAmount;

	if (!ReviveActor->OnRevivedPlayer.IsBound()) {
		ReviveActor->OnRevivedPlayer.AddUObject(this, &ATotemOfSpiritProtectionActor::OnPlayerRevived);
	}
	UGameplayStatics::FinishSpawningActor(ReviveActor, spawnTransform);
}

void ATotemOfSpiritProtectionActor::OnPlayerRevived(APlayerCharacter* revivedPlayer) {
	TryStartDestroyCountdown();
	auto players = actorquery::getActors<APlayerCharacter>(GetWorld());
	for (auto player : players) {
		player->OnPlayerDown.RemoveAll(this);
	}
}

AReviveOnOverlapActor::AReviveOnOverlapActor() {
	Overlap = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Overlap->SetSphereRadius(5.f);
	Movement = CreateDefaultSubobject<UMoveToTargetMovementComponent>(TEXT("Movement"));
}

void AReviveOnOverlapActor::BeginPlay() {
	Super::BeginPlay();
	if (TargetPlayer.IsValid()) {
		Movement->SetTarget(TargetPlayer.Get());
	}
	else {
		Destroy();
	}
}

void AReviveOnOverlapActor::NotifyActorBeginOverlap(AActor* OtherActor) {
	if (auto player = Cast<APlayerCharacter>(OtherActor)) {
		if ((OtherActor && TargetPlayer.IsValid()) && OtherActor == TargetPlayer) {
			OnRevive(TargetPlayer.Get());
			Destroy();
		}
	}
}

void AReviveOnOverlapActor::OnRevive_Implementation(APlayerCharacter* player) {
	auto abilitySystem = player->GetAbilitySystemComponent();
	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	auto spec = effects::CreateGameplayEffectSpec<UTotemOfSpiritProtectionReviveGameplayEffect>(abilitySystem, 1);
	spec.SetSetByCallerMagnitude(effects::HealthName, HealthAmount);
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

	OnRevivedPlayer.Broadcast(player);
}

ATotemOfSpiritProtectionInstance::ATotemOfSpiritProtectionInstance() {
	PowerEffects = { UHealingIncrease::StaticClass() };
}

float ATotemOfSpiritProtectionInstance::GetStats(EItemStats stat) const {
	switch (stat) {
	case EItemStats::LowestHealthHealed:
	case EItemStats::HighestHealthHealed:
		return 125.0f;
	}
	return -1;
}

void ATotemOfSpiritProtectionInstance::PreTotemBeginPlay(ATotemBaseActor* totemActor) {
	if (auto* spiritProtectionTotem = Cast<ATotemOfSpiritProtectionActor>(totemActor)) {
		spiritProtectionTotem->HealthAmount = 125.0f * GetPowerEffect(0)->GetMultiplier(ItemPower);
	}
}
