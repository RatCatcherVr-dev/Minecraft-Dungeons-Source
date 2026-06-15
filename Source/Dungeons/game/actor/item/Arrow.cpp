#include "Dungeons.h"
#include <Sound/SoundCue.h>
#include "game/component/ArrowVobbleComponent.h"

#include "Arrow.h"
#include <UnrealNetwork.h>
#include "game/mission/MissionDefs.h"
#include "game/util/ActorQuery.h"
#include "game/Game.h"
#include "UObjectBase.h"
#include "../character/player/PlayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "game/component/OxygenComponent.h"

AArrow::AArrow(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
, ConsumesStorableItemId(game::item::type::Arrow.getId()) {

}

void AArrow::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArrow, bShouldSpawnArrowProp)
}

void AArrow::BeginPlay()
{
	Super::BeginPlay();
	bShouldSpawnArrowPropDefault = bShouldSpawnArrowProp;
	SphereComponent = FindComponentByClass<USphereComponent>();
}

void AArrow::ToggleMagical_Implementation(bool magical) {
	bShouldSpawnArrowProp = bShouldSpawnArrowProp && !magical;
}

void AArrow::UpdateProjectileComponent(AActor* pOwner)
{
	if (ABaseCharacter* baseCharacter = Cast<ABaseCharacter>(pOwner))
	{
		ProjectileMovementComponent->SetUseUnderWaterPhysics(bAffectedByCustomPhysics && baseCharacter->IsUnderwater());
	}
}

void AArrow::LaunchProjectile(AActor* pOwner)
{
	if (SphereComponent)
	{
		SphereComponent->IgnoreActorWhenMoving(pOwner, true);
	}

	UpdateProjectileComponent(pOwner);
	Super::LaunchProjectile(pOwner);
	ProjectileMovementComponent->ResetComponent();
}

void AArrow::ResetProjectile()
{
	Super::ResetProjectile();
	bShouldSpawnArrowProp = bShouldSpawnArrowPropDefault;
	ProjectileMovementComponent->ResetComponent();
	if (SphereComponent)
	{
		SphereComponent->ClearMoveIgnoreActors();
	}
}

AArrowProp::AArrowProp(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));

	Sphere->SetIsReplicated(false);
	Sphere->SetMobility(EComponentMobility::Movable);
	Sphere->SetSphereRadius(10.0f);

	RootComponent = Sphere;
	Sphere->bEditableWhenInherited = true;

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArrowMesh"));
	ArrowMesh->AttachTo(Sphere);

	PrimaryActorTick.bCanEverTick = true;

	ArrowVobbleComponent = CreateDefaultSubobject<UArrowVobbleComponent>(TEXT("ArrowVobbleComponent"));
}

void AArrowProp::Initialise(bool bCharged, float fPropLifetime)
{
	InitialiseProp(bCharged, fPropLifetime);
}

void AArrowProp::InitialiseProp(bool bCharged, float fPropLifetime)
{
	Charged = bCharged;
	LifeTime = fPropLifetime;

	UAudioComponent* pHitAudio = CreateHitAudioComponent();

	if (pHitAudio)
	{
		pHitAudio->SetSound((bCharged) ? ChargedSound : UnChargedSound);
		pHitAudio->Play();
	}

	if (EnableVobble && ArrowVobbleComponent)
	{
		ArrowVobbleComponent->StartVobble();
	}

	OnInitializedProp(bCharged, fPropLifetime);

}

void AArrowProp::BeginPlay()
{
	Super::BeginPlay();
	CreateHitAudioComponent();
}

class UAudioComponent* AArrowProp::CreateHitAudioComponent()
{
	if (!HitSoundComponent && (UnChargedSound || ChargedSound))
	{
		HitSoundComponent = UGameplayStatics::SpawnSoundAttached(Charged ? ChargedSound : UnChargedSound, GetRootComponent());
		if (HitSoundComponent)
		{
			HitSoundComponent->bAutoDestroy = false;
			HitSoundComponent->Stop();
			HitSoundComponent->bAutoManageAttachment = true;
			HitSoundComponent->AutoAttachParent = GetRootComponent();
		}
	}

	return HitSoundComponent;
}