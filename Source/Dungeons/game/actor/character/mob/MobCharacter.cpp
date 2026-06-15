#include "Dungeons.h"
#include "MobCharacter.h"
#include "MobBtController.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/mob/MobTypeDefs.h"
#include "game/component/BaseParticleAssetsComponent.h"
#include "game/component/BehaviorComponent.h"
#include "game/component/GlobalHealthBarComponent.h"
#include "game/component/MobEnchantmentComponent.h"
#include "game/component/MobCharacterMovementComponent.h"
#include "game/component/MusicOverrideComponent.h"
#include "game/team/TeamQuery.h"
#include "util/StringUtil.h"
#include "world/entity/MobTags.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include <NavigationSystem.h>
#include "game/mobspawn/alpha/AlphaSpawner.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "online/sessions/OnlineUtil.h"
#include "Components/WidgetComponent.h"
#include "Components/DecalComponent.h"
#include "DungeonsGameState.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "Assets/DungeonsAssetManager.h"
#include "game/component/PlayerExperienceComponent.h"
#include "game/GameBP.h"
#include "game/actor/character/DungeonsAbilitySystemComponent.h"
#include "DungeonsGameInstance.h"
#include "game/util/ComponentUtils.h"
#include "game/GamplayEffects/WaterBreathing.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "util/ProjectileFunctionLibrary.h"
#include "game/actor/character/mob/AncientMobAudioComponent.h"
#include "game/Enchantments/VoidTouched.h"

DECLARE_STATS_GROUP(TEXT("HealthComponent"), STATGROUP_MobCharacter, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("OnMobDeath"), STAT_OnMobCharacterDeath, STATGROUP_MobCharacter);

#define USE_MOB_NEARMISS_COMPONENT PLATFORM_WINDOWS

namespace DungeonsQA {
	extern TAutoConsoleVariable<int32> CVShowMobAudioTriggers;
};

namespace mobcharacter {
	static TSet<FColor> dbg_colors = { FColor::Green, FColor::Turquoise };
	static int n = 0;
	static USphereComponent* setupDebugSphere(AMobCharacter* character, const TCHAR* name) {
#if !UE_BUILD_SHIPPING
		n += 1;
		USphereComponent* dbgSphere = NewObject<USphereComponent>(character, name);
		dbgSphere->SetVisibility(false);
		dbgSphere->SetHiddenInGame(false);
		dbgSphere->SetCollisionProfileName(TEXT("NoCollision"));
		dbgSphere->ShapeColor = n < 1 ? FColor::Green : FColor::Turquoise;

		return dbgSphere;
#endif
		return nullptr;
	}

	bool HasProperty(const uint8& flags, EMobSpawnProperties test) {
		return (flags >> (uint8)test & 1) != 0;
	}

	void SetProperty(uint8& flags, EMobSpawnProperties flag, bool value) {
		flags = (flags & ~(1 << (uint8)flag)) | ((uint8)value << (uint8)flag);
	}

}

AMobCharacter::AMobCharacter(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer
	.SetDefaultSubobjectClass<UMobCharacterMovementComponent>(CharacterMovementComponentName)
	.SetDefaultSubobjectClass<UMobEnchantmentComponent>(FName("EnchantmentComponent")))
{
	EntityType = EntityType::Undefined;
	Behavior = CreateDefaultSubobject<UBehaviorComponent>(TEXT("BehaviorComponent"));
	
	BlobShadow = CreateDefaultSubobject<UDecalComponent>(TEXT("blob_shadow"));
	BlobShadow->SetupAttachment(RootComponent);
	BlobShadow->bUseAttachParentBound = 1;

#if USE_MOB_NEARMISS_COMPONENT
	NearMissTargetSelectCollider = CreateDefaultSubobject<USphereComponent>(TEXT("NearMissTargetSelectCollider"));
	NearMissTargetSelectCollider->SetupAttachment(RootComponent);
	NearMissTargetSelectCollider->SetMobility(EComponentMobility::Movable);
	NearMissTargetSelectCollider->SetSphereRadius(215.0f);
	NearMissTargetSelectCollider->SetHiddenInGame(true);
	NearMissTargetSelectCollider->SetReceivesDecals(false);
	NearMissTargetSelectCollider->SetGenerateOverlapEvents(false);
	NearMissTargetSelectCollider->SetCanEverAffectNavigation(false);
	NearMissTargetSelectCollider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	NearMissTargetSelectCollider->SetCollisionProfileName("MobNearMiss");
	NearMissTargetSelectCollider->ComponentTags.Add("NearMissTargetSelect");
#endif

	HurtAudioComponent = nullptr;
	
	// replication config
	bOnlyRelevantToOwner = false;
	bAlwaysRelevant = false;
	bReplicateMovement = true;
	bNetLoadOnClient = true;
	bNetUseOwnerRelevancy = false;
	bReplicates = true;

	NetCullDistanceSquared = 51200000.0f;
	NetUpdateFrequency = 10.0f;
	NetPriority = 5.0f;

	NetDormancy = DORM_Awake;
	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

void AMobCharacter::PreRegisterAllComponents()
{
	if (AllMobMeshesUseAttachParentBounds) //Make sure all descendants of the main mesh are using the parent bounds to reduce movement calculation overhead
	{
		if (USkeletalMeshComponent* pMesh = GetMesh())
		{
			pMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			pMesh->bUseAttachParentBound = true;
			TArray<USceneComponent*> ChildrenList;
			pMesh->GetChildrenComponents(true, ChildrenList);

			for (auto* pChild : ChildrenList)
			{
				if (pChild)
					pChild->bUseAttachParentBound = true;
			}
		}
	}

	Super::PreRegisterAllComponents();
}

void AMobCharacter::PreInitializeComponents() {
	ABaseCharacter::PreInitializeComponents();

	GetAbilitySystemComponent()->AddSet<UHealthAttributeSet>();
}

void AMobCharacter::Falling() {
	Super::Falling();
	if (NoClip) {
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	}

	if (const auto aiController = AiController()) {
		aiController->GetPathFollowingComponent()->Deactivate();
	}
}

void AMobCharacter::SetSharedCooldown(const float activeCooldown, const float passiveCooldown){
	Super::SetSharedCooldown(activeCooldown, passiveCooldown);
	RefreshMovementCooldownState();	
	GetWorld()->GetTimerManager().SetTimer(CooldownStateRefreshHandle, FTimerDelegate::CreateUObject(this, &AMobCharacter::ResetSharedCooldown), passiveCooldown, false);
}

void AMobCharacter::ResetSharedCooldown() {
	Super::ResetSharedCooldown();
	RefreshMovementCooldownState();	
}

bool AMobCharacter::IsAnyCooldownActive() const {
	return GetWorld()->GetTimeSeconds() < GetCanActFromSecondPassive();
}

void AMobCharacter::RefreshMovementCooldownState() const {
	auto movement = FindComponentByClass<UMobCharacterMovementComponent>();
	if(movement)
		movement->ToggleIsInCooldown(IsAnyCooldownActive());
}

void AMobCharacter::Landed(const FHitResult& hit) {
	Super::Landed(hit);
	if (NoClip) {
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	}

	if (const auto aiController = AiController()) {
		aiController->GetPathFollowingComponent()->Activate();
	}
}

class AAIController* AMobCharacter::AiController() const {
	return dynamic_cast<AAIController*>(GetController());
}

const FText& AMobCharacter::GetDisplayName() const {
	return mob::type::getMobDisplayName(EntityType);
}

bool AMobCharacter::HasTag(const FString& tag) const {
	return hasMobTag(EntityType, stringutil::toStdString(tag));
}

bool AMobCharacter::HasTag(size_t tag_hash) const {
	return hasMobTag(EntityType, tag_hash);
}

void AMobCharacter::TrySpawnXPOrbs(AActor* OrbTarget, float Damage)
{
	if(auto* HealthComp = GetHealthComponent())
	{
		if (auto* pPlayerChar = Cast<APlayerCharacter>(OrbTarget))
		{
			const float fEndHealth = HealthComp->GetCurrentHealth() - Damage;
			if (fEndHealth <= 0.0f)
			{
				const int32 NumOrbs = FMath::RandRange( 2, FMath::Clamp((int32)(HealthComp->MaxHealth / 30.0f), 4, 8) ); //magic numbers from BP nodes
				UProjectileFunctionLibrary::SpawnXPOrbs(GetWorld(),this, pPlayerChar,NumOrbs);
			}
		}
	}
}

float AMobCharacter::CalculateAbsoluteMoveSpeed(float absoluteSpeed) const {
	return absoluteSpeed * AbilitySystem->GetNumericAttribute(UMovementAttributeSet::SpeedMultiplierAttribute());
}

float AMobCharacter::CalculateRelativeMoveSpeed(float relativeFraction) const {
	return CalculateAbsoluteMoveSpeed(relativeFraction * GetMaxSpeed());
}

void AMobCharacter::RespawnAt(const FVector& respawnLocation, bool vfx) const {
	if (AGameBP* game = actorquery::getFirstActor<AGameBP>(GetWorld())) {
		auto enchantmentComp = GetEnchantmentComponent();
		game->RequestMobSpawn(EntityType, game::mobspawn::providers::Location(respawnLocation), enchantmentComp ? enchantmentComp->GetEnchantments() : TArray<FEnchantmentData>(), game::mobspawn::configs::DefaultNoVariants(true),
			[this, vfx](AMobCharacter* mob){
				if( mob && vfx ) {
					if( auto abilitySystem = mob->GetAbilitySystemComponent() ) {
						abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.MobRevive"), FGameplayCueParameters());
					}
				}
			}
		);
	}
}


void AMobCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AMobCharacter, EntityType, COND_InitialOnly);
	DOREPLIFETIME(AMobCharacter, mMobReplicatedFlags);
}

bool AMobCharacter::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	const auto gameState = Cast<ADungeonsGameState>(GetWorld()->GetGameState());

	//Use the default replication relevancy cals for cutscenes.
	if (gameState && gameState->IsCinematicPlaying())
	{
		return AActor::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
	}
	else
	{
		return mPlayersVisible.Contains(ViewTarget) && AActor::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
	}
}

void AMobCharacter::Tick(float DeltaSecs) {
	Super::Tick(DeltaSecs);
	UpdateMobHurt();
	PlayIdleSound(DeltaSecs);

#if !UE_BUILD_SHIPPING
	if (DebugSphereInner) {
		DebugSphereInner->SetVisibility(DungeonsQA::CVShowMobAudioTriggers.GetValueOnGameThread() != 0);

	}
	if(DebugSphereOuter) {
		DebugSphereOuter->SetVisibility(DungeonsQA::CVShowMobAudioTriggers.GetValueOnGameThread() != 0);
	}
#endif
}

void AMobCharacter::UpdateMobHurt()
{
	if (bMobHurt)
	{
		if (hurtSound && IsAlive())
		{
			if (HurtAudioComponent)
			{
				HurtAudioComponent->Play();
			}
			else
			{
				HurtAudioComponent = UGameplayStatics::SpawnSoundAttached(hurtSound, GetRootComponent(), NAME_None, FVector(ForceInit), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, 1.f, 1.f, 0.f, nullptr, nullptr, false);
			}
		}
		bMobHurt = false;
	}
}

void AMobCharacter::PlayIdleSound(float DeltaSecs) {
	if (idleSound)
	{
		mIdleAudioCountDown -= DeltaSecs;
		if (mIdleAudioCountDown <= 0.0f)
		{
			mIdleAudioCountDown = FMath::FRandRange(idleSoundDelay, idleSoundDelay*2.0f);

			const auto healthComponent = FindComponentByClass<UHealthComponent>();
			if (healthComponent == nullptr || healthComponent->IsAlive()) {
				if (IdleAudioComponent)
				{
					IdleAudioComponent->Play();
				}
				else if (GetMesh())
				{
					IdleAudioComponent = UGameplayStatics::SpawnSoundAttached(idleSound, GetRootComponent());
					if (IdleAudioComponent)
					{
						IdleAudioComponent->bAutoDestroy = false;
					}
				}
			}
		}
	}
}

void AMobCharacter::BeginPlay() {
	Super::BeginPlay();

	SetEventMobRevealed(true);
	InstanceTracker< AMobCharacter >::AddInstance(this->GetWorld(), this);

	MobParams.startPos = GetActorLocation();
	MobParams.anchor = GetActorLocation();

	if (auto* MobHealthComponent = GetHealthComponent()) {
		MobHealthComponent->OnShieldChange.AddUObject(this, &AMobCharacter::HandleShieldChange);
		MobHealthComponent->OnDeath.AddUObject(this, &AMobCharacter::HandleDeath);
		MobHealthComponent->OnRevive.AddUObject(this, &AMobCharacter::HandleRevive);
	}

	// Mobs with a global health bar should always be relevant for replication.
	GlobalHealthBarComponent = FindComponentByClass<UGlobalHealthBarComponent>();
	if (GlobalHealthBarComponent) {
		bAlwaysRelevant = true;
		UE_LOG(LogDungeons, Warning, TEXT("Hello! I have global health. Sincerely %s."), *GetNameSafe(this));
	}

	

	// debug sphere render for music override triggers
	if (auto musicOverride = FindComponentByClass<UMusicOverrideComponent>())
	{
		MusicOverridePtr = musicOverride;
		if (auto gi = Cast<UDungeonsGameInstance>(GetGameInstance())) {
			DebugSphereInner = mobcharacter::setupDebugSphere(this, TEXT("Music Trigger"));

			if (DebugSphereInner != nullptr) {
				DebugSphereInner->RegisterComponent();
				DebugSphereInner->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
				DebugSphereInner->SetSphereRadius(gi->GetAudioMusicManager()->MusicOverrideTriggerRadius);
				DebugSphereInner->SetVisibility(DungeonsQA::CVShowMobAudioTriggers.GetValueOnGameThread() != 0);
			}

			DebugSphereOuter = mobcharacter::setupDebugSphere(this, TEXT("Music Shut-off"));
			if (DebugSphereOuter != nullptr) {
				DebugSphereOuter->RegisterComponent();
				DebugSphereOuter->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
				DebugSphereOuter->SetSphereRadius(gi->GetAudioMusicManager()->MusicOverrideResetRadius);
				DebugSphereOuter->SetVisibility(DungeonsQA::CVShowMobAudioTriggers.GetValueOnGameThread() != 0);

			}
		}
	}

	const auto gameState = Cast<ADungeonsGameState>(GetWorld()->GetGameState());

	//Dont bother death checking in cutscenes
	if (gameState && !gameState->IsCinematicPlaying())
	{
		if (!HasAuthority())
		{
			if (!IsAlive())
			{
				//This is a client instanced mob thats already dead, lets just hide it until the server clears it up
				SetActorHiddenInGame(true);
			}
		}
	}


	if (auto* pMesh = GetMesh())
	{
		if (deathSound)
		{
			DeathAudioComponent = UGameplayStatics::SpawnSoundAttached(deathSound, pMesh);
			if (DeathAudioComponent)
			{
				DeathAudioComponent->bAutoDestroy = false;
				DeathAudioComponent->bStopWhenOwnerDestroyed = false;
				DeathAudioComponent->Stop();
				DeathAudioComponent->SetComponentTickEnabled(false);
			}
		}

		if (idleSound)
		{
			IdleAudioComponent = UGameplayStatics::SpawnSoundAttached(idleSound, GetRootComponent());
			if (IdleAudioComponent)
			{
				IdleAudioComponent->bAutoDestroy = false;
				IdleAudioComponent->Stop();
			}

			mIdleAudioCountDown = FMath::FRandRange(idleSoundDelay, idleSoundDelay*2.0f);
		}


#if WITH_EDITOR

		//Check for naughty performance vampiric mesh elements

		{
			TArray<USceneComponent*> ChildComponents;
			pMesh->GetChildrenComponents(true, ChildComponents);

			for (USceneComponent* childComponent : ChildComponents)
			{
				if (childComponent->CanEverAffectNavigation())
				{
					FPlatformMisc::LowLevelOutputDebugStringf(TEXT("#Potential Performance issue: %s CanEverAffectNavigation set\n"), *childComponent->GetFullName());
					GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, FString::Printf(TEXT("Potential Performance issue: %s CanEverAffectNavigation set"), *childComponent->GetFullName()));
				}


				UPrimitiveComponent* pPrimitiveComp = Cast<UPrimitiveComponent>(childComponent);

				//Disable Unnecessary overheads
				if (pPrimitiveComp)
				{
					if (pPrimitiveComp->GetCollisionEnabled() != ECollisionEnabled::NoCollision)
					{
						FPlatformMisc::LowLevelOutputDebugStringf(TEXT("#Potential Performance issue: %s CollisionEnabled set\n"), *childComponent->GetFullName());
						GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, FString::Printf(TEXT("Potential Performance issue: %s CollisionEnabled set"), *childComponent->GetFullName()));
					}

					if (pPrimitiveComp->GetGenerateOverlapEvents())
					{
						FPlatformMisc::LowLevelOutputDebugStringf(TEXT("#Potential Performance issue: %s GenerateOverlapEvents set\n"), *childComponent->GetFullName());
						GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, FString::Printf(TEXT("Potential Performance issue: %s GenerateOverlapEvents set"), *childComponent->GetFullName()));
					}
				}

			}
		}

#endif

	}

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* abilitySystem = GetAbilitySystemComponent())
		{
			if (!HasTag(MobTags::HashTag_Underwater) && !HasTag(MobTags::HashTag_Unbubbled))
			{
				auto underwaterTagDelegate = FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &AMobCharacter::OnUnderwaterTagChange);
				abilitySystem->RegisterAndCallGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Underwater")), underwaterTagDelegate, EGameplayTagEventType::NewOrRemoved);
			}

			if(HasTag(MobTags::HashTag_Ender))
			{
				auto spec = effects::CreateGameplayEffectSpec<UVoidStrikeImmunityGameplayEffect>(abilitySystem, 1.f);
				abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
			}
		}
	}

	//We might have died before Begun play has been called, in which case we need to clean up.
	if(!GetHealthComponent()->IsAlive()) {
		HandleDeath();
	}

#if !USE_MOB_NEARMISS_COMPONENT
	//remove nearmiss on consoles
	if (NearMissTargetSelectCollider)
	{
		NearMissTargetSelectCollider->DestroyComponent();
		NearMissTargetSelectCollider = nullptr;
	}
#endif

}

void AMobCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	InstanceTracker< AMobCharacter >::RemoveInstance(this->GetWorld(), this);

}

void AMobCharacter::IssueWarning(AActor* actor) {
	if (Behavior) {
		Behavior->SetLastWarning(actor);
	}
}

void AMobCharacter::HandleShieldChange(const bool hasShield) {
	if (hasShield)
		return;

	const FName shieldTag { "Shield" };
	const auto candidateComponents = GetComponentsByTag(UChildActorComponent::StaticClass(), shieldTag);
	for (auto candidate : candidateComponents) {
		auto* child = Cast<UChildActorComponent>(candidate);
		if (child->GetChildActor() == nullptr)
			continue;

		UBaseParticleAssetsComponent* ParticleAssetsComponent = FindComponentByClass<UBaseParticleAssetsComponent>();
		SpawnEffectsAtLocation(ParticleAssetsComponent->ShieldBreak, shieldBreakSound, false);
		child->SetVisibility(false, true);
	}
}

bool AMobCharacter::IsEnchanted() const{
	if (auto mobenchantmentcomp = FindComponentByClass<UMobEnchantmentComponent>()) {
		return mobenchantmentcomp->IsEnchanted();
	}
	return false;
}

bool AMobCharacter::IsRaidCaptain() const
{
	return HasTag(MobTags::HashTag_RaidCaptain);
}

void AMobCharacter::HandleDeath() {

	SCOPE_CYCLE_COUNTER(STAT_OnMobCharacterDeath)

	const auto capsuleComponent = GetCapsuleComponent();
	CachedCapsuleCollisionType = capsuleComponent->GetCollisionEnabled();
	capsuleComponent->SetEnableGravity(false);
	capsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DisableNearMissSelection();

	if(HideOnDeath){
		SetWorldState(ECharacterWorldState::Disappeared);		
	}

	if (HasAuthority()) {
		SetLifeSpan(5.f);
		if (auto* meshComp = GetStaticMeshComponent()) {
			UNavigationSystemV1::OnComponentUnregistered(meshComp);
		}
	}	

	if (DeathAudioComponent)
	{
		DeathAudioComponent->SetComponentTickEnabled(true);
		DeathAudioComponent->Play();
	}
	else if(deathSound)
	{
		if (auto* pMesh = GetMesh())
		{
			DeathAudioComponent = UGameplayStatics::SpawnSoundAttached(deathSound, pMesh); 
			if (DeathAudioComponent)
			{
				DeathAudioComponent->bAutoDestroy = false;
				DeathAudioComponent->bStopWhenOwnerDestroyed = false;
			}
		}
	}
	else
	{
		UpdateMobHurt();
	}
	
	bMobHurt = false;

	if(BlobShadow)
		BlobShadow->SetVisibility(false);
	
	if( HasAuthority() ) {
		auto stats = Game->GetGame()->settings().difficultyStats;
		bool resurrect = bForceResurrection || (FMath::RandRange(0.0f, 1.0f) < stats.GetMobResurrectionChance());
		if( CanResurrect() && resurrect ) {
			GetWorld()->GetTimerManager().SetTimer(ResurrectionHandle, FTimerDelegate::CreateUObject(this, &AMobCharacter::OnResurrection), ResurrectTime, false);
			SetLifeSpan(FMath::Max(5.f, ResurrectTime));
		}
	}

	if (AncientMobAudioComponent)
	{
		AncientMobAudioComponent->OnMobDeath();
	}

	Super::HandleDeath();
}

void AMobCharacter::HandleRevive() {
	const auto capsuleComponent = GetCapsuleComponent();
	capsuleComponent->SetEnableGravity(true);
	capsuleComponent->SetCollisionEnabled(CachedCapsuleCollisionType);

	ResetNearMissSelection();

	if (HideOnDeath) {
		SetWorldState(ECharacterWorldState::InWorld);
	}

	if (HasAuthority()) {
		SetLifeSpan(0.f);
		if (auto* meshComp = GetStaticMeshComponent()) {
			UNavigationSystemV1::OnComponentRegistered(meshComp);
		}
	}

	if(BlobShadow)
		BlobShadow->SetVisibility(true);

	Super::HandleRevive();
}

UStaticMeshComponent* AMobCharacter::GetStaticMeshComponent() const {
	return Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
}

bool AMobCharacter::CanResurrect() {
	return !HasTag(MobTags::HashTag_EventMob) 
		&& !HasTag(MobTags::HashTag_Miniboss)
		&& !HasTag(MobTags::HashTag_NoRes)
		&& !HasTag(MobTags::HashTag_Passive)
		&& !HasTag(MobTags::HashTag_Friendly)
		&& !HasTag(MobTags::HashTag_Ancient);
}

void AMobCharacter::OnResurrection() {
	FVector location;
	if (RespawnAtStartLocation)
	{
		location = MobParams.startPos;
	}
	else
	{
		location = GetActorLocation();
		if (auto mesh = GetMesh()) {
			if (auto bodyInstance = mesh->GetBodyInstance()) {
				location = bodyInstance->GetCOMPosition();
			}
			mesh->SetVisibility(false);
		}
	}

	if (RespawnUnenchanted)
	{
		EnchantmentComponent->RemoveEnchantments();
	}

	RespawnAt( location, true );
}

FVector AMobCharacter::GetMoveGoalOffset(const AActor* MovingActor) const {
	FVector GoalOffset = Super::GetMoveGoalOffset(MovingActor);

	if (!HasCustomTargetableCapsules())
		return GoalOffset;

	const AController *controller = Cast<AController>(MovingActor);
	if (const APawn* pawn = controller ? controller->GetPawn() : nullptr) {
		if (const auto capsule = GetClosestTargetableCapsule(pawn->GetActorLocation())) {

			// D11.BC goal offset needs to be relative to nav agent transform
			// this may be inaccurate in case capsules move relative to nav agent transform
			const auto invMat = FQuatRotationTranslationMatrix(GetActorQuat(), GetNavAgentLocation()).Inverse();
			return invMat.TransformPosition(capsule->GetComponentLocation() - FVector{ 0.f, 0.f, capsule->GetScaledCapsuleHalfHeight() });
		}
	}

	return GoalOffset;
}

void AMobCharacter::GetMoveGoalReachTest(const AActor* MovingActor, const FVector& MoveOffset, FVector& GoalOffset, float& GoalRadius, float& GoalHalfHeight) const {

	Super::GetMoveGoalReachTest(MovingActor, MoveOffset, GoalOffset, GoalRadius, GoalHalfHeight);
	GoalOffset = GetMoveGoalOffset(MovingActor);
}

bool AMobCharacter::IsMoving() const {
	if (const auto mobController = Cast<AMobBtController>(GetController())) {
		const auto pathFollowing = mobController->GetPathFollowingComponent();
		if (pathFollowing && IsLocallyControlled()) {
			return (pathFollowing->GetStatus() == EPathFollowingStatus::Moving);
		}
	}
	return Super::IsMoving();
}

void AMobCharacter::RemoveInstanceTracking()
{
	Super::RemoveInstanceTracking();
	InstanceTracker< AMobCharacter >::RemoveInstance(this->GetWorld(), this);
}

void AMobCharacter::SetIsUnderlingMob(bool isUnderling) {
	mobcharacter::SetProperty(mMobReplicatedFlags, EMobSpawnProperties::EM_Underling, isUnderling);
}

void AMobCharacter::SetBlockMusicOverride(bool blockMusicOverride) {
	mobcharacter::SetProperty(mMobReplicatedFlags, EMobSpawnProperties::EM_BlockMusicOverride, blockMusicOverride);
}

void AMobCharacter::SetResurrectTime(float time) {
	ResurrectTime = time;
}

void AMobCharacter::SetForceResurrection(bool res) {
	bForceResurrection = res;
}

void AMobCharacter::CancelResurrection() {
	GetWorld()->GetTimerManager().ClearTimer(ResurrectionHandle);
}

void AMobCharacter::SetIsRequiredForObjective(bool required) {
	bIsRequiredForObjective = required;
}

bool AMobCharacter::IsRequiredForObjective() const {
	return bIsRequiredForObjective;
}


bool AMobCharacter::IsEventMob() const {
	return HasTag(MobTags::HashTag_EventMob);
}

bool AMobCharacter::IsAncient() const {
	return HasTag(MobTags::HashTag_Ancient);
}

void AMobCharacter::SetEventMobRevealed(bool reveal) {
	mobcharacter::SetProperty(mMobReplicatedFlags, EMobSpawnProperties::EM_Revealed, reveal);
}


bool AMobCharacter::HasMusicOverride() const {
	// e.g. Event mobs spawed in arenabattles as bosses have musicoverride Blocked!
	return !mobcharacter::HasProperty(mMobReplicatedFlags, EMobSpawnProperties::EM_BlockMusicOverride)
		&& MusicOverridePtr.IsSet() 
		&& MusicOverridePtr.GetValue() != nullptr;
}

bool AMobCharacter::IsRevealed() const {
	return IsEventMob() ?
		mobcharacter::HasProperty(mMobReplicatedFlags, EMobSpawnProperties::EM_Revealed) :
		true;
}

bool AMobCharacter::IsUnderling() const {
	return mobcharacter::HasProperty(mMobReplicatedFlags, EMobSpawnProperties::EM_Underling);
}

void AMobCharacter::PlayerEnteredVisibleRange_Implementation()
{
	if (auto* pMesh = GetMesh())
	{
		pMesh->SetComponentTickEnabled(true);
	}

	if (HasAuthority() && !hasMobTag(EntityType, MobTags::HashTag_Miniboss) && !hasMobTag(EntityType, MobTags::HashTag_Pet)) 
	{
		if (auto movement = GetMovementComponent())
		{	
			movement->PrimaryComponentTick.TickInterval = 0.0f;
		}
	}
	
	PlayerInVisibleRangeChanged(true);
}

void AMobCharacter::PlayerLeftVisibleRange_Implementation()
{
	if (auto* pMesh = GetMesh())
	{
		pMesh->SetComponentTickEnabled(false);
	}

	if (HasAuthority() && !hasMobTag(EntityType, MobTags::HashTag_Miniboss) && !hasMobTag(EntityType, MobTags::HashTag_Pet))
	{
		if (auto movement = GetMovementComponent())
		{
			movement->PrimaryComponentTick.TickInterval = 0.5f;
		}
	}

	PlayerInVisibleRangeChanged(false);
}

void AMobCharacter::MobAtExtremePlayerRangeChanged_Implementation(bool IsAtExtremeRange)
{
	//dont do this for bosses or client net mobs
	if (!HasAuthority() ||
		hasMobTag(EntityType, MobTags::HashTag_Miniboss) ||
		hasMobTag(EntityType, MobTags::HashTag_Pet)
	) {
		return;
	}

	if (IsAtExtremeRange)
	{
		TArray<UActorComponent*> Components;
		GetComponents(Components, true);

		//FPlatformMisc::LowLevelOutputDebugStringf(TEXT("# MobAtExtremePlayerRangeChanged_Implementation %s\n"), *GetName());

		//Push any ticking components on to the tracking list and disable them
		for (auto* pComponent : Components)
		{
			//FPlatformMisc::LowLevelOutputDebugStringf(TEXT("### pComponent %s IsComponentTickEnabled : %d\n"), *pComponent->GetName(), (int)pComponent->IsComponentTickEnabled());
			if (pComponent->IsComponentTickEnabled())
			{
				mExtremeRangedTickingComponents.Push(pComponent);
				pComponent->SetComponentTickEnabled(false);
			}
		}

		//disable the mob Behavior
		if (Behavior)
		{
			Behavior->SetEnabled(false);
		}

		//disable mob tick
		mExtremeRangeWasActorTickEnabled = IsActorTickEnabled();
		SetActorTickEnabled(false);

		//disable mob controller
		if (AController* pController = GetController())
		{
			mExtremeRangeWasControllerActorTickEnabled = pController->IsActorTickEnabled();
			pController->SetActorTickEnabled(false);
			pController->StopMovement();
		}
		else
		{
			mExtremeRangeWasControllerActorTickEnabled = false;
		}
		
		//extreme range mobs no longer need updating to the clients, they are frozen
		SetNetDormancy(ENetDormancy::DORM_DormantAll);

	}
	else
	{
		//re-enable net client updates
		SetNetDormancy(ENetDormancy::DORM_Awake);

		//re-enable any components from the tick tracking list
		for (auto weakComponent : mExtremeRangedTickingComponents)
		{
			//FPlatformMisc::LowLevelOutputDebugStringf(TEXT("### pComponent %s IsComponentTickEnabled : %d\n"), *pComponent->GetName(), (int)pComponent->IsComponentTickEnabled());
			if (weakComponent.IsValid()) {
				weakComponent->SetComponentTickEnabled(true);
			}
		}
		mExtremeRangedTickingComponents.Reset();

		//enable the mob Behavior
		if (Behavior)
		{
			Behavior->SetEnabled(true);
		}

		//re-enable mob tick
		if (mExtremeRangeWasActorTickEnabled)
		{
			SetActorTickEnabled(true);
		}

		//re-enable mob controller
		if (mExtremeRangeWasControllerActorTickEnabled)
		{
			if (AController* pController = GetController())
			{
				pController->SetActorTickEnabled(true);
			}
		}

	}
}

bool AMobCharacter::CanDamageTarget(const ABaseCharacter* target) const
{
	if (!GetBackPackComponent() && target->GetCurrentTeam() == ETeamName::Keys)
	{
		return false;
	}
	return Super::CanDamageTarget(target);
}

bool AMobCharacter::IsHostileTowards(const ABaseCharacter* target) const
{
	if (!GetBackPackComponent() && target->GetCurrentTeam() == ETeamName::Keys)
	{
		return false;
	}
	return Super::IsHostileTowards(target);
}

EMusicSequenceState AMobCharacter::GetMusicSequenceState_Implementation() const {	
	return EMusicSequenceState::Main;
}

void AMobCharacter::UnPossessed() {
	Super::UnPossessed();
	OnUnpossessedByController.Broadcast();
}

bool AMobCharacter::IsFriendlyTowardsPlayers() const {
	return teamquery::is::friendly(GetCurrentTeam(), ETeamName::Heroes);
}

bool AMobCharacter::IsHostileTowardsPlayers() const {
	return teamquery::is::hostile(GetCurrentTeam(), ETeamName::Heroes);
}

class UGlobalHealthBarComponent* AMobCharacter::GetGlobalHealthBarComponent() const {
	return GlobalHealthBarComponent;
}

void AMobCharacter::ReactToHurt_Implementation(AActor* byWhom, float magnitude) {
	bMobHurt = true;
}

void AMobCharacter::OnUnderwaterTagChange(const FGameplayTag Tag, int32 Count)
{
	if (UAbilitySystemComponent* abilitySystem = GetAbilitySystemComponent())
	{
		if (Count > 0)
		{
			abilitySystem->ApplyGameplayEffectSpecToSelf(effects::CreateGameplayEffectSpec<UMobWaterBreathingGameplayEffect>(abilitySystem));
		}
		else
		{
			FGameplayTagContainer tags;
			tags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.MobWaterBreathing")));
			abilitySystem->RemoveActiveEffectsWithTags(tags);
		}
	}
}

#undef USE_MOB_NEARMISS_COMPONENT
