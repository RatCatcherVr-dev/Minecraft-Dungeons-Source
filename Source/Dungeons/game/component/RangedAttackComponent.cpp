#include "Dungeons.h"
#include "RangedAttackComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/ItemSlot.h"
#include "game/item/instance/BurningOilVialInstance.h"
#include "game/item/instance/RangedWeaponGearItemInstance.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "DungeonsGameMode.h"
#include <AbilitySystemComponent.h>
#include "EquipmentComponent.h"
#include <Animation/AnimInstance.h>
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/affector/Affectors.h"
#include "game/item/ArrowItemSlot.h"
#include "DungeonsGameInstance.h"
#include "AbilitySystemGlobals.h"
#include "EnchantmentComponent.h"
#include "gamemodes/LobbyGameMode.h"
#include "game/ArmorProperties/ArmorPropertiesComponent.h"
#include "game/util/DungeonsGearUtilLibrary.h"
#include "game/actor/ProjectileManager.h"
#include "game/ActorShake.h"
#include "game/actor/ActorShakeManager.h"

FProjectileSpawnData::FProjectileSpawnData(float speed, AActor* target, const FItemId& id, const FVector& location,
	const FVector& direction, uint8 count, int32 inSeed)
	: rangedAttackSpeedMultiplier(speed),  attackTarget(target), ammoType(id), spawnLocation(location), spawnDirection(direction), arrowCountAtStart(count), seed(inSeed) {
}

bool FProjectileSpawnData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) {
	uint8 RepBits = 0;

	if (Ar.IsSaving()) {
		if (!FMath::IsNearlyEqual(rangedAttackSpeedMultiplier, 1.f)) {
			RepBits |= AttackSpeedFlag;
		}
		if (attackTarget) {
			RepBits |= AttackTargetFlag;
		}
		if (game::item::type::Arrow.getId() != ammoType) {
			RepBits |= AmmoTypeFlag;
		}
	}

	Ar.SerializeBits(&RepBits, 3);

	if (RepBits & AttackSpeedFlag) {
		if (Ar.IsLoading()) {
			ReadFixedCompressedFloat<8, 16>( rangedAttackSpeedMultiplier, Ar);
		} else {
			WriteFixedCompressedFloat<8, 16>( rangedAttackSpeedMultiplier, Ar);
		}
	}

	if (RepBits & AttackTargetFlag) {
		Ar << attackTarget;
	}

	if (RepBits & AmmoTypeFlag) {
		ammoType.NetSerialize(Ar, Map, bOutSuccess);
	}

	spawnLocation.NetSerialize(Ar, Map, bOutSuccess);
	spawnDirection.NetSerialize(Ar, Map, bOutSuccess);
	Ar << arrowCountAtStart;
	Ar << seed;

	bOutSuccess = true;
	return true;
}

bool FRangedAttack::IsAnyProjectileHoming(bool isCharged) const {
	return algo::any_of(isCharged ? ChargedProjectileDefinition : ProjectileDefinition, [](const FRangedAttackProjectileSpawnDescription& entry) { return entry.Projectile && entry.Projectile.GetDefaultObject()->IsHoming; });
}

void FRangedAttack::PreCacheProjectiles(UWorld* world) const
{
	for (const auto& AttackDesc : ProjectileDefinition)
	{
		if (AttackDesc.Projectile)
		{
			AProjectileActorManager::PreCache_ProjectileClass(AttackDesc.Projectile, world);
		}
	}

	for (const auto& AttackDesc : ChargedProjectileDefinition)
	{
		if (AttackDesc.Projectile)
		{
			AProjectileActorManager::PreCache_ProjectileClass(AttackDesc.Projectile, world);
		}
	}

}

URangedAttackComponent::URangedAttackComponent() {
	SetIsReplicated(true);
}

float URangedAttackComponent::GetAttackRange() const {
	return RangedAttackDefinition.Range;
}


float URangedAttackComponent::GetAttackTimeTotal() const {
	float total = 0;
	for (const auto& projectile : RangedAttackDefinition.GetCurrentProjectileDefinitions(IsCharged)) {
		total += projectile.GetTotalDelay();
	}

	return total;
}

float URangedAttackComponent::GetBiggestAttackAngleSpan() const {
	float biggest = 0;
	for (const auto& projectile : RangedAttackDefinition.GetCurrentProjectileDefinitions(IsCharged)) {
		biggest = FMath::Max(projectile.AngleSpan, biggest);
	}

	return biggest;
}

bool URangedAttackComponent::HasAmmo() const {
	return AvailableAmmoSlots.Num() <= 0 || (AmmoSlot.IsValid() && AmmoSlot->GetCount() > 0);
}

bool URangedAttackComponent::CanAttack(AActor* attackTarget /* = nullptr */) const {
	return AlwaysAttack || (HasAmmo() && Super::CanAttack(attackTarget));
}

void URangedAttackComponent::SetAmmoSlot(TWeakObjectPtr<UItemSlot> slot) {
	if (AmmoSlot != slot) {
		if (auto arrowslot = Cast<UArrowItemSlot>(AmmoSlot.Get())) {
			arrowslot->SetAsInactiveArrowSlot();
		}
		AmmoSlot = slot;
		if (auto arrowslot = Cast<UArrowItemSlot>(AmmoSlot.Get())) {
			arrowslot->SetAsActiveArrowSlot();
		}
		OnAmmoSlotChanged.Broadcast(slot.Get());
	}
}

void URangedAttackComponent::AmmoSlotChangedCount(const UItemSlot* slot) {
	DetermineAmmoSlot();
}

void URangedAttackComponent::AmmoSlotChangedCount(UItemSlot* slot){
	DetermineAmmoSlot();
}

void URangedAttackComponent::DetermineAmmoSlot() {
	//Try to find the first ammo slot with any ammo.		
	for ( auto slot : AvailableAmmoSlots ) {
		if (slot.IsValid() && slot->GetCount() > 0 && (slot->GetItem() || IsThrowableSlot(slot->SlotTypeId))) {
			SetAmmoSlot(slot);
			return;
		}
	}

	//Fallback to using the last ammo type - even if zero.
	auto NumberOfAmmoSlots = AvailableAmmoSlots.Num();
	if (NumberOfAmmoSlots > 0) {
		auto lastSlot = AvailableAmmoSlots[NumberOfAmmoSlots - 1];
		if(lastSlot.IsValid()) {
			SetAmmoSlot(lastSlot);
		}
	}
}

void URangedAttackComponent::PreCacheProjectileClasses()
{
	UWorld* pWorld = GetWorld();
	if (pWorld)
	{
		DefaultRangedAttack.PreCacheProjectiles(pWorld);

		for (auto& RangedAttackVar : AlternateRangedAttacks)
		{
			RangedAttackVar.PreCacheProjectiles(pWorld);
		}
	}
	
}

void URangedAttackComponent::BeginPlay() {
	Super::BeginPlay();

	ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
	if (!owner->IsA<APlayerCharacter>())
	{
		//lets see if we can push our projectile types for pre-caching
		PreCacheProjectileClasses();
	}
	else
	{
		if(ABasePlayerController* playerController = Cast<APlayerCharacter>(owner)->GetPlayerController())
		{
			playerController->OnAlternativeRangeAttack.BindUObject(this, &URangedAttackComponent::LaunchHeldThrowablePlayerForwardDir);
		}
	}

	RangedAttackDefinition = DefaultRangedAttack;
	
	UEquipmentComponent* equipmentComponent = GetOwner()->FindComponentByClass<UEquipmentComponent>();
	if (equipmentComponent) {
		if (AmmoTypes.Num() > 0) {
			for (auto ammoSlotType : AmmoTypes) {
				auto slots = equipmentComponent->GetSlotsOfType(ammoSlotType);
				if (slots.Num() > 0){
					auto const slot = slots[0];
					AvailableAmmoSlots.Add(slot);
					slot->OnReplicatedCountInteral.AddUObject(this, &URangedAttackComponent::AmmoSlotChangedCount);
					slot->OnItemInstanceReplicatedInternal.AddUObject(this, &URangedAttackComponent::AmmoSlotChangedCount);
				}
			}
			OnAvailableAmmoSlotsChanged.Broadcast();
		}				

		TArray<UItemSlot*> rangedSlot(equipmentComponent->GetSlotsOfType(ESlotType::RangedWeapon));
		if (rangedSlot.Num()) {
			rangedSlot[0]->OnItemInstanceReplicatedInternal.AddUObject(this, &URangedAttackComponent::OnWeaponSlotUpdated);
			OnWeaponSlotUpdated(rangedSlot[0]);
		}

		DetermineAmmoSlot(); //Determine which ammo to use
	}

	if(auto character = Cast<ABaseCharacter>(GetOwner())) {
		character->CancelActions.AddUObject(this, &URangedAttackComponent::StopAttack);
	}

	if (auto abilitySystem = owner->GetAbilitySystemComponent()) {
		auto delegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &URangedAttackComponent::OnAutoChargeChanged);
		abilitySystem->RegisterAndCallGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Ranged.AutoCharge")), delegate, EGameplayTagEventType::AnyCountChange);
	}
}

void URangedAttackComponent::EndPlay(const EEndPlayReason::Type EndPlayReason){	
	AvailableAmmoSlots.Reset();
	OnAvailableAmmoSlotsChanged.Broadcast();
}

UItemSlot* URangedAttackComponent::GetCurrentAmmoSlot() const {
	return AmmoSlot.Get();
}

void URangedAttackComponent::OnRangedDamageDealt(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation) {
	if (bRemoveInvisibilityAfterAttack)
	{
		if (auto attacker = Cast<ABaseCharacter>(GetOwner())) {
			attacker->RemoveInvisibility();
		}
	}
}

void URangedAttackComponent::OverrideOrigin(FVector origin) {
	overrideOrigin = origin;
}

void URangedAttackComponent::ResetOrigin() {
	overrideOrigin.Reset();
}

void URangedAttackComponent::PerformAttack(TWeakObjectPtr<AActor> attackTarget, ABaseCharacter* attacker, FItemId ammoType, float rangedAttackSpeedMultiplier, int32 seed) {
	auto attackerEmitLocation = overrideOrigin.Get(attacker->GetActorLocation());

	auto attackDirection = GetAttackerBaseEmitVector(attacker);

	auto overrideDefinition = RangedWeapon ? RangedWeapon->GetProjectileOverrideForItemType(ammoType) : nullptr;
	SpawnProjectileMulticast({rangedAttackSpeedMultiplier, attackTarget.Get(), ammoType, attackerEmitLocation, attackDirection, AuthorativeArrowCount, seed}, IsCharged);
	OnBeginRangedAttack.Broadcast();
}

void URangedAttackComponent::AttackCpp(AActor* attackTarget, int32 seed /*= 0*/, FSharedPredictionContext predictionContext) {
	auto attacker = Cast<ABaseCharacter>(GetOwner());

	bool attackDenied = false;

	bRemoveInvisibilityAfterAttack = true;

	DungeonsGearUtilLibrary::OnBeforeRangedAttack(attacker, attackTarget, attackDenied, predictionContext.GetKey());

	if (attackDenied) {
		return;
	}

	auto abilitySystem = attacker->GetAbilitySystemComponent();
	float rangedAttackSpeedMultiplier = abilitySystem->GetNumericAttribute(URangedAttributeSet::RangedAttackSpeedMultiplierAttribute());

	FItemId ammoType = AmmoSlot.IsValid() && AmmoSlot->GetCount() > 0 && AmmoSlot->GetItem() ? static_cast<FItemId>(AmmoSlot->GetItem()->GetItemId()) : game::item::type::Arrow.getId();

	float worldTime = GetWorld()->GetTimeSeconds();

	if (DelayBeginChargeHandle.IsValid()) {
		GetWorld()->GetTimerManager().ClearTimer(DelayBeginChargeHandle);
	}
	
	lastAttackTimestampSeconds = worldTime;
	const auto& itemType = GetItemRegistry().Get(ammoType);
	
	float attackDuration = RangedAttackDefinition.AttackRateSeconds / rangedAttackSpeedMultiplier;
	float ammoCooldown = GetItemRegistry().Get(ammoType).getCoolDownSeconds();

	//Only further ranged attacks get increased cooldown from ammo cooldowns.
	canAttackFromSecond = attackDuration + worldTime + ammoCooldown;

	GetWorld()->GetTimerManager().SetTimer(AttackDurationTimerHandle, this, &URangedAttackComponent::OnAttackDurationPassed, attackDuration + ammoCooldown);

	cachedOwner->SetSharedCooldown(attackDuration * activeCancelPoint * RangedAttackDefinition.ActiveCancelPorintFactor, attackDuration * passiveCancelPoint);

	///jryden rebase: not used on nether branch?
	//DungeonsGearUtilLibrary::OnBeforeRangedAttack(attacker, attackTarget, predictionContext.GetKey());

	if (RangedWeapon && IsCharged) {
		FGameplayCueParameters params;
		params.Instigator = attacker;
		abilitySystem->ExecuteGameplayCue(RangedWeapon->GetChargeShootEffectTag(), params);
	}

	if (attacker->HasAuthority()) {
		if (UAnimMontage* montage = Cast<UAnimMontage>(RangedAttackDefinition.Animation)) {
			attacker->MulticastJumpToSectionMontageWithPlayrate(IsCharged ? FName("FireCharged") : FName("Fire"), rangedAttackSpeedMultiplier, montage, predictionContext.GetKey());
		} else {
			attacker->MulticastPlayAnimationAsDynamicMontage(RangedAttackDefinition.Animation, RangedAttackDefinition.Slot, 0.f, 0.2f, rangedAttackSpeedMultiplier, 1, 0.f, 0.f, predictionContext.GetKey());
		}

		const FRangedAttackProjectileSpawnDescription& projectileDefinition = RangedAttackDefinition.GetCurrentProjectileDefinitions(IsCharged)[0];

		if (projectileDefinition.DelaySeconds > 0.f) {
			FTimerManager& timerManager = GetOwner()->GetWorld()->GetTimerManager();
			timerManager.SetTimer(ProjectileSpawnHandle, FTimerDelegate::CreateUObject(this, &URangedAttackComponent::PerformAttack, TWeakObjectPtr<AActor>(attackTarget), attacker, ammoType, rangedAttackSpeedMultiplier, FMath::Rand()), projectileDefinition.DelaySeconds / rangedAttackSpeedMultiplier, false);
		} else {
			PerformAttack(attackTarget, attacker, ammoType, rangedAttackSpeedMultiplier, FMath::Rand());
		}
	} else {
		if (UAnimMontage* montage = Cast<UAnimMontage>(RangedAttackDefinition.Animation)) {
			attacker->JumpToSectionMontageWithPlayrate(IsCharged ? FName("FireCharged") : FName("Fire"), rangedAttackSpeedMultiplier, montage);
		} else {
			attacker->PlayAnimationAsDynamicMontage(RangedAttackDefinition.Animation, RangedAttackDefinition.Slot, 0.f, 0.2f, rangedAttackSpeedMultiplier, 1, 0.f, 0.f);
		}
	}
}

void URangedAttackComponent::SpawnProjectileMulticast_Implementation(const FProjectileSpawnData& spawnData, bool isCharged)
{
	ABaseCharacter* attacker = Cast<ABaseCharacter>(GetOwner());
	FPredictionKey key;
	TSubclassOf<ABaseProjectile> projectileOverride;
	TSharedPtr<FRandomStream> randstream = MakeShared<FRandomStream>(spawnData.seed);
	TemporaryAttackArrowCount = spawnData.arrowCountAtStart;

	if (RangedWeapon) {
		projectileOverride = RangedWeapon->GetProjectileOverrideForItemType(
			affector::get(GetWorld()).GetPlayerArrowType().Get(spawnData.ammoType));
	}

	
	if (RangedAttackDefinition.GetCurrentProjectileDefinitions(isCharged).Num() == 0) {
		return;
	}
	SpawnProjectileForIndex(0, AttackIndex, spawnData.attackTarget, spawnData.spawnLocation, spawnData.spawnDirection, isCharged, projectileOverride, spawnData.rangedAttackSpeedMultiplier, key, spawnData.ammoType, randstream);
}

bool URangedAttackComponent::IsAttackInProgress() const {
	auto& timerManager = GetOwner()->GetWorld()->GetTimerManager();
	if (ProjectileSpawnArcDelayHandle.IsValid() && timerManager.IsTimerActive(ProjectileSpawnArcDelayHandle)) {
		return true;
	}
	if (ProjectileSpawnHandle.IsValid() && timerManager.IsTimerActive(ProjectileSpawnHandle)) {
		return true;
	}
	return false;
}

void URangedAttackComponent::SpawnProjectileForIndex(int index, int attackIndex, AActor* attackTarget, FVector spawnLocation, FVector spawnDirection, bool isCharged, TSubclassOf<ABaseProjectile> projectileOverride, float attackSpeedMultiplier, FPredictionKey key, FItemId ammoType, TSharedPtr<FRandomStream> randstream) {
	if (index == 0) {
		AttackCounter++;
		SuccessfulAttackCounter++;

		if (auto mob = Cast<AMobCharacter>(GetOwner())) {
			mob->MobParams.totalAttacks.all++;
			mob->MobParams.totalAttacks.ranged++;
			mob->MobParams.successfulAttacks.all++;
			mob->MobParams.successfulAttacks.ranged++;
		}
	}

	if (attackIndex != AttackIndex) //D11.SC if we have a mismatch, set the definition to the calling type for the scope of this call
	{
		SetRangedAttackDefinitionVariant(attackIndex);
	}
	
	//redetermine location and direction for delayed projectiles
	if (RecalculatePerProjectile)
	{
		ABaseCharacter* attacker = Cast<ABaseCharacter>(GetOwner());
		if (!attacker->IsAlive())
		{			
			if (attackIndex != AttackIndex) //restore attack variant
			{
				SetRangedAttackDefinitionVariant(AttackIndex);
			}
			return; //stop attack early
		}
		auto abilitySystem = attacker->GetAbilitySystemComponent();
		if (abilitySystem && abilitySystem->HasAnyMatchingGameplayTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.Stunned")))) 
		{
			if (attackIndex != AttackIndex) //restore attack variant
			{
				SetRangedAttackDefinitionVariant(AttackIndex);
			}
			return; //stop attack early
		}
		
		spawnLocation = overrideOrigin.Get(attacker->GetActorLocation());
		spawnDirection = GetAttackerBaseEmitVector(attacker);
	}

	const auto& definitions = RangedAttackDefinition.GetCurrentProjectileDefinitions(isCharged);
	const FRangedAttackProjectileSpawnDescription& definition = definitions[index];

	ExecuteProjectileDefintion(index, 0, attackTarget, spawnLocation, spawnDirection, isCharged, projectileOverride, attackSpeedMultiplier, key, ammoType, randstream);
	float perDefinitionDelay = 0;

	index++;
	while (index < definitions.Num()) {
		const FRangedAttackProjectileSpawnDescription& nextDefinition = definitions[index];
		perDefinitionDelay = definition.DelaySecondsPerProjectile * Math::max(0, definition.NumberOfProjectiles - 1) + nextDefinition.DelaySeconds;

		if (perDefinitionDelay <= 0) {
			ExecuteProjectileDefintion(index, 0, attackTarget, spawnLocation, spawnDirection, isCharged, projectileOverride, attackSpeedMultiplier, key, ammoType, randstream);
			index++;
		}
		else {
			FTimerManager& timerManager = GetOwner()->GetWorld()->GetTimerManager();
			timerManager.SetTimer(ProjectileSpawnHandle, FTimerDelegate::CreateUObject(this, &URangedAttackComponent::SpawnProjectileForIndex, index, attackIndex, attackTarget, spawnLocation, spawnDirection, isCharged, projectileOverride, attackSpeedMultiplier, key, ammoType, randstream), perDefinitionDelay / attackSpeedMultiplier, false);
			break;
		}
	}

	if (attackIndex != AttackIndex) //restore attack variant
	{
		SetRangedAttackDefinitionVariant(AttackIndex);
	}
}

void URangedAttackComponent::SetRangedAttackDefinitionVariant(int attackIndex)
{
	if (attackIndex == -1) {
		RangedAttackDefinition = DefaultRangedAttack;
	}
	else if (AlternateRangedAttacks.IsValidIndex(attackIndex)) {
		RangedAttackDefinition = AlternateRangedAttacks[attackIndex];
	}
}

bool URangedAttackComponent::InAttackOrientation(AActor* attackTarget) const {	
	const auto targetCapsule = attackTarget ? attackTarget->FindComponentByClass<UCapsuleComponent>() : nullptr;
	const auto targetCapsuleRadius = targetCapsule != nullptr ? targetCapsule->GetScaledCapsuleRadius() : 1.0f;
	auto attacker = Cast<ABaseCharacter>(GetOwner());
	const auto source = attacker->GetActorLocation();
	const auto targetLocation = attackTarget->GetActorLocation();
	const auto distance = (targetLocation  - attacker->GetActorLocation()).Size2D();
	const auto forward = attacker->GetActorForwardVector();
	const auto maxAngleRadians = FMath::Atan2(targetCapsuleRadius, distance);	
	return actorquery::isInArc(source, forward, targetLocation, maxAngleRadians);
}


//Get the projectile speed for the specific projectile index of the current attack using the current ammo
float URangedAttackComponent::GetProjectileDefinitionSpeed(const int index, const FItemId& ammoType) const {
	const auto& definition = RangedAttackDefinition.GetCurrentProjectileDefinitions(IsCharged)[index];

	TSubclassOf<ABaseProjectile> projectileOverride;
	if (AmmoSlot.IsValid() && AmmoSlot->GetCount() > 0) {
		if (RangedWeapon) projectileOverride = RangedWeapon->GetProjectileOverrideForItemType(ammoType);
	}

	auto projectileDefinition = projectileOverride ? projectileOverride : definition.Projectile;

	const auto projectile = Cast<ABaseProjectile>(projectileDefinition->GetDefaultObject());

	auto speed = projectile->ProjectileMovementComponent->InitialSpeed;

	if (projectile->RespectWeaponProjectileVelocityFactor) {
		speed *= RangedAttackDefinition.ProjectileVelocityFactor;
	}
	if (IsCharged) {
		speed *= RangedAttackDefinition.ChargeVelocityMultiplier;
	}
	return speed;
}

//Get the average projectile speed of all projectiles in the definition using current ammo.
float URangedAttackComponent::GetEstimatedProjectileSpeed(const FItemId& ammoType) const {
	float totalSpeed = 0.0f;
	int num = RangedAttackDefinition.GetCurrentProjectileDefinitions(IsCharged).Num();
	for (int index = 0; index < num; index++) {
		totalSpeed += GetProjectileDefinitionSpeed(index, ammoType);
	}
	return totalSpeed / (float)(num);
}

FVector URangedAttackComponent::GetAttackerBaseEmitVector(ABaseCharacter* attacker) const {
	return attacker->GetActorForwardVector();
}

void URangedAttackComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(URangedAttackComponent, IsCharged);
	DOREPLIFETIME(URangedAttackComponent, AttackIndex);
	DOREPLIFETIME(URangedAttackComponent, HeldThrowables);
	DOREPLIFETIME(URangedAttackComponent, RangedAttackDefinition);
}

///For when shooting a projectile from a location, get a good random homing offset. We only home towards 'flat' XY locations, never Z offset.	
FVector URangedAttackComponent::CalculateHomingTargetOffset(const FVector& emitLocation, const AActor* target, TSharedPtr<FRandomStream> randStream) const {
	if (target) {
		FVector offset = [&]() {
			if (const auto targetCharacter = Cast<ACharacter>(target)) {
				if (const auto targetCapsule = targetCharacter->GetCapsuleComponent()) {
					const float maxRadius = targetCapsule->GetScaledCapsuleRadius() * 0.75f;
					//FRandRange claims it generates >= 0 and <= 360 but the function it eventually calls (FMath::Fractional) returns in range >= 0 and < 1.
					return FRotator(0, randStream.Get()->FRandRange(0.0f, 360.f), 0.0f).Vector() * randStream.Get()->FRand() * maxRadius;
				}
			}
			return FVector(0.f);
		}();
		//Try to align offset preferring shooting parallel to ground - but still within the target capsule.
		const float emitLocationToTargetHeightDifference = emitLocation.Z - target->GetActorLocation().Z;		
		if (auto targetCharacter = Cast<ACharacter>(target)) {
			if (const auto targetCapsule = targetCharacter->GetCapsuleComponent()) {
				//If possible, allow targeting the whole of the top half of the target capsule for the purpose of keeping the arrow trajectory as flat as possible.
				const float halfCapsuleHeight = targetCapsule->GetScaledCapsuleHalfHeight_WithoutHemisphere();
				offset.Z = FMath::Clamp(emitLocationToTargetHeightDifference, 0.0f, halfCapsuleHeight);
			}
		}
		return offset;
	}
	return FVector(0.f);
}

FVector URangedAttackComponent::CalculateAttackLocation(ABaseCharacter* attacker, const FVector& attackerLocation, const FVector& emitDirection, AActor* attackTarget, const FRangedAttackProjectileSpawnDescription& definition, const FItemId& ammoType) const {
	if (overrideOrigin.IsSet()) {
		return attackTarget->GetActorLocation();
	}

	if (attackTarget) {
		//Default aim with target:
		//Shoot in the direction of the emitDirection, 
		//but try to aim the projectile in Z so that it would hit if the direction was correct.
		if (auto targetCharacter = Cast<ACharacter>(attackTarget)) {
			auto attackTargetLocation = attackerLocation;

			//Aim the attack straight out from the attacker if possible while still hitting the target capsule.				
			const auto attackerFromGroundZEmitHeight = attacker->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + definition.ProjectileOffset.Z;
			const auto targetCapsuleComponent = targetCharacter->GetCapsuleComponent();
			const auto targetToGroundCenterZHeight = targetCapsuleComponent->GetScaledCapsuleHalfHeight();
			const auto targetLocation = targetCharacter->GetActorLocation();
			const auto vectorToTarget = (targetLocation - attackerLocation);

			//This is the estimated distance the projectile will travel from its offset spawn location to the target
			//It is important to estimate this correctly since our Z offset results in the angle and
			//It will be wrong of the length of the aim is incorrect.
			const float estimatedDistanceToFly = FMath::Max(1.0f, vectorToTarget.Size() - targetCapsuleComponent->GetScaledCapsuleRadius() - definition.ProjectileOffset.X);
			//This is the offset from the attacker which we should aim at. It is clamped at 25% of range/1 block to reduce the effects of height differences in super close combat.
			const float clampedAttackerAimOffsetToTarget = FMath::Max3(100.0f, RangedAttackDefinition.Range * 0.25f, estimatedDistanceToFly + definition.ProjectileOffset.X);
			
			attackTargetLocation += emitDirection * clampedAttackerAimOffsetToTarget;

			//Aim at center of target -> offset down half target height -> offset up to half our height, but max the whole size of target.
			const float distanceOfMaxRangeFactor = FMath::Clamp(estimatedDistanceToFly / RangedAttackDefinition.Range, 0.0f, 1.0f);
			const float GroundZ = targetLocation.Z - targetToGroundCenterZHeight;
			attackTargetLocation.Z = GroundZ + FMath::Min(targetToGroundCenterZHeight*(2.0f - distanceOfMaxRangeFactor), attackerFromGroundZEmitHeight);
			return attackTargetLocation;
		}
	}
	//Default aim without target:
	//Just shoot in the GetAttackerBaseEmitVector direction
	return attackerLocation + emitDirection;
}

void URangedAttackComponent::ExecuteProjectileDefintion(int index, int projectileIndex, AActor* attackTarget, FVector attackerLocation, FVector spawnDirection, bool isCharged, TSubclassOf<ABaseProjectile> projectileOverride, float attackSpeedMultiplier, FPredictionKey key, FItemId ammoType, TSharedPtr<FRandomStream> randStream) {
	const FRangedAttackProjectileSpawnDescription& definition = RangedAttackDefinition.GetCurrentProjectileDefinitions(isCharged)[index];
	auto attacker = Cast<ABaseCharacter>(GetOwner());
	auto world = attacker->GetWorld();

	TSubclassOf<ABaseProjectile> projectileDefintion = projectileOverride ? projectileOverride : definition.Projectile;

	//Offset Z first to put emit location at the right height.
	auto attackerEmitLocation = attackerLocation;
	attackerEmitLocation.Z += definition.ProjectileOffset.Z;

	auto attackLocation = CalculateAttackLocation(attacker, attackerEmitLocation, spawnDirection, attackTarget, definition, ammoType);	

	auto spawnRotation = UKismetMathLibrary::FindLookAtRotation(attackerEmitLocation, attackLocation);
	auto emitRotation = spawnRotation;

	// Level out roll and pitch if we want to always force parallel
	if (pitchMode == EPitchMode::Parallel) {
		emitRotation.Roll = 0.f;
		emitRotation.Pitch = 0.f;
	}
	// Level out spawn rotation if we want the spawn origo to always be parallel
	if (pitchMode == EPitchMode::Parallel || pitchMode == EPitchMode::ParallelSpawn) {
		spawnRotation.Roll = 0.f;
		spawnRotation.Pitch = 0.f;
	}

	//Remove Z from offset that is rotated
	auto ProjectileOffsetToRotate = definition.ProjectileOffset;
	ProjectileOffsetToRotate.Z = 0.0f;
	if(attackTarget){
		//Never spawn the projectile at a larger offset than the distance to target
		//this makes sure we dont spawn projectiles behind our target, but instead inside them.
		const auto vectorToTarget = (attackTarget->GetActorLocation() - attackerLocation);
		ProjectileOffsetToRotate.X = FMath::Min(ProjectileOffsetToRotate.X, vectorToTarget.Size());
	}

	const auto spawnLocation = attackerEmitLocation + spawnRotation.RotateVector(ProjectileOffsetToRotate);

	const bool isFullCircle = FMath::IsNearlyEqual(definition.AngleSpan, 360.f);

	float AngleSpan = definition.AngleSpan;	
	float AngleOffset = definition.AngleOffset;	
	float MaxRandomizedAngleFactor = definition.MaxRandomizedAngleFactor;
	int numberOfProjectiles = definition.NumberOfProjectiles;


	float angleIncrement = AngleSpan;
	float angleStart = AngleOffset;	
	
	if (numberOfProjectiles > 1) {
		angleIncrement /= isFullCircle ? numberOfProjectiles : numberOfProjectiles - 1;
		angleStart -= angleIncrement * ((float)numberOfProjectiles - 1)/2.f;
	}
	
	const auto GetEmitRotation = [&] {
		int distanceSteps = (projectileIndex + 1) / 2; // 0, 1, 1, 2, 2
		int angleSide = projectileIndex % 2 == 0 ? 1 : -1; // 1, -1, 1, -1, 1

		float angleTarget = AngleOffset + distanceSteps * angleIncrement * angleSide;
		float angleRandomizationOffset = 0;
		if (!(projectileIndex == 0 && definition.NonRandomFirstArrow)) {
			angleRandomizationOffset = angleIncrement * randStream->FRandRange(-MaxRandomizedAngleFactor, MaxRandomizedAngleFactor);
		}
		return emitRotation + FRotator(0.f, angleTarget + angleRandomizationOffset, 0.f);
	};
	

	ExecuteProjectileDefinitionIndex(projectileDefintion, spawnLocation, isCharged, GetEmitRotation(), attacker, attackTarget, attackLocation, randStream);
	projectileIndex++;
	
	//Reset target if we are firing in an arc
	if (!(FMath::IsNearlyEqual(definition.AngleSpan, 0) && FMath::IsNearlyEqual(definition.AngleOffset, 0))) attackTarget = nullptr;
	
	while (projectileIndex < numberOfProjectiles) {

		if (definition.DelaySecondsPerProjectile <= 0.0f) {
			//Only the first arrow should have a target currently.
			ExecuteProjectileDefinitionIndex(projectileDefintion, spawnLocation, isCharged, GetEmitRotation(), attacker, nullptr, attackLocation, randStream);
			projectileIndex++;
		}else{
			FTimerManager& timerManager = world->GetTimerManager();			
			timerManager.SetTimer(ProjectileSpawnArcDelayHandle, FTimerDelegate::CreateUObject(this, &URangedAttackComponent::ExecuteProjectileDefintion, index, projectileIndex, attackTarget, attackerLocation, spawnDirection, isCharged, projectileOverride, attackSpeedMultiplier, key, ammoType, randStream), definition.DelaySecondsPerProjectile / attackSpeedMultiplier, false);
			break;
		}
	}

	//The last projectile has been spawned and we are thus finaly done with the ranged attack
	if (projectileIndex == numberOfProjectiles) {
		ProjectileSpawnArcDelayHandle.Invalidate();
		AttackEnded(true, projectileDefintion);
		DungeonsGearUtilLibrary::OnAfterRangedAttack(attacker, attackTarget, projectileDefintion, isCharged, attackerLocation, emitRotation, TemporaryAttackArrowCount, *randStream.Get());
	}
}

ABaseProjectile* URangedAttackComponent::SpawnProjectileDeferred(TSubclassOf<ABaseProjectile> projectileDefintion, float ItemPower, const FTransform& transform, ABaseCharacter* attacker, const FRangedAttack& rangedAttackDefinition, bool isCharged, bool canTriggerEnchantments, TOptional<float> sourceItemPower) {
	auto world = attacker->GetWorld();
	if (auto projectile = AProjectileActorManager::Pop_Projectile(projectileDefintion, transform, attacker))
	{
		auto abilitySystem = attacker->GetAbilitySystemComponent();
		auto pushbackMultiplier = abilitySystem->GetNumericAttributeChecked(URangedAttributeSet::RangedAttackPushbackMultiplierAttribute());

		projectile->bCanTriggerEnchantments = canTriggerEnchantments;

		const float projectileVelocityFactor = projectile->RespectWeaponProjectileVelocityFactor ? rangedAttackDefinition.ProjectileVelocityFactor : 1;

		projectile->IsCharged = isCharged;
		projectile->SourceItemPower = sourceItemPower;
		projectile->WeaponItemPower = ItemPower;
		projectile->SetVelocityFactor((isCharged ? rangedAttackDefinition.ChargeVelocityMultiplier : 1.0f) * projectileVelocityFactor * projectile->GetEnvironmentSpeedMultiplier());
		projectile->SetDamageFactor((isCharged ? rangedAttackDefinition.ChargeDamageMultiplier : 1.f) * rangedAttackDefinition.ProjectileDamageFactor);
		projectile->SetPushbackMultiplier(pushbackMultiplier);

		return projectile;
	}
	return nullptr;
}

void URangedAttackComponent::ResetProjectile(ABaseProjectile* projectile) {
	SpawnMap.Remove(projectile);
}

ABaseProjectile* URangedAttackComponent::GetProjectileForSpawnId(uint8 id) {
	for (auto it = SpawnMap.CreateIterator(); it; ++it) {
		if (it->Value == id) return it->Key;
	}

	return nullptr;
}

TOptional<uint8> URangedAttackComponent::GetProjectileId(const ABaseProjectile* projectile) const {
	return SpawnMap.Contains(projectile) ? SpawnMap[projectile] : TOptional<uint8>();
}

void URangedAttackComponent::TrySetDistanceBasedVelocity(ABaseProjectile* projectile, const FVector& from, const FVector& to) {
	const auto projectileMovementComponent = projectile->FindComponentByClass<UProjectileMovementComponent>();
	const auto shouldApplyGravity = projectileMovementComponent->ShouldApplyGravity();
	if (shouldApplyGravity) {
		const auto distance = FVector::Dist2D(from, to);

		const auto pitch = [&] {
			const FloatRange distanceRange(projectile->MaxDist5Degrees, projectile->MaxDist40Degrees);
			const FloatRange pitchRange(5.f, 40.f);
			return pitchRange.lerp(distanceRange.clampedFractionAt(distance));
		}();

		const FRotator adjustedPitch{ pitch, 0.f, 0.f };

		projectileMovementComponent->SetVelocityInLocalSpace(adjustedPitch.Vector() * projectileMovementComponent->InitialSpeed);
	}
}

void URangedAttackComponent::SetRandomizedPitchBasedVelocity(ABaseProjectile* projectile, const FFloatRange& pitchRange,  const FRandomStream& randStream) {
	const auto projectileMovementComponent = projectile->FindComponentByClass<UProjectileMovementComponent>();

	const FRotator randomPitch{
		randStream.FRandRange(pitchRange.GetLowerBoundValue(), pitchRange.GetUpperBoundValue()),
		0.f,
		0.f
	};

	projectileMovementComponent->SetVelocityInLocalSpace(randomPitch.Vector() * projectileMovementComponent->InitialSpeed);
}

void URangedAttackComponent::ExecuteProjectileDefinitionIndex(TSubclassOf<ABaseProjectile> projectileDefintion, const FVector& spawnLocation, bool isCharged, const FRotator& spawnRotation, ABaseCharacter* attacker, AActor* attackTarget, FVector attackLocation, TSharedPtr<FRandomStream> randStream) {
	FTransform transform(spawnRotation, spawnLocation);
	TOptional<float> level;
	if (const auto* arrowSlot = Cast<UArrowItemSlot>(GetCurrentAmmoSlot())) {
		if (const auto* sourceItem = arrowSlot->GetTopSourceItem()) {
			level = sourceItem->SpecToApply.GetLevel();
		}
	}

	const float itemPower = RangedWeapon ? RangedWeapon->GetItemPower() : 1.f;

	if (auto projectile = SpawnProjectileDeferred(projectileDefintion, itemPower, transform, attacker, RangedAttackDefinition, isCharged, true, level)) {
		const bool shouldHomeIn = projectile->IsHoming && attackTarget != nullptr && [&]{
			const auto* healthComponent = attackTarget->FindComponentByClass<UHealthComponent>();
			return healthComponent == nullptr || healthComponent->ShouldImpactProjectile(projectile->GetDamageType());
		}();

		if (shouldHomeIn) {
			projectile->SetHomingTarget(attackTarget, CalculateHomingTargetOffset(spawnLocation, attackTarget, randStream));
		}

		projectile->LaunchProjectile(attacker);
		SpawnMap.Add(projectile);
		SpawnMap[projectile] = TemporaryAttackArrowCount;

		if (pitchMode == EPitchMode::Adjust) {
			TrySetDistanceBasedVelocity(projectile, spawnLocation, attackLocation);
		} else if (pitchMode == EPitchMode::Random) {
			SetRandomizedPitchBasedVelocity(projectile, pitchRange, *randStream);
		}

		if (auto gameMode = Cast<ADungeonsGameMode>(attacker->GetWorld()->GetAuthGameMode())) {		
			gameMode->OnActorSpawnedProjectile(GetOwner(), projectile);
		}

		++AuthorativeArrowCount;
		++TemporaryAttackArrowCount;
		
		DungeonsGearUtilLibrary::OnAfterRangedAttackPerProjectile(attacker, attackTarget, attackLocation, projectile, projectileDefintion, spawnLocation, spawnRotation, TemporaryAttackArrowCount, *randStream.Get());

		projectile->EnableProjectileCollisions();
		bHasSpawnedProjectilesThisAttack = true;
	}
}


//Signal to player is intended here
void URangedAttackComponent::OnInsuffientArrows() const {
	OnRangedAttackInsuffientArrows.Broadcast();
}

//Signal to player is intended here
void URangedAttackComponent::OnNoRangedWeaponEquipped() const {
	OnNoRangedWeaponWasEquipped.Broadcast();
}

bool URangedAttackComponent::BeginAttack(const FAttackAimProvider& targetProvider) {

	ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
	if (!HasAmmo() || Cast<APlayerCharacter>(GetOwner())->IsFrozenSolid()) return false;
	IsContinuousAttack = true;

	const FSharedPredictionContext predictionContext = FSharedPredictionContext::WithNewKey(owner->GetAbilitySystemComponent());
	BeginAttackAnimation(predictionContext);

	//Send info to server if we are an autonomous proxy.
	if (!owner->HasAuthority()) {
		ServerBeginAttack(owner->GetControlRotation(), predictionContext.GetKey());
	}
	
	CachedTargetProvider = targetProvider;

	if (auto player = Cast<APlayerCharacter>(owner)) {
		OnPlayerBeginRangedAttack.Broadcast(player);
	}

	if (ShouldUseChargeUpAttack()) {
		return false;
	}
	
	return Super::BeginAttack(targetProvider);
}

void URangedAttackComponent::BeginAttackAnimation(FSharedPredictionContext predictionContext)
{
	ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
	auto world = owner->GetWorld();
	if (AttackStateResetHandle.IsValid()) {
		world->GetTimerManager().ClearTimer(AttackStateResetHandle);
	}
	if (DelayedEndAttackHandle.IsValid()) {
		world->GetTimerManager().ClearTimer(DelayedEndAttackHandle);
	}

	IsCharged = false;

	if (UAnimMontage* montage = Cast<UAnimMontage>(RangedAttackDefinition.Animation)) {
		//If we are at 0 cooldown, this animation should start at the very end and wait
		//otherwise, we animate a cooldown up until the point of attack
		//We animate the cooldown at the start of the attack because first attack
		//has no cooldown, and the actual cooldown can happen while moving around.

		const FName reloadSection("Reload");
		const auto sectionIndex = montage->GetSectionIndex(reloadSection);

		//Use length of cooldown animation to adapt animation speed - should finish when next attack is possible
		const auto cooldownAnimDuration = montage->IsValidSectionIndex(sectionIndex) ? montage->GetSectionLength(sectionIndex) : 1.0f;

		//Blend if there is cooldown remaining, otherwise just snap.
		const float secondsUntilAttackCanHappen = GetSecondsUntilNextPossibleAttack();
		const float blendTime = Math::min(0.2f, secondsUntilAttackCanHappen);
		const float attackCooldown = attackRateSeconds;

		//Start animation somewhere between beginning and end of cooldown animation to match remaining cooldown.
		const float SectionPercentage = attackCooldown > 0.0f ? Math::clamp(1.0f - secondsUntilAttackCanHappen / attackCooldown, 0.0f, 1.0f) : 1.0f;

		//Speed of cooldown animation should match the weapon cooldown.
		const float animationSpeed = attackCooldown > 0.0f ? cooldownAnimDuration / attackCooldown : 1.0f;

		//Animate
		if (owner->HasAuthority()) {
			owner->MulticastPlayMontageSectionWithBlendAtSectionPercentage(montage, reloadSection, animationSpeed, EMontagePlayReturnType::Duration, 0.f, blendTime, SectionPercentage, predictionContext.GetKey());
		}
		else {
			owner->PlayMontageSectionWithBlendAtSectionPercentage(montage, reloadSection, animationSpeed, EMontagePlayReturnType::Duration, 0.f, blendTime, SectionPercentage);
		}
	}
	owner->SetAttackState(EAttackState::Ranged);

	if (ShouldUseChargeUpAttack()) {
		auto now = GetWorld()->GetTimeSeconds();
		if (AutoCharge || now >= canAttackFromSecond) {
			BeginChargeUp(predictionContext);
		}
		else {
			GetWorld()->GetTimerManager().SetTimer(DelayBeginChargeHandle, FTimerDelegate::CreateUObject(this, &URangedAttackComponent::BeginChargeUp, predictionContext), canAttackFromSecond - now, false);
		}
	}
}

FPredictionKey URangedAttackComponent::AttackLocal(AActor* attackTarget) {
	if (CanAttack(attackTarget)) {
		if (auto character = Cast<APlayerCharacter>(GetOwner())) {
			OnPlayerRangedAttack.Broadcast(character);
			if (attackTarget) {
				character->OnAttack();
			}
		}
		IssuedAttackCounter++;
		return Super::AttackLocal(attackTarget);
	}
	return FPredictionKey();
}

void URangedAttackComponent::EndAttack(AActor* attackTarget) {
	bool isAlive = actorquery::isAlive(GetOwner());

	IsContinuousAttack = false;

	if (ShouldUseChargeUpAttack() && isAlive) {
		AttackLocal(attackTarget);
	}
	else {
		Super::EndAttack(attackTarget);
	}

	CachedTargetProvider.Unbind();

	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	FSharedPredictionContext predictionContext = FSharedPredictionContext::WithNewKey(abilitySystem);
	
	ServerEndAttack(predictionContext.GetKey());
	if (!GetOwner()->HasAuthority()) {
		EndAttack_Internal(predictionContext);
	}
}

void URangedAttackComponent::ServerStopAttackAnimation_Implementation(FPredictionKey predictionKey) {
	StopAttackAnimation(FSharedPredictionContext(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()), predictionKey));
}

bool URangedAttackComponent::ServerStopAttackAnimation_Validate(FPredictionKey predictionKey) {
	return true;
}

void URangedAttackComponent::StopAttackAnimation(FSharedPredictionContext predictionContext) {
	if (ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner())) {
		if (owner->HasAuthority()) {
			if (UAnimMontage* montage = Cast<UAnimMontage>(RangedAttackDefinition.Animation)) {
				owner->MulticastStopMontage(montage->BlendOut.GetAlpha(), montage, predictionContext.GetKey());
			} else {
				owner->MulticastStopSlotAnimation(RangedAttackDefinition.Slot, 0.2f, predictionContext.GetKey());
			}
		} else {
			if (UAnimMontage* montage = Cast<UAnimMontage>(RangedAttackDefinition.Animation)) {
				owner->StopMontage(montage->BlendOut.GetAlpha(), montage);
			} else {
				owner->StopSlotAnimation(RangedAttackDefinition.Slot, 0.2f);
			}
		}
	}
}

void URangedAttackComponent::SetRangeAttackVariant(int attackIndex) {
	FTimerManager& timerManager = GetOwner()->GetWorld()->GetTimerManager(); //don't switch attack variant while projectiles a waiting to fire. Don't want to loose the projectile definitions for the current attack
	if( GetOwner()->HasAuthority() && !timerManager.IsTimerActive(ProjectileSpawnHandle)) {
		AttackIndex = attackIndex;
		OnRep_AttackIndex();
	}
}

void URangedAttackComponent::OnAttackDurationPassed() {
	OnDurationPassed.Broadcast();
}

void URangedAttackComponent::OnRep_AttackIndex() {
	SetRangedAttackDefinitionVariant(AttackIndex);
}


void URangedAttackComponent::ResetRangeAttackVariant() {
	SetRangeAttackVariant(-1);
}

void URangedAttackComponent::StopAttack() {	
	float delayTimeRemaining = 0;
	if (ProjectileSpawnHandle.IsValid())
	{
		FTimerManager& timerManager = GetOwner()->GetWorld()->GetTimerManager();
		delayTimeRemaining = timerManager.GetTimerRemaining(ProjectileSpawnHandle);
	}

	if (!AttackDefinition().Interruptable && delayTimeRemaining > 0)
	{
		return;
	}

	if (IsAttacking() || ProjectileSpawnHandle.IsValid()) {
		ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
		FSharedPredictionContext context = FSharedPredictionContext::WithNewKey(owner->GetAbilitySystemComponent());
		auto& timerManager = owner->GetWorld()->GetTimerManager();

		IsContinuousAttack = false;

		CachedTargetProvider.Unbind();

		ResetAttackState();

		Super::EndAttack();
		timerManager.ClearAllTimersForObject(this);
		
		if (!GetOwner()->HasAuthority()) {
			ServerStopAttackAnimation(context.GetKey());
		} else if(ProjectileSpawnHandle.IsValid() || ProjectileSpawnArcDelayHandle.IsValid()) {
			const auto& definitions = RangedAttackDefinition.GetCurrentProjectileDefinitions(IsCharged);
			AttackEnded(false, definitions[definitions.Num() - 1].Projectile);
		}

		StopAttackAnimation(context);
	}

	OnAttackStopped.Broadcast();
	Server_StopAttack();
}

void URangedAttackComponent::Server_StopAttack_Implementation() {
	OnAttackStopped.Broadcast();
}

bool URangedAttackComponent::Server_StopAttack_Validate() {
	return true;
}

void URangedAttackComponent::AttackEnded(bool completed, TSubclassOf<ABaseProjectile> projectileDefinition) {
	if (GetOwnerRole() == ROLE_Authority && AmmoSlot.IsValid())
	{
		if(bHasSpawnedProjectilesThisAttack) {
			ConsumeAmmo(projectileDefinition);
		}
		DungeonsGearUtilLibrary::OnRangedAttackEnded(Cast<ABaseCharacter>(GetOwner()), completed, bHasSpawnedProjectilesThisAttack);
		bHasSpawnedProjectilesThisAttack = false;
	}
}

void URangedAttackComponent::LaunchHeldThrowablePlayerForwardDir()
{
	AThrowablePropActor* heldThrowable = HeldThrowables.Num() > 0 ? HeldThrowables[HeldThrowables.Num() - 1] : nullptr;
	if (heldThrowable)
	{
		heldThrowable->ThrowInPlayerForwardDir();
	}
}

void URangedAttackComponent::ConsumeAmmo(TSubclassOf<ABaseProjectile> projectileDefinition) {
	if (bShouldConsumeAmmo) {
		AmmoSlot->Consume();
		
		const auto& definitions = RangedAttackDefinition.GetCurrentProjectileDefinitions(IsCharged);
		TSubclassOf<ABaseProjectile> BaseProjectileClass = projectileDefinition ? projectileDefinition : definitions[definitions.Num() - 1].Projectile;
		DungeonsGearUtilLibrary::OnAfterAmmoConsumed(Cast<ABaseCharacter>(GetOwner()), BaseProjectileClass);

		if (AmmoSlot->GetCount() == 0) {
			if(ALobbyGameMode* gameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode())){
				if (APlayerCharacter* owner = Cast<APlayerCharacter>(GetOwner())) {
					FTimerHandle handle;
					GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateUObject(owner, &APlayerCharacter::ResetArrowCount), gameMode->GetArrowReplenishTime(), false);
				}
			}
		}
	}
}

bool URangedAttackComponent::ServerBeginAttack_Validate(FRotator attackRotation, FPredictionKey predictionKey) {
	return true;
}

bool URangedAttackComponent::ServerEndAttack_Validate(FPredictionKey predictionKey) {
	return true;
}

void URangedAttackComponent::ServerBeginAttack_Implementation(FRotator attackRotation, FPredictionKey predictionKey) {
	ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
	owner->GetController()->SetControlRotation(attackRotation);
	BeginAttackAnimation(FSharedPredictionContext(owner->GetAbilitySystemComponent(), predictionKey));
}

void URangedAttackComponent::BeginChargeUp(FSharedPredictionContext predictionContext) {
	if (!ShouldUseChargeUpAttack()) {
		return;
	}
	ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
	
	auto world = owner->GetWorld();
	auto abilitySystem = owner->GetAbilitySystemComponent();
	const float rangedAttackSpeedMultiplier = abilitySystem->GetNumericAttribute(URangedAttributeSet::RangedAttackSpeedMultiplierAttribute());

	if (UAnimMontage* montage = Cast<UAnimMontage>(RangedAttackDefinition.Animation)) {
		const FName chargeSection("Charge");
		if (owner->HasAuthority()) {
			owner->MulticastJumpToSectionMontageWithPlayrate(chargeSection, rangedAttackSpeedMultiplier, montage, predictionContext.GetKey());
		}
		else {
			owner->JumpToSectionMontageWithPlayrate(chargeSection, rangedAttackSpeedMultiplier, montage);
		}
	}

	if (AutoCharge) {
		ChargeComplete(predictionContext);
		return;
	}
	else {
		world->GetTimerManager().SetTimer(DelayBeginChargeHandle, FTimerDelegate::CreateUObject(this, &URangedAttackComponent::ChargeComplete, predictionContext), RangedAttackDefinition.ChargeTimeSecond / rangedAttackSpeedMultiplier, false);
	}
}

void URangedAttackComponent::ChargeComplete(FSharedPredictionContext predictionContext) {
	IsCharged = true;
	ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
	if (UAnimMontage* montage = Cast<UAnimMontage>(RangedAttackDefinition.Animation)) {
		const FName chargeSection("Charged");
		auto abilitySystem = owner->GetAbilitySystemComponent();
		const float rangedAttackSpeedMultiplier = abilitySystem->GetNumericAttribute(URangedAttributeSet::RangedAttackSpeedMultiplierAttribute());

		if (owner->HasAuthority()) {
			owner->MulticastJumpToSectionMontageWithPlayrate(chargeSection, rangedAttackSpeedMultiplier, montage, predictionContext.GetKey());
		} else {
			owner->JumpToSectionMontageWithPlayrate(chargeSection, rangedAttackSpeedMultiplier, montage);
		}
	}

	const float rangedAttackSpeedMultiplier = owner->GetAbilitySystemComponent()->GetNumericAttribute(URangedAttributeSet::RangedAttackSpeedMultiplierAttribute());
	OnChargeComplete.Broadcast(RangedAttackDefinition.ChargeTimeSecond / rangedAttackSpeedMultiplier);
}

void URangedAttackComponent::ServerEndAttack_Implementation(FPredictionKey predictionKey) {
	EndAttack_Internal(FSharedPredictionContext(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()), predictionKey));
}

TArray<UItemSlot*> URangedAttackComponent::GetAvailableAmmoSlots() const {
	TArray<UItemSlot*> slots;
	for (auto slot : AvailableAmmoSlots) {
		if (slot.IsValid()) {
			slots.Add(slot.Get());
		}
	}
	return slots;
}

void URangedAttackComponent::EndAttack_Internal(FSharedPredictionContext predictionContext) {
	ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
	auto world = owner->GetWorld();
	float now = world->GetTimeSeconds();
	float canActFrom = owner->GetCanActFromSecondActive();

	if (DelayBeginChargeHandle.IsValid()) {
		world->GetTimerManager().ClearTimer(DelayBeginChargeHandle);
	}

	IsCharged = false;

	if (UAnimMontage* montage = Cast<UAnimMontage>(RangedAttackDefinition.Animation)) {
		if (canActFrom > now) {
			world->GetTimerManager().SetTimer(DelayedEndAttackHandle, FTimerDelegate::CreateUObject(this, &URangedAttackComponent::EndAttack_Internal, predictionContext), canActFrom - now, false);
		} else {
			const float blendOut = 0.2f;
			if (owner->HasAuthority()) {
				owner->MulticastStopMontage(blendOut, montage, predictionContext.GetKey());
			} else {
				owner->StopMontage(blendOut, montage);
			}
			world->GetTimerManager().SetTimer(AttackStateResetHandle, FTimerDelegate::CreateUObject(this, &URangedAttackComponent::ResetAttackState), blendOut, false);
		}
	}
}

void URangedAttackComponent::ResetAttackState() {
	ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
	owner->SetAttackState(EAttackState::None);
}

void URangedAttackComponent::OnWeaponSlotUpdated(UItemSlot* slot) {
	RangedWeapon = Cast<ARangedWeaponGearItemInstance>(slot->GetItem());

	RangedAttackDefinition = RangedWeapon ? RangedWeapon->GetRangedAttackDefintion() : DefaultRangedAttack;

	if (RangedWeapon)
	{
		if (APlayerCharacter* pPlayerOwner = Cast<APlayerCharacter>(GetOwner()))
		{
			//player's arrows, lets make sure we have cached the required ones
			TArray< TSubclassOf< ABaseProjectile > > NewProjectiles;
			NewProjectiles.Reserve(64);

			for (const auto& AttackDesc : RangedAttackDefinition.ProjectileDefinition)
			{
				if (AttackDesc.Projectile)
				{
					NewProjectiles.Push(AttackDesc.Projectile);
				}
			}

			for (const auto& AttackDesc : RangedAttackDefinition.ChargedProjectileDefinition)
			{
				if (AttackDesc.Projectile)
				{
					NewProjectiles.Push(AttackDesc.Projectile);
				}
			}

			if (RangedWeapon)
			{
				for (const auto& OverrideClass : RangedWeapon->GetOverrides())
				{
					if (OverrideClass)
					{
						NewProjectiles.Push(OverrideClass);
					}
				}
			}

			AProjectileActorManager::PreCache_PlayerProjectiles(NewProjectiles, pPlayerOwner);
		}
	}

	OnRangedWeaponChanged.Broadcast();
}

bool URangedAttackComponent::HasRangedWeaponEquipped() const {
	return RangedWeapon != nullptr;
}

bool URangedAttackComponent::ShouldUseChargeUpAttack() const {
	return RangedAttackDefinition.CanChargeUp();
}

bool URangedAttackComponent::EndAttackIsAttackExecution() const {
	return ShouldUseChargeUpAttack();
}

//Stats
float URangedAttackComponent::GetRangedAttackStat(const FRangedAttack& AttackDefinition, TSubclassOf<ABaseProjectile> ProjectileClass, EItemStats stat)
{
	switch (stat) {
	case EItemStats::AverageDamage:
	{
		float damage = 0.0f;
		float projectiles = 0.0f;

		for (const auto& var : AttackDefinition.ProjectileDefinition) {
			projectiles += var.NumberOfProjectiles;
			if (auto projectileClass = ProjectileClass ? ProjectileClass : var.Projectile) {
				const auto projectile = Cast<ABaseProjectile>(projectileClass->GetDefaultObject());
				damage += (projectile->GetDamage() + projectile->GetExplosionDamage()) * AttackDefinition.ProjectileDamageFactor * var.NumberOfProjectiles;
			}
		}

		return damage / projectiles;
	}
	case EItemStats::DamagePerVolley:
	{
		float damage = 0.0f;

		for (const auto& var : AttackDefinition.ProjectileDefinition) {
			if(auto projectileClass = ProjectileClass ? ProjectileClass : var.Projectile){
				const auto projectile = Cast<ABaseProjectile>(projectileClass->GetDefaultObject());
				damage += (projectile->GetDamage() + projectile->GetExplosionDamage()) * var.NumberOfProjectiles * AttackDefinition.ProjectileDamageFactor;
			}
		}

		return damage ;
	}
	case EItemStats::AttackSpeed:
	{
		float projectiles = 0.0f;
		float seconds = AttackDefinition.AttackRateSeconds;

		for (const auto& var : AttackDefinition.ProjectileDefinition) {
			projectiles += var.NumberOfProjectiles;
			seconds += var.GetTotalDelay();
		}

		return projectiles / seconds;
	}
	case EItemStats::AreaDamage:
	{
		float damage = 0.0f;
		float projectiles = 0.0f;

		for (const auto& var : AttackDefinition.ProjectileDefinition) {
			projectiles += var.NumberOfProjectiles;
			if(auto projectileClass = ProjectileClass ? ProjectileClass : var.Projectile){
				const auto projectile = Cast<ABaseProjectile>(projectileClass->GetDefaultObject());
				float explosionArea = FMath::Pow(projectile->GetExplosionRadius(), 2) * PI;
				damage += projectile->GetExplosionDamage() * var.NumberOfProjectiles * explosionArea * AttackDefinition.ProjectileDamageFactor;
			}
		}

		return damage / projectiles;
	}
	case EItemStats::ArrowSpeed:
	{
		float speed = 0.0f;
		float projectiles = 0.0f;

		for (const auto& var : AttackDefinition.ProjectileDefinition) {
			projectiles += var.NumberOfProjectiles;
			if(auto projectileClass = ProjectileClass ? ProjectileClass : var.Projectile){
				const auto projectile = Cast<ABaseProjectile>(projectileClass->GetDefaultObject());
				speed += projectile->ProjectileMovementComponent->InitialSpeed * var.NumberOfProjectiles * AttackDefinition.ProjectileVelocityFactor;
			}
		}

		return speed / projectiles;
	}
	case EItemStats::DamagePerSec:
	{
		float damage = 0.0f;
		float seconds = AttackDefinition.AttackRateSeconds;

		for (const auto& var : AttackDefinition.ProjectileDefinition) {
			seconds += var.GetTotalDelay();
			if(auto projectileClass = ProjectileClass ? ProjectileClass : var.Projectile){
				const auto projectile = Cast<ABaseProjectile>(projectileClass->GetDefaultObject());
				damage += (projectile->GetDamage() + projectile->GetExplosionDamage()) * var.NumberOfProjectiles * AttackDefinition.ProjectileDamageFactor;
			}
		}

		return damage / seconds;
	}
	case EItemStats::LowestDamage:
	{
		float lowest = FLT_MAX;
		for (const auto& projectile : AttackDefinition.ProjectileDefinition) {
			if(auto projectileClass = ProjectileClass ? ProjectileClass : projectile.Projectile){
				const auto defaultProjectileObject = projectileClass.GetDefaultObject();
				lowest = FMath::Min(lowest, FMath::Max(defaultProjectileObject->GetDamage(), defaultProjectileObject->GetExplosionDamage()));
			}
		}

		lowest *= AttackDefinition.ProjectileDamageFactor;

		return lowest;
	}
	case EItemStats::HighestDamage:
	{
		float highest = -FLT_MAX;
		for (const auto& projectile : AttackDefinition.ProjectileDefinition) {
			if (auto projectileClass = ProjectileClass ? ProjectileClass : projectile.Projectile) {
				const auto defaultProjectileObject = projectileClass.GetDefaultObject();
				highest = FMath::Max(highest, FMath::Max(defaultProjectileObject->GetDamage(), defaultProjectileObject->GetExplosionDamage()));
			}
		}

		const float chargedDamageFactor = (AttackDefinition.CanChargeUp() ? AttackDefinition.ChargeDamageMultiplier : 1.0f);

		highest *= chargedDamageFactor;
		highest *= AttackDefinition.ProjectileDamageFactor;

		return highest;
	}
	case EItemStats::ProjectilesPerAttack:
	{
		float count = 0;
		for (const auto& projectile : AttackDefinition.ProjectileDefinition) {
			count += projectile.NumberOfProjectiles;
		}
		return count;
	}
	default:
		return -1.0f;
	}
}

bool URangedAttackComponent::AddThrowable(AThrowablePropActor* throwable) {
	if (CanAddThrowable(throwable->ThrowType)) {
		HeldThrowables.Emplace(throwable);
		return true;
	}
	return false;
}

bool URangedAttackComponent::CanAddThrowable(EThrowableType type) {
	for (AThrowablePropActor* throwable : HeldThrowables) {
		if (!throwable || (throwable && throwable->ThrowType != type)) {
		
			if (AActorShakeManager* shakeManager = actorquery::getFirstActor<AActorShakeManager>(GetWorld())) {
				shakeManager->AddActorShake(throwable, throwable->DenialShake, 1.0f);
			}
			return false;
		}
	}
	return true;
}

void URangedAttackComponent::RemoveThrowable(AThrowablePropActor* throwable) {
	if (HeldThrowables.Contains(throwable)) {
		HeldThrowables.Remove(throwable);
	}
}

AThrowablePropActor* URangedAttackComponent::GetAndRemoveTopThrowable() {
	if (HeldThrowables.Num() <= 0)
	{
		return nullptr;
	}

	AThrowablePropActor* throwable = HeldThrowables[HeldThrowables.Num() - 1];
	HeldThrowables.Remove(throwable);
	throwable->Consume();
	throwable->DisableInput(nullptr);
	return throwable;
}

bool URangedAttackComponent::IsThrowableSlot(ESlotType SlotTypeID) {
	switch (SlotTypeID) {
		case ESlotType::TNT :
		case ESlotType::Trident :
		case ESlotType::Conduit :
			return true;
	}
	return false;
}

float URangedAttackComponent::GetWeaponChargeTime() {
	const float rangedAttackSpeedMultiplier = Cast<ABaseCharacter>(GetOwner())->GetAbilitySystemComponent()->GetNumericAttribute(URangedAttributeSet::RangedAttackSpeedMultiplierAttribute());
	return (RangedAttackDefinition.ChargeTimeSecond / rangedAttackSpeedMultiplier) + (RangedAttackDefinition.AttackRateSeconds / rangedAttackSpeedMultiplier);
}