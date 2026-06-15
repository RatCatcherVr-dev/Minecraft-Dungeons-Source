// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "SoulComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/item/instance/AItemInstance.h"
#include "game/affector/Affectors.h"
#include "game/component/EquipmentComponent.h"
#include "UnrealNetwork.h"
#include "game/actor/item/Soul.h"
#include "game/item/instance/MeleeWeaponGearItemInstance.h"
#include "game/item/instance/RangedWeaponGearItemInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include "game/item/instance/GearItemInstance.h"
#include "game/item/instance/TormentQuiver.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemComponent.h>
#include "gamemodes/LobbyGameMode.h"
#include "game/actor/ProjectileManager.h"

UAddPlainSoulModCalculation::UAddPlainSoulModCalculation()
{

}

float UAddPlainSoulModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	return 1.0f;
}

UAddSoulModCalculation::UAddSoulModCalculation()
 : SoulGatheringCapture(UItemAttributeSet::SoulGatheringAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
 , SoulGatheringMultiplierCapture(UItemAttributeSet::SoulGatheringMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Target, true)
{
	RelevantAttributesToCapture.Emplace(SoulGatheringCapture);
	RelevantAttributesToCapture.Emplace(SoulGatheringMultiplierCapture);
}

float UAddSoulModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float soulGathering = 0.f;
	float soulGatheringMultiplier = 1.f;


	const FAggregatorEvaluateParameters params;
	GetCapturedAttributeMagnitude(SoulGatheringCapture, Spec, params, soulGathering);
	GetCapturedAttributeMagnitude(SoulGatheringMultiplierCapture, Spec, params, soulGatheringMultiplier);

	return soulGathering * soulGatheringMultiplier;
}

UBaseGenerateSoulsGameplayEffect::UBaseGenerateSoulsGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("Soul.Absorb"));
}

UGenerateSoulsGameplayEffect::UGenerateSoulsGameplayEffect() {

	FCustomCalculationBasedFloat souls;
	souls.CalculationClassMagnitude = UAddSoulModCalculation::StaticClass();

	FGameplayModifierInfo soulInfo;
	soulInfo.Attribute = UItemAttributeSet::SoulsAttribute();
	soulInfo.ModifierMagnitude = souls;
	soulInfo.ModifierOp = EGameplayModOp::Additive;

	Modifiers.Add(soulInfo);
	
}

UModifySoulsGameplayEffect::UModifySoulsGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FSetByCallerFloat souls;
	souls.DataName = SoulKey;

	FGameplayModifierInfo soulInfo;
	soulInfo.Attribute = UItemAttributeSet::SoulsAttribute();
	soulInfo.ModifierMagnitude = souls;
	soulInfo.ModifierOp = EGameplayModOp::Additive;

	Modifiers.Add(soulInfo);
}

const FName UModifySoulsGameplayEffect::SoulKey("Souls");

USoulComponent::USoulComponent() {
	bReplicates = true;
}

void USoulComponent::ServerAddSouls_Implementation(int amount) {
	AddSouls(amount);
}
bool USoulComponent::ServerAddSouls_Validate(const int amount) {
	return true;
}

int USoulComponent::GetSoulCount() const {
	return GetAbilitySystemComponent()->GetNumericAttribute(UItemAttributeSet::SoulsAttribute());
}

int USoulComponent::GetMaxSoulCount() const {
	return GetAbilitySystemComponent()->GetNumericAttribute(UItemAttributeSet::MaxSoulsAttribute());
}

float USoulComponent::GetSoulPercentage() const {
	const float maxSouls = GetMaxSoulCount();

	return maxSouls > 0.f ? GetSoulCount() / maxSouls : 0.f;
}

void USoulComponent::TriggerSoulsNeededHint(int soulCount) const {
	OnSoulCountNeededHint.Broadcast(soulCount, static_cast<float>(soulCount) / static_cast<float>(GetMaxSoulCount()));
}

void USoulComponent::OnSoulCountChanged(const FOnAttributeChangeData& data) {
	const int delta = FMath::RoundToInt(data.NewValue - mLastSeenSoulCount);
	if(delta != 0){
		OnSoulCountDelta.Broadcast(delta);
		OnSoulPercentageChanged.Broadcast(this);
	}
	mLastSeenSoulCount = data.NewValue;

	if (mLastSeenSoulCount == 0) {
		if (ALobbyGameMode* gameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode())) {
			FTimerHandle handle;
			TWeakObjectPtr<USoulComponent> Weakthis = this;
			GetWorld()->GetTimerManager().SetTimer(handle, [Weakthis] { if (Weakthis.IsValid()) Weakthis->ServerAddSouls(Weakthis->GetMaxSoulCount()); }, gameMode->GetSoulReplenishTime(), false);
		}
	}
}

void USoulComponent::OnMaxSoulCountChanged(const FOnAttributeChangeData&) {
	OnSoulPercentageChanged.Broadcast(this);
}

void USoulComponent::OnSoulGatheringChanged(const FOnAttributeChangeData &) {
	UpdateIsCollecting();
}

ABaseCharacter* USoulComponent::GetCharacterOwner() const {
	return Cast<ABaseCharacter>(GetOwner());
}

UAbilitySystemComponent* USoulComponent::GetAbilitySystemComponent() const {
	return GetCharacterOwner()->GetAbilitySystemComponent();
}

void USoulComponent::UpdateIsCollecting() {
	bool isCollecting = IsCollecting();
	if (mWasCollecting == isCollecting) {
		return;
	}

	mWasCollecting = isCollecting;
	if (isCollecting) {
		OnSoulCollectingStarted.Broadcast(this);
		if (auto gameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode())) {
			if(!gameMode->OnPlayerKilledMob.IsBoundToObject(this)){
				gameMode->OnPlayerKilledMob.AddUObject(this, &USoulComponent::OnAnyPlayerKilledMob);
			}
		}
	}
	else {
		OnSoulCollectingStopped.Broadcast(this);
		if (auto gameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode())) {			
			gameMode->OnPlayerKilledMob.RemoveAll(this);
		}
	}	
}

void USoulComponent::BeginPlay() {
	Super::BeginPlay();

	auto bc = GetCharacterOwner();

	auto OwnerAbilitySystem = GetAbilitySystemComponent();

	if (bc->IsLocallyControlled() || GetOwnerRole() == ROLE_Authority) {
		OwnerAbilitySystem->GetGameplayAttributeValueChangeDelegate(UItemAttributeSet::SoulsAttribute()).AddUObject(this, &USoulComponent::OnSoulCountChanged);
		OwnerAbilitySystem->GetGameplayAttributeValueChangeDelegate(UItemAttributeSet::MaxSoulsAttribute()).AddUObject(this, &USoulComponent::OnMaxSoulCountChanged);
		OwnerAbilitySystem->GetGameplayAttributeValueChangeDelegate(UItemAttributeSet::SoulGatheringAttribute()).AddUObject(this, &USoulComponent::OnSoulGatheringChanged);
		OwnerAbilitySystem->GetSet<UItemAttributeSet>()->OnAttemptChangeSouls.BindUObject(this, &USoulComponent::OnAttemptChangeSouls);
		UpdateIsCollecting();
	}
}

void USoulComponent::EndPlay(const EEndPlayReason::Type endPlayReason) {
	Super::EndPlay(endPlayReason);
	
	if (auto gameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode())) {
		gameMode->OnPlayerKilledMob.RemoveAll(this);
	}
}

void USoulComponent::ServerSpawnSoul(const FVector& spawnPosition, const int amount, float riseTimeMin/* = 1.0f*/, float riseTimeMax/* = 1.0f*/) {
	FGameplayEffectSpec soulGatherSpec = effects::CreateGameplayEffectSpec<UGenerateSoulsGameplayEffect>(GetAbilitySystemComponent());

	ServerSpawnSoulInternal(spawnPosition, soulGatherSpec, amount, riseTimeMin, riseTimeMax);
}

void USoulComponent::ServerSpawnSoulWithBaseGE(TSubclassOf<UBaseGenerateSoulsGameplayEffect> generateSoulsGE, const FVector& spawnPosition, const int amount /*= 1*/, float riseTimeMin /*= 1.0f*/, float riseTimeMax /*= 1.0f*/)
{
	FGameplayEffectSpec soulGatherSpec = effects::CreateGameplayEffectSpecFromSubClass(GetAbilitySystemComponent(), generateSoulsGE);

	ServerSpawnSoulInternal(spawnPosition, soulGatherSpec, amount, riseTimeMin, riseTimeMax);
}

void USoulComponent::OnAttemptChangeSouls(const FGameplayEffectModCallbackData & data) {
	FGameplayTagContainer tags;
	data.EffectSpec.GetAllAssetTags(tags);

	static const FGameplayTag absorbtionTag = FGameplayTag::RequestGameplayTag("Soul.Absorb");

	if (tags.HasTag(absorbtionTag)) {
		OnAbsorbedSoul.Broadcast();
	}
}

void USoulComponent::ServerSpawnSoulInternal(const FVector& spawnPosition, FGameplayEffectSpec captureSpec, const int amount /* = 1 */, float riseTimeMin /* = 1.0f */, float riseTimeMax /* = 1.0f */) {
	ensure(GetOwnerRole() == ROLE_Authority);
	const auto soulTime = CalculateSoulTravelTime(spawnPosition, GetOwner()->GetActorLocation());
	const int finalAmount = amount * affector::get(GetWorld()).GetSoulCountMultiplier();

	FTimerHandle absorbTimeHandle;	
	GetWorld()->GetTimerManager().SetTimer(absorbTimeHandle, FTimerDelegate::CreateUObject(this, &USoulComponent::OnAbsorbSouls, captureSpec, finalAmount), soulTime, false);
	ClientSpawnSoul(spawnPosition, soulTime, finalAmount, riseTimeMin, riseTimeMax);
}

void USoulComponent::ClientSpawnSoul_Implementation(const FVector& spawnPosition, float travelTime, int amount, float riseTimeMin/* = 1.0f*/, float riseTimeMax/* = 1.0f*/) {
	SpawnSoul(spawnPosition, travelTime, FRandomStream(FMath::Rand()), amount, FloatRange(riseTimeMin, riseTimeMax));
}

void USoulComponent::SpawnSoul(const FVector& spawnPosition, float travelTime, const FRandomStream& rand, int amount, FloatRange riseTime) {
	for (int i = 0; i < amount; ++i) 
	{
		FTransform spawnTransform;
		spawnTransform.SetLocation(spawnPosition);
			
		auto soul = AProjectileActorManager::Pop_Soul(SoulClass, spawnTransform, GetWorld());

		FVector riseToPosition = FVector(spawnPosition.X, spawnPosition.Y, spawnPosition.Z + 200.0f);
		if (amount > 1)
		{
			FVector offsetVector(rand.FRandRange(-300.0f, 300.0f), rand.FRandRange(-300.0f, 300.0f), 200.0f);
			riseToPosition += offsetVector;
		}
		soul->SetOwner(GetOwner());
		soul->Initialize(spawnPosition, riseToPosition, travelTime, riseTime.random());

		soul->LaunchSoul();
	}
		
}

float USoulComponent::CalculateSoulTravelTime(const FVector& startPosition, const FVector& endPosition) {
	const float baseTime = 3.f;
	const float maxTime = 6.f;
	const float maxDistance = 3000.f;

	const float distance = (endPosition - startPosition).Size();
	auto distanceFraction = FMath::Clamp(distance, 0.f, maxDistance) / maxDistance;

	return baseTime + ((maxTime - baseTime) * FMath::Pow(distanceFraction, 0.5f));
}

void USoulComponent::OnAbsorbSouls(FGameplayEffectSpec spec, int amount) {
	auto abilitySystem = GetAbilitySystemComponent();
	
	for (int i = 0; i < amount; ++i) {
		abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void USoulComponent::AddSouls(int amount) {
	auto abilitySystem = GetAbilitySystemComponent();
	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UModifySoulsGameplayEffect>(abilitySystem);
	spec.SetSetByCallerMagnitude(UModifySoulsGameplayEffect::SoulKey, amount);
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void USoulComponent::SpawnSoulWrapper(TargetProvider target, FGameplayEffectSpec captureSpec, FRandomStream rand, const int amount /* = 1 */, float riseTimeMin/* = 1.0f*/, float riseTimeMax/* = 1.0f*/) {
	ServerSpawnSoulInternal(target(), captureSpec, amount, riseTimeMin, riseTimeMax);
}

static const float MAX_SHARED_SOULRANGE_SQUARED = FMath::Square(1200.f);

void USoulComponent::OnAnyPlayerKilledMob(const AMobCharacter* mobKilled, AActor * byActor, AActor* byWhat) {
	if (!byWhat || !mobKilled) {
		return;
	}

	const auto owner = Cast<APlayerCharacter>(GetOwner());

	if (!owner) {
		return;
	}

	check(IsCollecting() && "OnAnyPlayerKilledOther should not be bound if we are not collecting");
	if (!IsCollecting()) {
		return;
	}
		
	if (const auto itemInstance = Cast<AItemInstance>(byWhat)) {
		const auto& itemType = itemInstance->GetItemType();
		if (itemType.isSoulGatherItem() && itemType.hasTag(ItemTag::Item)) {
			return;
		}
	}

	if (byWhat->IsA(ATormentProjectile::StaticClass())) {
		//Torment projectiles can never gather souls.
		return;
	}
	
	if (mobKilled->NoSouls) {
		return;
	}
	
	if (!owner->IsHostileTowards(mobKilled)) {
		return;
	}	

	//You should get souls from your own kills.
	//You should get from from kills by other player kills and pets:
	//  * While the killing player is close to yourself.
	//  * If the killed mob is close to yourself.
	if (owner != byActor &&
		(actorquery::getActorDistanceSquared2D(byActor, owner) > MAX_SHARED_SOULRANGE_SQUARED && actorquery::getActorDistanceSquared2D(mobKilled, owner) > MAX_SHARED_SOULRANGE_SQUARED))
	{
		return;
	}

	FGameplayEffectSpec soulGatherSpec = effects::CreateGameplayEffectSpec<UGenerateSoulsGameplayEffect>(GetAbilitySystemComponent());
	TWeakObjectPtr<AActor> actor = TWeakObjectPtr<AActor>(Cast<AActor>(mobKilled));
	FVector fallback = actor->GetActorLocation();

	FTimerHandle spawnSoulTimer;
	FTimerDelegate spawnDelegate;

	spawnDelegate.BindUObject(this, &USoulComponent::SpawnSoulWrapper, (TargetProvider)[actor, fallback]() { return actor.IsValid() ? actor->GetActorLocation() : fallback; }, soulGatherSpec, FRandomStream(FMath::Rand()), 1, 1.0f, 2.0f);
	GetWorld()->GetTimerManager().SetTimer(spawnSoulTimer, spawnDelegate, 1.0f, false);

	OnAnyMobKilled.Broadcast(byActor);
}

bool USoulComponent::IsCollecting() const {
	return GetAbilitySystemComponent()->GetNumericAttribute(UItemAttributeSet::SoulGatheringAttribute()) > 0.f;
}