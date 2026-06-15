#include "Dungeons.h"
#include "TridentItem.h"
#include "AIController.h"
#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "UnrealNetwork.h"
#include "game/util/Pushback.h"
#include "game/component/HealthComponent.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "StatTracker.h"
#include "DungeonsGameInstance.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/ArrowItemSlot.h"
#include "Kismet/KismetMathLibrary.h"
#include "game/component/ArrowVobbleComponent.h"
#include "game/actor/ProjectileManager.h"
#include "game/actor/ImpactActionHandler.h"
#include "util/ProjectileFunctionLibrary.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

UTridentDamageGameplayEffect::UTridentDamageGameplayEffect() {
	InheritableOwnedTagsContainer.AddTag(damageTag::strongExplosion());
	InheritableOwnedTagsContainer.AddTag(damageTag::damageFriends());
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Explosion.Strong"), 0, 1);
}

ATridentItem::ATridentItem(const class FObjectInitializer& OI) : Super(OI) {
	PrimaryActorTick.bCanEverTick = true;
	EffectRadius = 750;
	EffectDamage = 200;
	EffectTime = 3.0f;	
	SetReplicates(true);
	EffectTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Trigger.Explosion.Trident"));
	SlotType = ESlotType::Trident;
	ThrowType = EThrowableType::Trident;

	USphereComponent* AttachPointComponent = CreateDefaultSubobject<USphereComponent>(USceneComponent::GetDefaultSceneRootVariableName());
	AttachPointComponent->bEditableWhenInherited = true;
	AttachPointComponent->SetIsReplicated(false);
	AttachPointComponent->SetMobility(EComponentMobility::Movable);
	AttachPointComponent->SetSphereRadius(1.0f);
	AttachPointComponent->SetSimulatePhysics(false);
	AttachPointComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachPointComponent->SetGenerateOverlapEvents(false);
	AttachPointComponent->SetHiddenInGame(true);
	AttachPointComponent->SetCanEverAffectNavigation(false);
	AttachPointComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	AttachPointComponent->SetEnableGravity(false);
	RootComponent = AttachPointComponent;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetIsReplicated(false);
	Sphere->SetMobility(EComponentMobility::Movable);
	Sphere->SetSphereRadius(18.0f);
	Sphere->SetSimulatePhysics(false);
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sphere->SetCollisionProfileName("Projectiles");
	Sphere->SetGenerateOverlapEvents(true);
	Sphere->SetHiddenInGame(true);
	Sphere->SetCanEverAffectNavigation(false);
	Sphere->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	Sphere->SetRelativeScale3D(FVector(1.5f));
	Sphere->bEditableWhenInherited = true;
	Sphere->SetEnableGravity(false);
	Sphere->SetupAttachment(RootComponent);

	TridentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TridentMesh"));
	TridentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TridentMesh->SetGenerateOverlapEvents(false);
	TridentMesh->SetCanEverAffectNavigation(false);
	TridentMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	TridentMesh->bEditableWhenInherited = true;
	TridentMesh->bUseAttachParentBound = true;
	TridentMesh->SetSimulatePhysics(false);
	TridentMesh->SetEnableGravity(false);
	TridentMesh->SetAbsolute(false, false, true);
	TridentMesh->SetupAttachment(Sphere);

	MainSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MainSoundComponent"));
	MainSoundComponent->bAutoManageAttachment = true;
	MainSoundComponent->bAutoDestroy = false;
	MainSoundComponent->SetupAttachment(RootComponent);

	AffectedByGravity = false;
}

void ATridentItem::BeginPlay() {
	Super::BeginPlay();
}

void ATridentItem::Tick(float DeltaSeconds) {
	if (PendingActivatedUpdate) {
		OnActivated();
		PendingActivatedUpdate = false;
	}
}

void ATridentItem::ThrowInMouseDir() {
	
	if (APlayerCharacter* owningPlayerCharacter = Cast<APlayerCharacter>(GetOwner()))
	{
		if (APlayerController* owningPlayerController = owningPlayerCharacter->GetPlayerController())
		{
			DisableInput(nullptr);

			const auto controller = Cast<ABasePlayerController>(owningPlayerController);
			const auto mouseWorldPos = GetMouseClickedWorldPosition(controller);
			auto dir = mouseWorldPos - GetActorLocation();
			dir.Z = 0;

			auto mob = controller->GetCurrentTarget();
			FRotator newArrowRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), mouseWorldPos);
			newArrowRotation.Pitch = 0.0f;
			SpawnProjectileServer(Cast<ABaseCharacter>(mob), newArrowRotation);
			Throw(FVector(1.f, 0.f, 0.f));
		}
	}
}

void ATridentItem::ThrowInPlayerForwardDir() {

	if (APlayerCharacter* owningPlayerCharacter = Cast<APlayerCharacter>(GetOwner()))
	{
		if (APlayerController* owningPlayerController = owningPlayerCharacter->GetPlayerController())
		{
			DisableInput(nullptr);
			auto autoTarget = Cast<ABasePlayerController>(owningPlayerController)->GetAutoTargetSystem();
			TWeakObjectPtr<AMobCharacter> mob;

			if (autoTarget->IsValidLowLevel())
			{
				mob = autoTarget->GetTarget();
			}

			FRotator newArrowRotation;

			if (mob.IsValid())
			{
				newArrowRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), mob->GetActorLocation());
				newArrowRotation.Pitch = 0.0f;
			}
			else
			{
				newArrowRotation = owningPlayerController->GetPawn()->GetActorRotation();
			}

			SpawnProjectileServer(Cast<ABaseCharacter>(mob), newArrowRotation);
			Throw(FVector(1.f, 0.f, 0.f));
		}
	}
}

void ATridentItem::Activate(const FVector& dir) 
{
	Super::Activate(dir);
}


FGameplayEffectSpec ATridentItem::GetGameplayEffectSpec(UAbilitySystemComponent* AbilitySystemComponent, AActor* owner, const float scaledDamage)
{
	return effects::CreateGameplayEffectSpec<UTridentDamageGameplayEffect>(AbilitySystemComponent, effects::HealthName, -scaledDamage, owner, this, owner->GetActorLocation(), 1.f);
}

void ATridentItem::ThrowServer_Implementation(const FVector& dir)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	const FVector direction = dir;
	Activate(direction);
	if (Slot)
	{
		Slot->Consume();
	}
}

void ATridentItem::SpawnProjectileServer_Implementation(ABaseCharacter* targetMob, FRotator newArrowRotation)
{
	if (HasAuthority())
	{
		SpawnProjectileMulticast(targetMob, newArrowRotation);
		SetLifeSpan(1.0f);
	}

	SetActorHiddenInGame(true);

}

bool ATridentItem::SpawnProjectileServer_Validate(ABaseCharacter* targetMob, FRotator newArrowRotation)
{
	return true;
}

void ATridentItem::SpawnProjectileMulticast_Implementation(ABaseCharacter* targetMob, FRotator newArrowRotation)
{

	if (TridentProjectile)
	{
		FTransform transform(GetOwner()->GetActorTransform());
		transform.SetRotation(newArrowRotation.Quaternion());

		if (auto projectile = AProjectileActorManager::Pop_Projectile(TridentProjectile, transform, Cast<APawn>(GetOwner()))) {
			projectile->bCanTriggerEnchantments = false;
			if (targetMob)
			{
				projectile->IsHoming = true;
				projectile->SetHomingTarget(targetMob);
			}
			else
			{
				projectile->IsHoming = false;
			}

			projectile->LaunchProjectile(GetOwner());

			auto arrow = Cast<AArrow>(projectile);
			arrow->bShouldSpawnArrowProp = false;
			
			if (UDelayedExplosionAction* action = NewObject<UTridentExplosionAction>()) {
				action->ExplosionCue = EffectTag;
				action->ExplosionDamage = EffectDamage;
				action->ExplosionRadius = EffectRadius;
				action->ExplosionDelaySeconds = EffectTime;
				arrow->ImpactActionHandler->AddRuntimeAction(action);
			}

			projectile->EnableProjectileCollisions();

			if (ThrownSound && MainSoundComponent)
			{
				MainSoundComponent->Stop();
				MainSoundComponent->SetSound(ThrownSound);
				MainSoundComponent->Play();
			}

		}
	}

}

void ATridentItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


ATridentItemArrowProp::ATridentItemArrowProp(const FObjectInitializer& ObjectInitializer)
:
	Super(ObjectInitializer)
{
}

void ATridentItemArrowProp::ExplodeNative(float radius, FGameplayEffectSpec spec, TWeakObjectPtr<UAbilitySystemComponent> instigatorAbilitySystem, const FPushback& pushback, FGameplayTag cue)
{
	int32 iKillCount=0;

	impactaction::ExplodeProjectile(instigatorAbilitySystem.Get(), spec, radius, cue, this, pushback, &iKillCount);
	
	if (iKillCount)
	{
		// D11.SSN
		if (auto* playerCharacter = Cast<APlayerCharacter>(GetInstigator())) {
			playerCharacter->TridentKillCount = iKillCount;
			playerCharacter->OnRep_TridentKillCount();
		}
	}
	
}

void ATridentItemArrowProp::InitialiseProp(bool bCharged, float fPropLifetime)
{
	Super::InitialiseProp(bCharged, fPropLifetime);
	mCurrentEmissiveValue = 0.0f;
	mEffectTime = fPropLifetime;
	EmissiveChange();
}

void ATridentItemArrowProp::EmissiveChange()
{
	mCurrentEmissiveValue += 2.0f;

	ArrowMesh->SetScalarParameterValueOnMaterials(EmissiveMaterialName, mCurrentEmissiveValue);

	if (mCurrentEmissiveValue < 8.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(EmissiveTimerHandle, this, &ATridentItemArrowProp::EmissiveChange, mEffectTime*0.2f);
	}
}

UTridentExplosionAction::UTridentExplosionAction()
{
	EffectToApply = UTridentDamageGameplayEffect::StaticClass();
}
