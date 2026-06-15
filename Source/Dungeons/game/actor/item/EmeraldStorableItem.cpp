#include "Dungeons.h"
#include "EmeraldStorableItem.h"
#include "game/actor/ProjectileManager.h"
#include "game/component/WalkPickupComponent.h"

static int sMaxVisibleMeshes = 5;
FAutoConsoleVariableRef CVarMaxVisibleMeshes(
	TEXT("Dungeons.Emeralds.MaxInstancesPerActor"),
	sMaxVisibleMeshes,
	TEXT("Maximum number of Mesh instances shown for this actor (one per value amount)"),
	ECVF_Default);


AEmeraldStorableItem::AEmeraldStorableItem(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
:Super(ObjectInitializer)
{
	
	pMainMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMeshComponent"));
	pMainMeshes->SetMobility(EComponentMobility::Movable);
	pMainMeshes->SetupAttachment(RootComponent);	
	pMainMeshes->AddInstance(FTransform::Identity);
	pMainMeshes->bUseAttachParentBound = true;

	EmeraldBurst = CreateDefaultSubobject<UAudioComponent>(TEXT("EmeraldBurst"));
	EmeraldBurst->SetupAttachment(RootComponent);

	ParticleSystemShimmer = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemShimmer"));
	ParticleSystemShimmer->SetupAttachment(RootComponent);
	ParticleSystemShimmer->bUseAttachParentBound = true;

	WalkPickup = CreateDefaultSubobject<UWalkPickupComponent>(TEXT("WalkPickup"));
	WalkPickup->AllowPickupWhenDead = true;

	SubMeshOffsets.SetNumZeroed(sMaxVisibleMeshes);
	
	//Fill out random points to move submeshes to during spawn
	float fMaxDist = Sphere->GetUnscaledSphereRadius();
	for (int i = 1; i < sMaxVisibleMeshes; ++i)
	{
		FVector2D RandPoint = FMath::RandPointInCircle(fMaxDist);
		SubMeshOffsets[i].X = RandPoint.X;
		SubMeshOffsets[i].Y = RandPoint.Y;
	}


	SimpleMovementComponent->bSweepOnMove = false;
}


void AEmeraldStorableItem::BeginPlay()
{
	RotationTime = GetWorld()->GetTimeSeconds();
	Super::BeginPlay();
}

void AEmeraldStorableItem::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AEmeraldStorableItem::Tick(float DeltaSeconds)
{
	const float fNewRotTime = GetWorld()->GetTimeSeconds() - RotationTime;
	
	const int32 iMeshCount = pMainMeshes->GetInstanceCount();
	
	{
		const FQuat NewQuatRot = FQuat(FVector::UpVector, FMath::Sin(RotationSpeed * fNewRotTime) * PendelSpan * .5f);
		const FTransform NewTransform = { NewQuatRot, FVector::ZeroVector, FVector::OneVector };
		pMainMeshes->UpdateInstanceTransform(0, NewTransform);
	}

	for (int i = 1; i < iMeshCount; ++i)
	{
		FTransform CurrentTransform;
		
		if (pMainMeshes->GetInstanceTransform(i, CurrentTransform))
		{
			//interpolate to new
			const FVector Offset = SubMeshOffsets[i];
			const FVector CurrLoc = FMath::Lerp(CurrentTransform.GetLocation(), Offset, 0.1f);
			const FQuat NewQuatRot = FQuat(FVector::UpVector, FMath::Sin(RotationSpeed * fNewRotTime + (Offset.X * Offset.Y)) * PendelSpan * .5f);

			const FTransform NewTransform = {NewQuatRot, CurrLoc, FVector::OneVector};			
			pMainMeshes->UpdateInstanceTransform(i, NewTransform);
		}
	}

	pMainMeshes->MarkRenderStateDirty();
	
	Super::Tick(DeltaSeconds);
}

void AEmeraldStorableItem::ReInitialiseItem()
{
	const int32 iNumMeshes = FMath::Min(sMaxVisibleMeshes,ItemData.OverrideStoreCount.Get(1));
	const int32 iMeshCount = pMainMeshes->GetInstanceCount();

	float fMaxDist = Sphere->GetUnscaledSphereRadius();

	if (iNumMeshes != iMeshCount)
	{
		pMainMeshes->ClearInstances();
		pMainMeshes->AddInstance(FTransform::Identity);

		for (int i = 1; i < iNumMeshes; ++i)
		{
			pMainMeshes->AddInstance({ FQuat::Identity, FVector(0.0f,0.0f,FMath::FRandRange(-fMaxDist, fMaxDist)), FVector::OneVector });
		}
	}
	else
	{
		pMainMeshes->UpdateInstanceTransform(0, FTransform::Identity);
		for (int i = 1; i < iMeshCount; ++i)
		{
			pMainMeshes->UpdateInstanceTransform(i, { FQuat::Identity, FVector(0.0f,0.0f,FMath::FRandRange(-fMaxDist, fMaxDist)), FVector::OneVector });
		}		
	}

	pMainMeshes->MarkRenderStateDirty();
	ParticleSystemShimmer->Deactivate();
	ParticleSystemShimmer->Activate(true);

	WalkPickup->ResetPickup();

	const FVector MyLocation = GetActorLocation();
	const auto world = GetWorld();
	AProjectileActorManager::SpawnHitParticleEffect(world, InitParticleSystemTemplate, MyLocation, FRotator::ZeroRotator);
	
	EmeraldBurst->Play(0.0f);

	Super::ReInitialiseItem();
}

void AEmeraldStorableItem::OnPlayerInVisibleRangeChange(bool isVisible)
{
	if (isVisible)
	{
		ParticleSystemShimmer->Activate(false);
	}
	else
	{
		//reset and deactivate
		ParticleSystemShimmer->Activate(true);
		ParticleSystemShimmer->Deactivate();
	}

	Sphere->SetComponentTickEnabled(isVisible);
	pMainMeshes->SetComponentTickEnabled(isVisible);
	ParticleSystemShimmer->SetComponentTickEnabled(isVisible);
	EmeraldBurst->SetComponentTickEnabled(isVisible);
	WalkPickup->SetComponentTickEnabled(isVisible);

	Sphere->SetVisibility(isVisible,true);

	Super::OnPlayerInVisibleRangeChange(isVisible);
}

void AEmeraldStorableItem::OnLandedInternal()
{
	if (IsInUnsafeLocation()) {
		DestroyOrReturnToPool();
	}
	else {
		DisablePhysics();
		OnLanded();
	}

	//If we arent in range of a player, disable the simple movement component
	if (SimpleMovementComponent && !mPlayerVisible)
	{
		SimpleMovementComponent->SetComponentTickEnabled(false);
	}
}

