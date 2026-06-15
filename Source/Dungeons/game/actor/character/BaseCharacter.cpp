#include "Dungeons.h"
#include "BaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "lovika/LovikaLevelActor.h"
#include "game/component/HealthComponent.h"
#include "game/component/HealthBarComponent.h"
#include "game/component/BaseParticleAssetsComponent.h"
#include "game/component/RagdollOnDeathComponent.h"
#include "game/util/ActorQuery.h"
#include "game/team/TeamQuery.h"
#include "game/Conversion.h"
#include "game/GameBP.h"
#include "world/level/BlockSource.h"
#include "UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "game/abilities/effects/LavaBurningGamePlayEffect.h"
#include "game/abilities/effects/LavaGameplayEffect.h"
#include "game/abilities/effects/WaterGameplayEffect.h"
#include "NavigationPath.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.h"
#include <GameplayTagContainer.h>
#include <GameplayEffectTypes.h>
#include "game/DamageHelpers.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "game/abilities/attributes/StatusAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <NavigationSystem.h>
#include <NavModifierComponent.h>
#include <NavArea_Null.h>
#include "game/abilities/attributes/DifficultyAttributeSet.h"
#include "game/abilities/attributes/AffectorAttributeSet.h"
#include "game/util/Tags.h"
#include "game/level/sound/AudioManager.h"
#include "DungeonsGameInstance.h"
#include "client/renderer/block/BlockGraphics.h"
#include "AI/Navigation/NavAgentInterface.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/component/BlockTriggerComponent.h"
#include "DungeonsAbilitySystemComponent.h"
#include "game/abilities/effects/FallDamageEffect.h"
#include "game/abilities/attributes/FrozenSolidIceCubeAttributeSet.h"
#include "game/util/DungeonsGearUtilLibrary.h"
#include "game/util/DungeonsEffectLibrary.h"
#include "game/util/ComponentUtils.h"
#include "util/CollectionUtils.h"
#include "game/abilities/attributes/DamageAttributeSet.h"
#include "game/component/WitherPoisonVisualizationComponent.h"
#include "game/component/PlayerCharacterMovementComponent.h"
#include "game/Enchantments/Levitation.h"
#include "game/Enchantments/VoidTouched.h"

DECLARE_STATS_GROUP(TEXT("HealthComponent"), STATGROUP_BaseCharacter, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("OnCharacterDeath"), STAT_OnCharacterDeath, STATGROUP_BaseCharacter);

bool FMontageData::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) {
	uint16 RepBits = gatherRepBits(Ar);

	Ar.SerializeBits(&RepBits, 9);
	Ar << asset;
	Ar << slotNodeName;
	writeOptionals(Ar, RepBits);

	if (RepBits & PredictionKeyFlag) {
		key.NetSerialize(Ar, Map, bOutSuccess);
	}

	bOutSuccess = true;
	return true;
}

uint16 FMontageData::gatherRepBits(FArchive& Ar)
{
	uint16 RepBits = 0;

	if (Ar.IsSaving()) {
		if (!FMath::IsNearlyZero(blendInTime)) {
			RepBits |= BlendInTimeFlag;
		}
		if (!FMath::IsNearlyEqual(blendOutTime, 0.2f)) {
			RepBits |= BlendOutTimeFlag;
		}
		if (!FMath::IsNearlyEqual(inPlayRate, 1.f)) {
			RepBits |= InPlayRateFlag;
		}
		if (loopCount != 1) {
			RepBits |= LoopCountFlag;
		}
		if (!FMath::IsNearlyZero(blendOutTriggerTime)) {
			RepBits |= BlendOutTriggerTimeFlag;
		}
		if (!FMath::IsNearlyZero(inTimeToStartMontageAt)) {
			RepBits |= InTimeToStartMontageAtFlag;
		}
		if (!FMath::IsNearlyZero(sectionPercentage)){
			RepBits |= SectionPercentageFlag;
		}
		if (returnValueType != EMontagePlayReturnType::MontageLength) {
			RepBits |= ReturnValueTypeFlag;
		}
		if (key.IsValidKey()) {
			RepBits |= PredictionKeyFlag;
		}
	}
	return RepBits;
}

void FMontageData::writeOptionals(FArchive& Ar, uint16 RepBits) {
	if (RepBits & BlendInTimeFlag) {
		readWriteFloat(Ar, blendInTime);
	}
	if (RepBits & BlendOutTimeFlag) {
		readWriteFloat(Ar, blendOutTime);
	}
	if (RepBits & InPlayRateFlag) {
		readWriteFloat(Ar, inPlayRate);
	}
	if (RepBits & LoopCountFlag) {
		Ar << loopCount;
	}
	if (RepBits & BlendOutTriggerTimeFlag) {
		readWriteFloat(Ar, blendOutTriggerTime);
	}
	if (RepBits & InTimeToStartMontageAtFlag) {
		readWriteFloat(Ar, inTimeToStartMontageAt);
	}
	if (RepBits & SectionPercentageFlag) {
		readWriteFloat(Ar, inTimeToStartMontageAt);
	}
	if (RepBits & ReturnValueTypeFlag) {
		Ar << returnValueType;
	}
}

void FMontageData::readWriteFloat(FArchive& Ar, float floaty)
{
	if (Ar.IsLoading()) {
		ReadFixedCompressedFloat<8, 16>(floaty, Ar);
	}
	else {
		WriteFixedCompressedFloat<8, 16>(floaty, Ar);
	}
}



//Get Path
static FORCEINLINE FName GetObjPath(const UObject* Obj) {
	if (!Obj) {
		return NAME_None;
	}
	FStringAssetReference ThePath = FStringAssetReference(Obj);

	if (!ThePath.IsValid()) {
		return NAME_None;
	}

	//The Class FString Name For This Object
	FString Str = Obj->GetClass()->GetDescription();

	Str.AppendChar('\'');
	Str += ThePath.ToString();
	Str.AppendChar('\'');

	return FName(*Str);
}

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer) 
		: 
	Super(ObjectInitializer)
	, CachedHealthComponent(nullptr)
{

	EnchantmentComponent = CreateDefaultSubobject<UEnchantmentComponent>(TEXT("EnchantmentComponent"));	
	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBarComponent"));
	BlockTriggerComponent = CreateDefaultSubobject<UBlockTriggerComponent>(TEXT("BlockTriggerComponent"));
	

	AbilitySystem = CreateDefaultSubobject<UDungeonsAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystem->SetIsReplicated(true);

	auto witherVisualization = CreateDefaultSubobject<UWitherPoisonVisualizationComponent>(TEXT("WitherPoisonVisualizationComponent"));
	witherVisualization->Deactivate();

	InvunlerabilityEffect = UInvulnerableGameplayEffect::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	tagsThatRemoveVoid.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Aoe")));
	tagsThatRemoveVoid.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Melee")));
	tagsThatRemoveVoid.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Explosion.Medium")));
	tagsThatRemoveVoid.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Explosion.Strong")));
	tagsThatRemoveVoid.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Explosion.Weak")));
	tagsThatRemoveVoid.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Ranged")));
}

void ABaseCharacter::PreInitializeComponents() {
	ACharacter::PreInitializeComponents();
	AbilitySystem->AddSet<UResistanceAttributeSet>();
	AbilitySystem->AddSet<UStatusAttributeSet>();
	AbilitySystem->AddSet<UMovementAttributeSet>();
	AbilitySystem->AddSet<UMeleeAttributeSet>();
	AbilitySystem->AddSet<URangedAttributeSet>();
	AbilitySystem->AddSet<UItemAttributeSet>();
	AbilitySystem->AddSet<UDifficultyAttributeSet>();
	AbilitySystem->AddSet<UAffectorAttributeSet>();
	AbilitySystem->AddSet<UFrozenSolidIceCubeAttributeSet>();
	AbilitySystem->AddSet<UDamageAttributeSet>();

	CachedHealthComponent = (UHealthComponent*)ACharacter::FindComponentByClass(UHealthComponent::StaticClass());
	CachedMassComponent = (UMassComponent*)ACharacter::FindComponentByClass(UMassComponent::StaticClass());
	CachedRagdollOnDeathComponent = (URagdollOnDeathComponent*)ACharacter::FindComponentByClass(URagdollOnDeathComponent::StaticClass());
}

void ABaseCharacter::GetTargetableCapsules_Implementation(TArray<UCapsuleComponent*> &outCapsules) const {
	outCapsules.Add(GetCapsuleComponent());
}

ETeamName ABaseCharacter::GetCurrentTeam() const {
	if (const auto master = GetMaster()) {
		return master->GetCurrentTeam();
	}
	return TeamName;
}

void ABaseCharacter::ApplyInvulnerability(float duration, bool bApplyCue) const {
	FGameplayEffectSpec effect;
	if (bApplyCue)
	{
		effect = effects::CreateGameplayEffectSpec<UInvulnerableGameplayEffect>(GetAbilitySystemComponent());
	}
	else
	{
		effect = effects::CreateGameplayEffectSpec<UInvulnerableNoCueGameplayEffect>(GetAbilitySystemComponent());;
	}
	effect.SetSetByCallerMagnitude("Duration", duration);
	AbilitySystem->ApplyGameplayEffectSpecToSelf(effect);
}

void ABaseCharacter::ServerApplyInvulnerability_Implementation(float duration) {
	ApplyInvulnerability(duration);
}

bool ABaseCharacter::ServerApplyInvulnerability_Validate(float duration) {
	return true;
}

void ABaseCharacter::ServerRemoveInvulnerability_Implementation() {
	RemoveInvulnerability();
}
bool ABaseCharacter::ServerRemoveInvulnerability_Validate() {
	return true;
}

void ABaseCharacter::RemoveInvulnerability() const {
	FGameplayEffectQuery query;
	query.EffectDefinition = UInvulnerableGameplayEffect::StaticClass();
	AbilitySystem->RemoveActiveEffects(query);
	query.EffectDefinition = UInvulnerableNoCueGameplayEffect::StaticClass();
	AbilitySystem->RemoveActiveEffects(query);
}

bool ABaseCharacter::HasCustomTargetableCapsules() const {
	if (TargetableCapsules.Num() > 1) 
		return true;

	// we don't have anything custom if the capsule component is the only targetable capsule
	return TargetableCapsules[0] != GetCapsuleComponent();
}

UCapsuleComponent* ABaseCharacter::GetClosestTargetableCapsule(const FVector & queryLocation) const {
	if (TargetableCapsules.Num() == 1)
		return TargetableCapsules[0].Get();

	UCapsuleComponent *closest = nullptr;
	float minDistSquared = std::numeric_limits<float>::max();
	for (const auto &capsule : GetCachedTargetableCapsules()) {
		if (capsule.IsValid()) {
			const auto distSquared = FVector::DistSquared(queryLocation, capsule->GetComponentLocation());
			if (distSquared < minDistSquared) {
				minDistSquared = distSquared;
				closest = capsule.Get();
			}
		}
	}

	return closest;
}

const TArray<TWeakObjectPtr<UCapsuleComponent>>& ABaseCharacter::GetCachedTargetableCapsules() const {
	return TargetableCapsules;
}

ABaseCharacter* ABaseCharacter::GetMaster() const {
	return Master.Get();
}

bool ABaseCharacter::IsFriendlyTowards(const ABaseCharacter* target) const {
	return teamquery::is::friendly(GetCurrentTeam(), target->GetCurrentTeam());	
}

bool ABaseCharacter::CanDamageTarget(const ABaseCharacter* target) const {
	return teamquery::can::damage(GetCurrentTeam(), target->GetCurrentTeam());
}

bool ABaseCharacter::CanHealTarget(const ABaseCharacter* target) const {
	return teamquery::can::heal(GetCurrentTeam(), target->GetCurrentTeam());
}

bool ABaseCharacter::IsHostileTowards(const ABaseCharacter* target) const {
	return teamquery::is::hostile(GetCurrentTeam(), target->GetCurrentTeam());
}

void ABaseCharacter::BeginPlay() {
	Super::BeginPlay();

	InstanceGuid = FGuid::NewGuid();
	InstanceTracker< ABaseCharacter >::AddInstance(GetWorld(),this);
	
	AnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance());

	Particles = ACharacter::FindComponentByClass<class UBaseParticleAssetsComponent>();

	if (auto hc = GetHealthComponent()) {		
		hc->OnHeal.AddUObject(this, &ABaseCharacter::HandleHeal);
		hc->OnDamageReceived.AddUObject(this, &ABaseCharacter::HandleDamageReceived);
		hc->OnDamageTypeReceived.AddUObject(this, &ABaseCharacter::HandleDamageTagReceived);
		hc->OnDamageReceivedWithType.AddUObject(this, &ABaseCharacter::OnCharacterDamageReceivedWithType);
	}	

	auto capsuleComponent = GetCapsuleComponent();
	
	//check to see if we are immune?
	auto ImmunityTag = FGameplayTag::RequestGameplayTag("Immunity.PushVolume");
	auto* pAbilityComponent = GetAbilitySystemComponent();
	if (pAbilityComponent)
	{
		pAbilityComponent->RegisterAndCallGameplayTagEvent(ImmunityTag, FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &ABaseCharacter::OnPushImmunityChanged));	
		capsuleComponent->SetCollisionResponseToChannel((ECollisionChannel)ECustomTraceChannels::PushObject, (GetAbilitySystemComponent()->GetTagCount(ImmunityTag) > 0) ? ECollisionResponse::ECR_Ignore : ECollisionResponse::ECR_Overlap);
	}
	else
	{
		//enable push detection
		capsuleComponent->SetCollisionResponseToChannel((ECollisionChannel)ECustomTraceChannels::PushObject, ECollisionResponse::ECR_Overlap);
	}

	DefaultCapsuleCollisionResponse = capsuleComponent->GetCollisionResponseToChannels();

#if !PLATFORM_WINDOWS
	//remove near miss target select on consoles to reduce overlap overhead
	auto components = GetComponentsByTag(USphereComponent::StaticClass(), FName("NearMissTargetSelect"));
	for (auto* pNearMissComp : components)
	{
		if(pNearMissComp)
			pNearMissComp->DestroyComponent();
	}
#endif

	if (const auto nearMiss = GetNearMissComponent()) {
		CachedNearMissCollisionResponse = nearMiss->GetCollisionResponseToChannels();
	}

	Game = actorquery::getFirstActor<AGameBP>(GetWorld());

	const auto mat = GetMesh()->GetMaterial(0);
	if (mat != nullptr) {
		auto* dynMatInstance = Cast<UMaterialInstanceDynamic>(mat);
		if (dynMatInstance == nullptr) {
			dynMatInstance = UMaterialInstanceDynamic::Create(mat, this);
			GetMesh()->SetMaterial(0, dynMatInstance);
		}
	}

	// #D11.CM - Logic for spawning frozen solid ice block
	AbilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.Immobile.FrozenSolid")).AddUObject(this, &ABaseCharacter::OnFrozenSolid);
	AbilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.GhostWalk")).AddUObject(this, &ABaseCharacter::OnGhostWalk);
	AbilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.Bubbled")).AddUObject(this, &ABaseCharacter::OnBubbled);
	AbilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.Levitation")).AddUObject(this, &ABaseCharacter::OnLevitate);
	AbilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("Immunity.Damage")).AddUObject(this, &ABaseCharacter::OnDamageImmunity);

	const auto movementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (movementComponent) {
		setIntendedAnimationSpeed(movementComponent->MaxWalkSpeed);
	} else {
		setIntendedAnimationSpeed(MaxSpeed);
	}

	if (HasAuthority() && InitialTimeToLive > 0.f) {
		SetTimeToLive(InitialTimeToLive);
	}

	if (Master.IsValid()) {
		Master->AcquireMinion(this);
	}

	//disable push functionality by default
	EnablePushVolumes(false);

	backpackComponent = componentutils::GetComponentByTag<UChildActorComponent>(this, "Backpack");

	if (backpackComponent && !backpackComponent->GetChildActorClass())
	{
		//Detach so we dont constantly update it for no reason
		SetBackPackComponentAttached(false);
	}

}

void ABaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);	
	InstanceTracker< ABaseCharacter >::RemoveInstance(GetWorld(),this);
}

void ABaseCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	// Update step and overlap materials

	if (Game.IsValid() && Game->GetGame()) {
		const game::TilePtr currentTile = Game->GetGame()->tiles().getTile(*this);
		if(currentTile) {
			LastValidTile = currentTile;
		}

		UMovementComponent* pMoveComp = GetMovementComponent();

		const UPlayerCharacterMovementComponent* playerCharacterMovementComp = Cast<UPlayerCharacterMovementComponent>(pMoveComp);
		const bool usingElytra = playerCharacterMovementComp ? playerCharacterMovementComp->IsMovingWithElytra() : false;

		if (!currentTile && usingElytra) {
			const FVector actorLocation = GetActorLocation();
			const FBox bounds = LastValidTile->bounds();
			const FVector directionToBounds = (bounds.GetClosestPointTo(actorLocation) - actorLocation) * FVector(1,1,0.0f);

			GetCharacterMovement()->AddImpulse(directionToBounds, true);
		}

		UpdateHitFlash();

		if (
			HasAuthority() &&
			WorldState == ECharacterWorldState::InWorld &&
			GetHealthComponent() &&
			currentTile == nullptr &&
			!usingElytra
		) {
			switch (damagehelpers::evaluateBoundsCheckPenalty(Game->GetGame()->tiles(), *this)) {
				case ECharacterBoundsCheckPenalty::Death:        GetHealthComponent()->Kill(); break;
				case ECharacterBoundsCheckPenalty::KillZoneLike: damagehelpers::tryKillByKillZone(*this, false); break;
				case ECharacterBoundsCheckPenalty::Nothing: break;
			}
		}
	}
}

UActorComponent* ABaseCharacter::FindComponentByClass(const TSubclassOf<UActorComponent> ComponentClass) const
{
	UClass* pTargetClass = ComponentClass.Get();

	//D11.SC may replace this with a Map based on class as key really dont like else-if blocks :/
	if (pTargetClass == UCapsuleComponent::StaticClass())
	{
		return GetCapsuleComponent();
	}	
	else if ( pTargetClass == UMovementComponent::StaticClass())
	{
		return GetMovementComponent();
	}
	else if (pTargetClass == UEnchantmentComponent::StaticClass())
	{
		return EnchantmentComponent;
	}
	else if (pTargetClass == UHealthBarComponent::StaticClass())
	{
		return GetHealthBarComponent();
	}
	else if (pTargetClass == UPushVolumeReactiveComponent::StaticClass())
	{
		return GetPushVolumeReactiveComponent();
	}
	else if (pTargetClass == UHealthComponent::StaticClass()) {
		return GetHealthComponent();
	}
	else if (pTargetClass == UMassComponent::StaticClass() && CachedMassComponent.IsValid())
	{
		return CachedMassComponent.Get();
	}

	return ACharacter::FindComponentByClass(ComponentClass);
}

UPushVolumeReactiveComponent* ABaseCharacter::GetPushVolumeReactiveComponent() const
{
	return PushVolumeReactiveComponent;
}

UPushVolumeReactiveComponent* ABaseCharacter::GetOrCreatePushVolumeReactiveComponent(bool CreateEnabled)
{
	if (!PushVolumeReactiveComponent)
	{
		//create it, and its movement component
		PushVolumeReactiveComponent = NewObject<UPushVolumeReactiveComponent>(this, TEXT("PushVolumeReactiveComponent"));
		PushVolumeReactiveComponent->OnPushForceUpdated.AddDynamic(this, &ABaseCharacter::UpdatePushVolumeMovement);
		PushVolumeReactiveComponent->OnReceiverDeactivated.AddDynamic(this, &ABaseCharacter::OnPushRecieverDeactivated);

		float pushXY, pushZ = 0;
		GetCapsuleComponent()->GetScaledCapsuleSize(pushXY, pushZ);
		FVector PushBoxExtent{ pushXY, pushXY, pushZ };
		PushVolumeReactiveComponent->SetBoxExtent(PushBoxExtent);		
		PushVolumeReactiveComponent->RegisterComponent();
		

		PushVolumeMovementComponent = NewObject<USimpleMovementComponent>(this, TEXT("PushVolumeSimpleMovementComponent"));
		PushVolumeMovementComponent->bShouldSlide = true;
		PushVolumeMovementComponent->bShouldAccelerate = true;
		PushVolumeMovementComponent->RegisterComponent();

		if (CreateEnabled)
		{
			PushVolumeReactiveComponent->SetupAttachment(GetCapsuleComponent());
		}
		else
		{
			PushVolumeReactiveComponent->SetActive(false);
			PushVolumeReactiveComponent->SetVolumeOverlapsEnabled(false);
			PushVolumeMovementComponent->SetActive(false);
			PushVolumeMovementComponent->SetComponentTickEnabled(false);
		}
	}

	return PushVolumeReactiveComponent;
}

UHealthBarComponent* ABaseCharacter::GetHealthBarComponent() const
{
	return HealthBarComponent;
}

UHealthComponent* ABaseCharacter::GetHealthComponent() const
{
	return CachedHealthComponent.Get();
}

URagdollOnDeathComponent* ABaseCharacter::GetRagdollOnDeathComponent() const
{
	return CachedRagdollOnDeathComponent.Get();
}


void ABaseCharacter::PlayMeleeAttackVariantAnimation(uint8 index, float animationPlayRate) {
	if (auto melee = FindComponentByClass<UMeleeAttackComponent>()) {
		melee->PlayAttackVariantMontage(index, animationPlayRate);
	}
}

void ABaseCharacter::RemoveInvisibility() const {
	const FGameplayTagContainer tagContainer({ FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Invisible")) });
	const auto query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(tagContainer);
	AbilitySystem->RemoveActiveEffects(query);

	DungeonsGearUtilLibrary::OnInvisibilityRemoved(this);
}

bool ABaseCharacter::TryRemoveVoidAffliction(const FGameplayTagContainer& tags) const
{	
	bool didRemoveVoid = false;

	if (tags.HasAnyExact(tagsThatRemoveVoid))
	{
		if (AbilitySystem->RemoveActiveEffectsWithAppliedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.VoidStrike")))) > 0)
		{
			AbilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.VoidStrike.Expunged")), FGameplayCueParameters());
			didRemoveVoid = true;
		}

		if (AbilitySystem->RemoveActiveEffectsWithAppliedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.VoidTouched")))) > 0)
		{
			AbilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.VoidTouched.Expunged")), FGameplayCueParameters());
			didRemoveVoid = true;
		}

		if (AbilitySystem->RemoveActiveEffectsWithAppliedTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Voided")))) > 0)
		{
			AbilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Voided.Expunged")), FGameplayCueParameters());
			didRemoveVoid = true;
		}
	}

	return didRemoveVoid;
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseCharacter, AttackState);
	DOREPLIFETIME(ABaseCharacter, WorldState);
	DOREPLIFETIME(ABaseCharacter, EnchantmentComponent);
	DOREPLIFETIME(ABaseCharacter, LookAtActor);
	DOREPLIFETIME_CONDITION(ABaseCharacter, IntendedAnimationSpeed, COND_SkipOwner);
	DOREPLIFETIME(ABaseCharacter, Master);
	DOREPLIFETIME(ABaseCharacter, Targetable);
}

void ABaseCharacter::Kill() const {
	if (auto* health = GetHealthComponent()) {
		health->Kill();
	}
}

void ABaseCharacter::Falling() {
	Super::Falling();
	const auto currentLocation = GetActorLocation();

	if (!FallFromLocation.IsSet() || currentLocation.Z > FallFromLocation->Z) {
		FallFromLocation = currentLocation;
	}
}


void ABaseCharacter::Landed(const FHitResult& Hit) {
	Super::Landed(Hit);
	if (!FallFromLocation) {
		return;
	}

	const auto currentLocation = GetActorLocation();
	const auto deltaZ = FallFromLocation->Z - currentLocation.Z;
	
	if (IsFrozenSolid()) {
		if (OnFrozenLandedSound != nullptr) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), OnFrozenLandedSound, GetActorLocation());
		}
	}
	else if (OnLandedSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), OnLandedSound, GetActorLocation());
	}

	static const auto leviTag = FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.LevitationFalling"));

	if (HasAuthority() && !AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Underwater")))) {
		const FloatRange damageDeltaRange(400.f, 3000.f);
		const FloatRange percentageRange(0.1f, 0.75f);
		const auto damagePercentage = deltaZ > damageDeltaRange.min() ? percentageRange.lerp(damageDeltaRange.clampedFractionAt(deltaZ)) : 0.f;

		const bool isLevitating = AbilitySystem->HasMatchingGameplayTag(leviTag);

		if (damagePercentage > 0 || isLevitating) {
			const auto* health = GetHealthComponent();
			if (health && health->IsAlive()) {
				const auto landedOnActor = Hit.GetActor();

				FGameplayEffectSpec spec;

				if (isLevitating)
				{
					FGameplayEffectQuery query;
					query.EffectDefinition = ULevitationFallGameplayEffect::StaticClass();
					auto effectHandles = AbilitySystem->GetActiveEffects(query);
					if (effectHandles.Num() > 0)
					{
						auto effect = AbilitySystem->GetActiveGameplayEffect(effectHandles[0]);
						AActor* playerActor = effect->Spec.GetContext().GetInstigator();
						const auto damage = health->GetMaximumHealth() * effect->Spec.GetSetByCallerMagnitude(ULevitationGameplayEffect::EffectMagnitudeName);
						spec = effects::CreateGameplayEffectSpec<ULevitationFallDamageEffect>(GetAbilitySystemComponent(), effects::HealthName, -damage, playerActor ? playerActor : landedOnActor, landedOnActor, GetActorLocation(), 1.f);
					}
				}
				else
				{
					const auto damage = health->GetMaximumHealth() * damagePercentage;
					spec = effects::CreateGameplayEffectSpec<UFallDamageEffect>(GetAbilitySystemComponent(), effects::HealthName, -damage, landedOnActor, landedOnActor, Hit.Location, 1.f);
				}

				if (spec.Def)
				{
					AbilitySystem->ApplyGameplayEffectSpecToSelf(spec);
				}
			}
		}
	}

	AbilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer(leviTag)); //remove levitating effect even if we didn't take damage
	static const auto SELevitation = FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Levitation"));
	AbilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer(SELevitation));

	FallFromLocation.Reset();
	OnCharacterWalking.Broadcast();
}

void ABaseCharacter::OnAddedToLevel() {
	FallFromLocation.Reset();
}

void ABaseCharacter::PullCharacter_Implementation(FVector DeltaLocation, bool bSweep, FHitResult OutSweepHitResult, ETeleportType Teleport)
{
	if (IsLocallyControlled()) {
		FHitResult newHitResult = OutSweepHitResult;
		AddActorWorldOffset(DeltaLocation, bSweep, &newHitResult, Teleport);
	}
}

void ABaseCharacter::OnInvisibilityGained()
{
	EnchantmentComponent->OnInvisibilityGained();
}

float ABaseCharacter::GetMaxSpeed() const {
	return MaxSpeed;
}

float ABaseCharacter::GetAnimationWalkSpeed() const {
	return AnimationWalkSpeed;
}

void ABaseCharacter::setIntendedAnimationSpeed(int16 speed) {
	IntendedAnimationSpeed = speed;
}

int16 ABaseCharacter::getIntendedAnimationSpeed() const {
	return IntendedAnimationSpeed;
}

bool ABaseCharacter::IsMoving() const {
	return (GetVelocity().SizeSquared() > 0.f);
}

void ABaseCharacter::HandleDeath() {
	SCOPE_CYCLE_COUNTER(STAT_OnCharacterDeath)

	OnDeath.Broadcast();
	OnCharacterDeath();

	RemoveActiveEffects();
}

void ABaseCharacter::HandleRevive() {
	OnRevive.Broadcast();
	OnCharacterRevive();
}

void ABaseCharacter::HandleHeal(float amount) {
	OnCharacterHeal(amount);
}

void ABaseCharacter::HandleDamageReceived(float amount) {
	OnCharacterDamageReceived(amount);
}

void ABaseCharacter::HandleDamageTagReceived(float amount, const FGameplayTagContainer& tag) {
	OnCharacterDamageReceivedWithTag(amount, tag);

	TryRemoveVoidAffliction(tag);
}

void ABaseCharacter::LifeSpanExpired()
{

	if (AGameBP* game = actorquery::getFirstActor<AGameBP>(GetWorld())) 
	{		
		SetActorTickEnabled(false);

		TArray<UActorComponent*> Components;
		GetComponents(Components, true);

		//disable all components
		for (auto* pComponent : Components)
		{
			if (pComponent->IsComponentTickEnabled())
			{
				pComponent->SetComponentTickEnabled(false);
			}
		}
		
		//push to character cleanup
		game->RequestCharacterDestroy(this);

		//awake net dormancy so that destruction of actor is replicated to clients
		SetNetDormancy(ENetDormancy::DORM_Awake);

		//dont bother tracking this any more, its dying
		RemoveInstanceTracking();
	}
	else
	{
		//no game bp, just do base expiry
		Super::LifeSpanExpired();
	}
}

#define DISSASEMBLE_COMPONENT_PER_CALL 10

bool ABaseCharacter::DissasembleCharacter()
{
	TArray<UActorComponent*> Components;
	GetComponents(Components, true);

	int iComponent = Math::min(Components.Num(), DISSASEMBLE_COMPONENT_PER_CALL);
	bool bComplete = (Components.Num() <= DISSASEMBLE_COMPONENT_PER_CALL);

	while (iComponent--)
	{
		UActorComponent* pThisComponent = Components[iComponent];

		//leave Characters health and ability and root components for the destructor for safety, they are used for validation in a lot of places
		if(pThisComponent != AbilitySystem && pThisComponent != CachedHealthComponent && pThisComponent != RootComponent)
			pThisComponent->DestroyComponent();
	}
	
	return bComplete;
}


void ABaseCharacter::RemoveInstanceTracking()
{
	InstanceTracker< ABaseCharacter >::RemoveInstance(GetWorld(), this);
}

void ABaseCharacter::OnGhostWalk(const FGameplayTag tag, const int32 tagCount) {
	SetCapsuleCollisionResponse( tagCount > 0 ? ESynchornizedCollisionResponse::WorldOnly : ESynchornizedCollisionResponse::Default);
}

void ABaseCharacter::OnBubbled(const FGameplayTag tag, const int32 tagCount) {
	if (HasAuthority()) {
		if (tagCount > 0) {
			if (UHealthComponent* HealthComponent = FindComponentByClass<UHealthComponent>()) {
				BubbledHandle = HealthComponent->OnDamageReceived.AddUObject(this, &ABaseCharacter::OnAttackedWhileBubbled);
			}
		}
	}
}

void ABaseCharacter::OnLevitate(const FGameplayTag tag, const int32 tagCount) {
	if (HasAuthority()) {
		if (tagCount > 0) {
			//tag added
			if (UMassComponent* MassComponent = FindComponentByClass<UMassComponent>()) {
				MassComponent->SetAdditionalMultiplier(0.0f);
			}
		}
		else
		{
			//tag removed
			if (UMassComponent* MassComponent = FindComponentByClass<UMassComponent>()) {
				MassComponent->SetAdditionalMultiplier(1.0f);
			}
			FallFromLocation.Reset();
			Falling();
		}
	}
}

void ABaseCharacter::OnDamageImmunity(const FGameplayTag tag, const int32 tagCount)
{
	OnDamageImmunityChanged.Broadcast(tagCount > 0);
}

void ABaseCharacter::OnAttackedWhileBubbled(float damagedReceived) {
	GetWorld()->GetTimerManager().SetTimerForNextTick([=]() {
		FGameplayTagContainer tag;
		tag.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Bubbled"));
		AbilitySystem->RemoveActiveEffectsWithTags(tag);
	});

	if (UHealthComponent* HealthComponent = FindComponentByClass<UHealthComponent>()) {
		HealthComponent->OnDamageReceived.Remove(BubbledHandle);
	}
}

void ABaseCharacter::OnFrozenSolid(const FGameplayTag tag, const int32 tagCount) {
	if (HasAuthority()) {
		if (tagCount > 0) {
			// We have more than one instance of the gameplay effect, so spawn the gameplay actor if we have one.
			if (FrozenSolidIceCubeActor != nullptr) {
				currentIceBlock = Cast<AFrozenSolidIceCubeActor>(GetWorld()->SpawnActor(FrozenSolidIceCubeActor));
				FAttachmentTransformRules attachRules{ EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false };
				currentIceBlock->SetOwner(this);
				currentIceBlock->AttachToActor(this, attachRules);
			}
		}
		else if(currentIceBlock) {
			// We have removed a tag, so remove our iceblock if we have one.
			currentIceBlock->Destroy(true);
			currentIceBlock = nullptr;
		}
	}
}

void ABaseCharacter::SpawnEffectsAtLocation_Implementation(UParticleSystem* emitter, USoundCue* sound, bool attached) {

	if (emitter != nullptr) {
		if (attached)
			UGameplayStatics::SpawnEmitterAttached(emitter, RootComponent);
		else
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), emitter, GetActorLocation());
	}
		
	if (sound != nullptr) {
		if (attached)
			UGameplayStatics::SpawnSoundAttached(sound, RootComponent);
		else
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), sound, GetActorLocation());
	}

}

//
// Multicast Particle Spawning
//
void ABaseCharacter::SpawnAttachedParticles_Implementation(UParticleSystem* particles, FVector offset/* = FVector::ZeroVector*/, float duration/* = 0.f*/, bool fadeOut/* = true*/) {
	const auto particleComponent = UGameplayStatics::SpawnEmitterAttached(particles, RootComponent, NAME_None, offset);
	
	if (duration > 0.f) {
		FTimerHandle timerHandle;
		const auto timerDelegate(fadeOut ? FTimerDelegate::CreateUObject(particleComponent, &UParticleSystemComponent::Deactivate) : FTimerDelegate::CreateUObject(particleComponent, &UParticleSystemComponent::DestroyComponent, false));
		GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDelegate, duration, false);
	}
}

EMaterialTypeEnum ABaseCharacter::GetStepMaterial() {
	return StepMaterial;
}

EMaterialTypeEnum ABaseCharacter::GetOverlapMaterial() {
	return OverlapMaterial;
}

void ABaseCharacter::SetStepMaterial(EMaterialTypeEnum material) {
	if (material != StepMaterial) {
		StepMaterial = material;
		OnStepMaterialChanged.Broadcast(material);
	}
}

void ABaseCharacter::SetModifierSurface(EMaterialTypeEnum surface) {
	OnModifierSurfaceChanged.Broadcast(surface);
}

void ABaseCharacter::SetOverlapMaterial(EMaterialTypeEnum material) {
	if (material != OverlapMaterial){
		OverlapMaterial = material;
		OnOverlapMaterialChanged.Broadcast(material);
	}
}

TWeakObjectPtr<ABaseCharacter> ABaseCharacter::GetLastInjuredBy() const {
	return LastInjuredBy;
}

void ABaseCharacter::SetLastInjuredBy(ABaseCharacter* lastInjuredBy) {
	LastInjuredBy = lastInjuredBy;
}

void ABaseCharacter::SetTimeToLive(float timeToLive) {
	FTimerManager& timerManager = GetWorld()->GetTimerManager();
	if (DelayedKillHandle.IsValid()) {
		timerManager.ClearTimer(TimerHandle_LifeSpanExpired);
		TimerHandle_LifeSpanExpired.Invalidate();
	}

	InitialTimeToLive = timeToLive;

	if (timeToLive > 0.f) {
		ABaseCharacter* baseThis = this;
		timerManager.SetTimer(DelayedKillHandle, FTimerDelegate::CreateUObject(baseThis, &ABaseCharacter::Kill), InitialTimeToLive, false);
	}
}

float ABaseCharacter::GetTimeToLive() const {
	if (UWorld* World = GetWorld())
	{
		// Timer remaining returns -1.0f if there is no such timer - return this as ZERO
		const float CurrentLifespan = World->GetTimerManager().GetTimerRemaining(DelayedKillHandle);
		return (CurrentLifespan != -1.0f) ? CurrentLifespan : 0.0f;
	}

	return 0.0f;
}

bool ABaseCharacter::IsLocationReachable(const FVector& target) const {
	if (Game.IsValid()) { 
		const auto terrain = Game->GetTerrain();

		if (!terrain->getType(conversion::ueToTerrain(target)).isReachable()) {
			return false;
		}
	}


	const auto world = GetWorld();

	const auto start = GetActorLocation();
	const auto startProjected = UNavigationSystemV1::ProjectPointToNavigation(world, start);
	const auto targetProjected = UNavigationSystemV1::ProjectPointToNavigation(world, target);

	if (conversion::ueToBlock(startProjected) == conversion::ueToBlock(targetProjected)) {
		return true;
	}


	const UNavigationSystemV1* navigationSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(world);
	const INavAgentInterface* navigationAgent = Cast<INavAgentInterface>(this);
	const FNavAgentProperties& agentProps = navigationAgent->GetNavAgentPropertiesRef();
	const ANavigationData* navigationData = navigationSystem->GetNavDataForProps(agentProps);

	FPathFindingQuery query { *navigationAgent, *navigationData, startProjected, targetProjected };
	query.SetAllowPartialPaths(false);

	return navigationSystem->TestPathSync(query, EPathFindingMode::Hierarchical);
}

void ABaseCharacter::SetCapsuleCollisionResponse(ESynchornizedCollisionResponse response) {
	auto capsule = GetCapsuleComponent();
	switch (response) {
	case ESynchornizedCollisionResponse::Default:
		capsule->SetCollisionResponseToChannels(DefaultCapsuleCollisionResponse);
		break;
	case ESynchornizedCollisionResponse::WorldOnly:
		capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		capsule->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(ECustomTraceChannels::PlayerPawn), ECR_Ignore);
		break;
	}
}

void ABaseCharacter::PlayMontageSectionWithBlendAtSectionPercentage(UAnimMontage* MontageToPlay, FName SectionName, float InPlayRate, EMontagePlayReturnType ReturnValueType, float InTimeToStartMontageAt, float BlendInTime, float SectionPercentage) {
	MontageToPlay->BlendIn.SetBlendTime(BlendInTime); //Actually overriding the MontageBlendData - a bit dangerous......
	const auto sectionIndex = MontageToPlay->GetSectionIndex(SectionName);
	if (MontageToPlay->IsValidSectionIndex(sectionIndex)) {
		auto& curSection = MontageToPlay->GetAnimCompositeSection(sectionIndex);
		const auto sectionLength = MontageToPlay->GetSectionLength(sectionIndex);
		const auto newPosition = curSection.GetTime() + SectionPercentage * sectionLength;
		PlayMontage(MontageToPlay, InPlayRate, ReturnValueType, newPosition);
	}
	else {
		PlayMontage(MontageToPlay, InPlayRate, ReturnValueType, 0.0f);
		UE_LOG(LogTemp, Warning, TEXT("Section missing in animation montage: %s"), *SectionName.ToString());
	}
}




void ABaseCharacter::StopSlotAnimation(FName slotName, float blendOutTime) {
	USkeletalMeshComponent* mesh = GetMesh();
	check(mesh && "Character does not have mesh.");
	if (UAnimInstance* animInstance = mesh->GetAnimInstance()) {
		animInstance->StopSlotAnimation(blendOutTime, slotName);
	}
}

UAnimMontage* ABaseCharacter::PlayAnimationAsDynamicMontage(UAnimSequenceBase* Asset, FName SlotNodeName, float BlendInTime, float BlendOutTime, float InPlayRate, int32 LoopCount, float BlendOutTriggerTime, float InTimeToStartMontageAt) {
	USkeletalMeshComponent* mesh = GetMesh();
	check(mesh && "Character does not have mesh.");
	if (UAnimInstance* animInstance = mesh->GetAnimInstance()) {
		UAnimMontage* dynamicMontage = animInstance->PlaySlotAnimationAsDynamicMontage(Asset, SlotNodeName, BlendInTime, BlendOutTime, InPlayRate, LoopCount < 0 ? 1 : LoopCount, BlendOutTriggerTime, InTimeToStartMontageAt);
		if (LoopCount < 0) {
			animInstance->Montage_SetNextSection(FName(TEXT("Default")), FName(TEXT("Default")), dynamicMontage);
		}
		return dynamicMontage;
	}
	return nullptr;
}


float ABaseCharacter::PlayMontage(UAnimMontage* MontageToPlay, float InPlayRate, EMontagePlayReturnType ReturnValueType, float InTimeToStartMontageAt) {
	USkeletalMeshComponent* mesh = GetMesh();
	check(mesh && "Character does not have mesh.");
	if (mesh)
	{
		UAnimInstance* animInstance = mesh->GetAnimInstance();
		if (animInstance)
		{
			auto returnDuration = animInstance->Montage_Play(MontageToPlay, InPlayRate, ReturnValueType, InTimeToStartMontageAt);
			return returnDuration;
		}
	}
	return 0.0f;
}

void ABaseCharacter::StopMontage(float InBlendOutTime, const UAnimMontage* Montage/* = NULL*/) {
	USkeletalMeshComponent* mesh = GetMesh();
	check(mesh && "Character does not have mesh.");
	if (UAnimInstance* animInstance = mesh->GetAnimInstance()) {
		animInstance->Montage_Stop(InBlendOutTime, Montage);
	}
}

void ABaseCharacter::StopAnimation(float InBlendOutTime, UAnimSequenceBase* Asset, FName SlotNodeName)
{
	USkeletalMeshComponent* mesh = GetMesh();
	check(mesh && "Character does not have mesh.");
	if (UAnimInstance* animInstance = mesh->GetAnimInstance()) {
		UAnimMontage* OutMontage;
		if (animInstance->IsPlayingSlotAnimation(Asset, SlotNodeName, OutMontage)) {
			animInstance->StopSlotAnimation(InBlendOutTime, SlotNodeName);
		}
	}
}

void ABaseCharacter::PauseMontage(const UAnimMontage* Montage) {
	USkeletalMeshComponent* mesh = GetMesh();
	check(mesh && "Character does not have mesh.");
	if (UAnimInstance* animInstance = mesh->GetAnimInstance()) {
		animInstance->Montage_Pause(Montage);
	}
}

void ABaseCharacter::ResumeMontage(const UAnimMontage* Montage) {
	USkeletalMeshComponent* mesh = GetMesh();
	check(mesh && "Character does not have mesh.");
	if (UAnimInstance* animInstance = mesh->GetAnimInstance()) {
		animInstance->Montage_Resume(Montage);
	}
}

void ABaseCharacter::JumpToSectionMontage(FName SectionName, const UAnimMontage* Montage) {
	USkeletalMeshComponent* mesh = GetMesh();
	check(mesh && "Character does not have mesh.");
	if (UAnimInstance* animInstance = mesh->GetAnimInstance()) {
		animInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

void ABaseCharacter::SetPlayrateMontage(const UAnimMontage* Montage, float NewPlayRate) {
	USkeletalMeshComponent* mesh = GetMesh();
	check(mesh && "Character does not have mesh.");
	if (UAnimInstance* animInstance = mesh->GetAnimInstance()) {
		animInstance->Montage_SetPlayRate(Montage, NewPlayRate);
	}
}

void ABaseCharacter::JumpToSectionMontageWithPlayrate(FName SectionName, float InPlayRate, const UAnimMontage* Montage) {
	SetPlayrateMontage(Montage, InPlayRate);
	JumpToSectionMontage(SectionName, Montage);
}

void ABaseCharacter::MulticastPlayMontage(UAnimMontage* MontageToPlay, float InPlayRate /* = 1.f */, EMontagePlayReturnType ReturnValueType /* = EMontagePlayReturnType::MontageLength */, float InTimeToStartMontageAt /* = 0.f */, FPredictionKey predictionKey /* = FPredictionKey() */) 
{
	FMontageData data;
	data.asset = MontageToPlay;
	data.inPlayRate = InPlayRate;
	data.returnValueType = ReturnValueType;
	data.inTimeToStartMontageAt = InTimeToStartMontageAt;
	data.key = predictionKey;
	MulticastPlayMontagePacked(data);
}

void ABaseCharacter::MulticastPlayMontagePacked_Implementation(const FMontageData& data)
{
	if (!data.key.IsLocalClientKey() || HasAuthority()) {
		PlayMontage(Cast<UAnimMontage>(data.asset), data.inPlayRate, data.returnValueType, data.inTimeToStartMontageAt);
	}
}

void ABaseCharacter::MulticastStopMontage_Implementation(float InBlendOutTime, const UAnimMontage* Montage /* = NULL */, FPredictionKey predictionKey /* = FPredictionKey() */) {
	if (!predictionKey.IsLocalClientKey() || HasAuthority()) {
		StopMontage(InBlendOutTime, Montage);
	}
}

void ABaseCharacter::MulticastStopAnimation_Implementation(float InBlendOutTime, UAnimSequenceBase* Asset, FName SlotNodeName, FPredictionKey predictionKey /*= FPredictionKey()*/){
	if (!predictionKey.IsLocalClientKey() || HasAuthority()) {
		StopAnimation(InBlendOutTime, Asset, SlotNodeName);
	}
}

void ABaseCharacter::MulticastPauseMontage_Implementation(const UAnimMontage* Montage /* = NULL */, FPredictionKey predictionKey /* = FPredictionKey() */) {
	if (!predictionKey.IsLocalClientKey() || HasAuthority()) {
		PauseMontage(Montage);
	}
}

void ABaseCharacter::MulticastResumeMontage_Implementation(const UAnimMontage* Montage /* = NULL */, FPredictionKey predictionKey /* = FPredictionKey() */) {
	if (!predictionKey.IsLocalClientKey() || HasAuthority()) {
		ResumeMontage(Montage);
	}
}

void ABaseCharacter::MulticastPlayMontageSectionWithBlendAtSectionPercentage(UAnimMontage* MontageToPlay, FName SectionName, float InPlayRate /* = 1.f */, EMontagePlayReturnType ReturnValueType /* = EMontagePlayReturnType::MontageLength */, float InTimeToStartMontageAt /* = 0.f */, float BlendInTime, float SectionPercentage, FPredictionKey predictionKey /* = FPredictionKey() */) {
	MulticastPlayMontageSectionWithBlendAtSectionPercentagePacked({ MontageToPlay, SectionName, BlendInTime, 0.2f, InPlayRate, 1, 0.f, InTimeToStartMontageAt, SectionPercentage, ReturnValueType, predictionKey });
}

void ABaseCharacter::MulticastJumpToSectionMontageWithPlayratePacked_Implementation(const FMontageData& data)
{
	if (!data.key.IsLocalClientKey() || HasAuthority()) {
		JumpToSectionMontageWithPlayrate(data.slotNodeName, data.inPlayRate, Cast<UAnimMontage>(data.asset));
	}
}

void ABaseCharacter::MulticastPlayMontageSectionWithBlendAtSectionPercentagePacked_Implementation(const FMontageData& data)
{
	if (!data.key.IsLocalClientKey() || HasAuthority()) {
		PlayMontageSectionWithBlendAtSectionPercentage(Cast<UAnimMontage>(data.asset), data.slotNodeName, data.inPlayRate, data.returnValueType, data.inTimeToStartMontageAt, data.blendInTime, data.sectionPercentage);
	}
}

void ABaseCharacter::MulticastJumpToSectionMontage_Implementation(FName SectionName, const UAnimMontage* Montage /* = NULL */, FPredictionKey predictionKey /* = FPredictionKey() */) {
	if (!predictionKey.IsLocalClientKey() || HasAuthority()) {
		JumpToSectionMontage(SectionName, Montage);
	}
}

void ABaseCharacter::MulticastJumpToSectionMontageWithPlayrate(FName SectionName, float NewPlayRate /* = 1.f */, UAnimMontage* Montage /* = NULL */, FPredictionKey predictionKey /* = FPredictionKey() */) {
	FMontageData data;
	data.slotNodeName = SectionName;
	data.inPlayRate = NewPlayRate;
	data.asset = Montage;
	data.key = predictionKey;
	MulticastJumpToSectionMontageWithPlayratePacked(data);
}

void ABaseCharacter::MulticastSetPlayrateMontage_Implementation(const UAnimMontage* Montage, float NewPlayRate /* = 1.f */, FPredictionKey predictionKey /* = FPredictionKey() */) {
	if (!predictionKey.IsLocalClientKey() || HasAuthority()) {
		SetPlayrateMontage(Montage, NewPlayRate);
	}
}

void ABaseCharacter::MulticastPlayAnimationAsDynamicMontage(UAnimSequenceBase* Asset, FName SlotNodeName, float BlendInTime, float BlendOutTime, float InPlayRate, int32 LoopCount, float BlendOutTriggerTime, float InTimeToStartMontageAt, FPredictionKey key) {
	uint8 loopCount = 1;
	if (LoopCount < 0 || LoopCount > UINT8_MAX) {
		loopCount = UINT8_MAX;
	}
	else {
		loopCount = static_cast<uint8>(LoopCount);
	}

	MulticastPlayAnimationAsDynamicMontagePacked({ Asset, SlotNodeName, BlendInTime, BlendOutTime, InPlayRate, loopCount, BlendOutTriggerTime, InTimeToStartMontageAt, 0.0f, EMontagePlayReturnType::MontageLength, key });
}

void ABaseCharacter::MulticastPlayAnimationAsDynamicMontagePacked_Implementation(const FMontageData& data)
{
	if (!data.key.IsLocalClientKey() || HasAuthority()) {
		PlayAnimationAsDynamicMontage(data.asset, data.slotNodeName, data.blendInTime, data.blendOutTime, data.inPlayRate, data.loopCount == UINT8_MAX ? -1 : data.loopCount, data.blendOutTriggerTime, data.inTimeToStartMontageAt);
	}
}



void ABaseCharacter::MulticastStopSlotAnimation_Implementation(FName slotName, float blendOutTime, FPredictionKey predictionKey /* = FPredictionKey() */) {
	if (!predictionKey.IsLocalClientKey() || HasAuthority()) {
		StopSlotAnimation(slotName, blendOutTime);
	}
}

void ABaseCharacter::MulticastPlayMeleeAttackVariantAnimation_Implementation(uint8 index, float animationPlayRate, FPredictionKey key) {
	if (!key.IsLocalClientKey() || HasAuthority()) {
		PlayMeleeAttackVariantAnimation(index, animationPlayRate);
	}
}

void ABaseCharacter::SetNearMissSelection(const ECollisionResponse& nearMissResponse) {
	if (auto nearMiss = GetNearMissComponent()) {
		CachedNearMissCollisionResponse = nearMiss->GetCollisionResponseToChannels();
		for (const auto channel : GetNearMissSelectionChannels()) {
			nearMiss->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(channel), nearMissResponse);	
		}
	}
}

void ABaseCharacter::ResetNearMissSelection() const {
	if (auto nearMiss = GetNearMissComponent()) {
		nearMiss->SetCollisionResponseToChannels(CachedNearMissCollisionResponse);
	}
}

void ABaseCharacter::ChangeTeam(ETeamName teamName) {
	const auto previousName = TeamName;
	TeamName = teamName;
	BroadcastTeamChange(previousName, teamName);
}

void ABaseCharacter::ChangeMaster(ABaseCharacter* newMaster) {
	if (Master == newMaster) {
		return;
	}

	if (Master.IsValid()) {
		Master->ReleaseMinion(this);
	}

	Master = newMaster;
	if (Master.IsValid()) {
		Master->AcquireMinion(this);
		BroadcastTeamChange(TeamName, Master->GetCurrentTeam());
	}
}

void ABaseCharacter::AcquireMinion(ABaseCharacter* minion)
{
	// don't do anything if the minion hasn't begun play. 
	// it will call back to us when it's ready
	if (!minion->HasActorBegunPlay()) {
		return;
	}

	// skip over cosmetic pets
	if(minion->ActorHasTag("cosmetic"))	{
		return;
	}

	Minions.Add(minion);
	minion->GetHealthComponent()->OnDeath.AddUObject(this, &ABaseCharacter::ReleaseMinion, minion);
	OnMinionAcquired.Broadcast(minion);
}

void ABaseCharacter::ReleaseMinion(ABaseCharacter* minion)
{
	Minions.Remove(minion);
	if (auto hc = minion->GetHealthComponent()) {
		hc->OnDeath.RemoveAll(this);
	}
	
	OnMinionReleased.Broadcast(minion);
}

TArray<ABaseCharacter*> ABaseCharacter::GetMinions() const
{
	return Util::getRawPointers(Minions);
}

void ABaseCharacter::BroadcastTeamChange(const ETeamName& previousTeam, const ETeamName& newTeam) const {
	if (previousTeam != newTeam) {
		OnTeamChanged.Broadcast();
	}
}

static void AppearSceneComponent(USceneComponent* component) {
	if (component->IsA<UParticleSystemComponent>()) {
		component->Activate();
	} else {
		component->SetVisibility(true);
	}
}

static void DisappearSceneComponentAndAddToList(USceneComponent* component, TArray<TWeakObjectPtr<USceneComponent>>& disapperedComponents) {
	if (component->IsA<UParticleSystemComponent>()) {
		if (component->IsActive() && component->ComponentHasTag(tags::deactivateOnDisappear)) {
			component->Deactivate();
			disapperedComponents.Add(component);			
		}
	} else {		
		if (component->IsVisible()) {
			component->SetVisibility(false);
			disapperedComponents.Add(component);
		}
	}
	for (auto child : component->GetAttachChildren()) {
		if (child) { // @attn @todo @nogame @fredstefanaron @dlc4hotfix
			DisappearSceneComponentAndAddToList(child, disapperedComponents);
		}
	}
}

void ABaseCharacter::OnRep_WorldState(ECharacterWorldState previousWorldState) {
	if (previousWorldState != WorldState){
		switch (WorldState) {
		case ECharacterWorldState::InWorld:

			SetActorTickEnabled(true);
			SetActorEnableCollision(true);

			for (auto sceneComponent : componentsDisappeared) {
				if (sceneComponent.IsValid()){
					AppearSceneComponent(sceneComponent.Get());
				}
			}
			componentsDisappeared.Empty();

			if (UCharacterMovementComponent* movementComponent = FindComponentByClass<UCharacterMovementComponent>()) {
				movementComponent->SetDefaultMovementMode();
			}
			break;

		case ECharacterWorldState::Disappeared:
			SetActorEnableCollision(false);
		case ECharacterWorldState::DisappearedWithCollision:
			SetActorTickEnabled(false);

			{
				TArray<AActor*> attachedActors;
				GetAttachedActors(attachedActors);
				for (auto* attachedActor : attachedActors) {
					if (IAttachable* attachableObject = Cast<IAttachable>(attachedActor)) {
						attachableObject->Execute_AttachedToActorHidden(attachedActor, this);
					}
				}
			}

			componentsDisappeared.Empty();
			for (auto component : GetComponentsByClass(USceneComponent::StaticClass())) {
				if (const auto sceneComponent = Cast<USceneComponent>(component)) {
					DisappearSceneComponentAndAddToList(sceneComponent, componentsDisappeared);
				}
			}

			if (UCharacterMovementComponent* movementComponent = FindComponentByClass<UCharacterMovementComponent>()) {
				movementComponent->DisableMovement();
			}

			break;
		default:
			break;
		}
		OnWorldStateChanged.Broadcast();
	}
}

ECharacterWorldState ABaseCharacter::GetWorldState() const {
	return WorldState;
}

void ABaseCharacter::SetWorldState(ECharacterWorldState worldState) {
	const auto previousWorldState = WorldState;
	WorldState = worldState;
	OnRep_WorldState(previousWorldState);
}

const FGuid& ABaseCharacter::GetInstanceId_ServerOnly() const {
	return InstanceGuid;
}

void ABaseCharacter::CancelAllActions() const {
	CancelActions.Broadcast();
}

bool ABaseCharacter::IsTargetable() const {
	static FGameplayTag sUntargetable = FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Untargetable"));
	return Targetable && !AbilitySystem->HasMatchingGameplayTag(sUntargetable);
}

void ABaseCharacter::SetTargetable(bool targetable) {
	if(targetable != Targetable) {
		Targetable = targetable;
		ForceNetUpdate();
	}
}

void ABaseCharacter::DisableNearMissSelection() {
	SetNearMissSelection(ECR_Ignore);
}

USphereComponent* ABaseCharacter::GetNearMissComponent() const {
#if PLATFORM_WINDOWS
	if (!CachedNearMissComponent.IsValid()) {
		auto components = GetComponentsByTag(USphereComponent::StaticClass(), FName("NearMissTargetSelect"));
		if (components.Num() > 0)  {
			CachedNearMissComponent = Cast<USphereComponent>(components[0]);
		}
	}
	return CachedNearMissComponent.Get();
#else 
	return nullptr;
#endif
}

TArray<ECustomTraceChannels> ABaseCharacter::GetNearMissSelectionChannels() {
	return {ECustomTraceChannels::TargetSelect, ECustomTraceChannels::NearMissTargetSelect};
}

void ABaseCharacter::OnRep_Controller() {
	Super::OnRep_Controller();
	OnControllerReplicated.Broadcast();
}

void ABaseCharacter::OnPushImmunityChanged(FGameplayTag Tag, int32 tagCount)
{
	SetPushImmunity(tagCount > 0);
}

void ABaseCharacter::UpdateHitFlash()
{
	if (NextHitFlash.IsSet())
	{
		//do hit flash
		const BaseCharacterHitFlash& DoFlash = NextHitFlash.GetValue();
		UDungeonsEffectLibrary::HitFlashOnCharacter(this, DoFlash.color, DoFlash.duration, DoFlash.startFraction);
		NextHitFlash.Reset();
	}
}

void ABaseCharacter::RemoveActiveEffects() const {
	if (HasAuthority()) {
		FGameplayTagContainer tags(FGameplayTag::RequestGameplayTag(TEXT("PersistAfterDeath")));
		const auto query = FGameplayEffectQuery::MakeQuery_MatchNoOwningTags(tags);		
		AbilitySystem->RemoveActiveEffects(query);
	}
}

void ABaseCharacter::OnRep_AttackState() {
	OnAttackStateChanged.Broadcast(AttackState);
}

void ABaseCharacter::OnRep_Master() {

}

void ABaseCharacter::OnPushRecieverDeactivated()
{
	EnablePushVolumes(false);
}

bool ABaseCharacter::IsAlive() const {
	if (auto* health = GetHealthComponent())
		return health->IsAlive();
	else
		return false;
}

bool ABaseCharacter::IsNotAlive() const {
	if (auto* health = GetHealthComponent())
		return health->IsNotAlive();
	else
		return false;
}

void ABaseCharacter::OnRep_LookAtActor() {
	if (auto animInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance())) {
		if (AActor* actor = LookAtActor.Get()) {
			animInstance->SetLookAtActor(actor);
		}
		else {
			animInstance->ClearLookAtActor();
		}
	}	
}

bool ABaseCharacter::IsImmobile() const {
	static const auto immobileTag = FGameplayTag::RequestGameplayTag("StatusEffect.Immobile");
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(immobileTag);
}

bool ABaseCharacter::IsFrozenSolid() const {
	static const auto immobileTag = FGameplayTag::RequestGameplayTag("StatusEffect.Immobile.FrozenSolid");
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(immobileTag);
}

bool ABaseCharacter::IsMeleeDisabled() const {
	static const auto disabledTag = FGameplayTag::RequestGameplayTag("StatusEffect.Melee.Disabled");
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(disabledTag);
}

bool ABaseCharacter::AllowsNavigation(const AActor* actor) {
	if (const auto navModifier = actor->FindComponentByClass<UNavModifierComponent>()) {
		if (navModifier->AreaClass == UNavArea_Null::StaticClass()) {
			return false;
		}
	}
	return true;
}

void ABaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) {
	if(GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking || GetCharacterMovement()->MovementMode == EMovementMode::MOVE_NavWalking) {
		OnCharacterWalking.Broadcast();
	}
}

TOptional<FVector> ABaseCharacter::GetFallFromLocation() const {
	return FallFromLocation;
}

void ABaseCharacter::UpdatePushVolumeMovement(FVector pushForce){
	if (PushVolumeMovementComponent) {
		PushVolumeMovementComponent->Velocity = pushForce;
	}

	if (pushForce.IsZero()) {
		if (auto controller = GetController()) {
			if (auto movement = Cast<UCharacterMovementComponent>(GetMovementComponent())) {
				FVector vel = movement->Velocity;
				controller->SetControlRotation(vel.Rotation());
			}
		}
	}
}

void ABaseCharacter::SetPushImmunity(bool bEnable)
{
	GetCapsuleComponent()->SetCollisionResponseToChannel((ECollisionChannel)ECustomTraceChannels::PushObject, (bEnable) ? ECollisionResponse::ECR_Ignore: ECollisionResponse::ECR_Overlap);	
}

void ABaseCharacter::EnablePushVolumes(bool bEnable)
{
	if(bEnable)
		GetOrCreatePushVolumeReactiveComponent(); //make sure we have it

	if (PushVolumeReactiveComponent && PushVolumeMovementComponent)
	{
		const bool PushAttached = PushVolumeReactiveComponent->IsAttachedTo(GetCapsuleComponent());

		if (PushAttached != bEnable)
		{
			if (bEnable)
			{
				PushVolumeReactiveComponent->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			}
			else
			{
				PushVolumeReactiveComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
			}
		
			PushVolumeReactiveComponent->SetActive(bEnable);
			PushVolumeReactiveComponent->SetVolumeOverlapsEnabled(bEnable);
			PushVolumeMovementComponent->SetActive(bEnable);
			PushVolumeMovementComponent->SetComponentTickEnabled(bEnable);

			if(bEnable)
				PushVolumeReactiveComponent->UpdateOverlaps();

		}
	}
}

bool ABaseCharacter::IsUnderwater() const {
	if (GetAbilitySystemComponent()) {
		bool bIsUnderwater = GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Underwater")));
		bool bIsImmune = GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Immunity.Underwater")));
		return bIsUnderwater && !bIsImmune;
	}
	return false;
}

bool ABaseCharacter::HitFlashOnCharacter(FLinearColor color, float duration, float startFraction /*= 1.f*/)
{
	BaseCharacterHitFlash NewFlash = { color, duration, startFraction };
	NextHitFlash = NewFlash;
	return true;
}

void ABaseCharacter::SetBackPackComponentAttached(bool AttachVal)
{
	if (backpackComponent)
	{
		if (AttachVal)
		{
			backpackComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, backpackSocketName);
		}
		else if(backpackComponent->GetAttachParent())
		{
			backpackSocketName = backpackComponent->GetAttachSocketName();
			backpackComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		}

		backpackComponent->SetActive(AttachVal);		
	}
}

void ABaseCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();

	TArray<UCapsuleComponent*> targetCapsules;
	GetTargetableCapsules(targetCapsules);
	TOptional<float> scaledRadius;
	for (const auto &capsule : targetCapsules) {
		if (capsule) {
			if (!scaledRadius) {
				scaledRadius = capsule->GetScaledCapsuleRadius();
			}

			ensureMsgf(FMath::IsNearlyEqual(capsule->GetScaledCapsuleRadius(), scaledRadius.GetValue(), 1.f), TEXT("Expected all target capsules to have same radius."));
			TargetableCapsules.Add(capsule);
		}
	}
}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const {
	return AbilitySystem;
}
