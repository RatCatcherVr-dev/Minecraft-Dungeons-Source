#include "Dungeons.h"
#include "InstancedMeshesActor.h"
#include <Components/InstancedStaticMeshComponent.h>
#include <Components/HierarchicalInstancedStaticMeshComponent.h>
#include <Engine/StaticMeshActor.h>

AInstancedMeshesActor::AInstancedMeshesActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void AInstancedMeshesActor::SetHierarchical(bool hierarchical) {
	check(mInstances.Num() == 0 && "You must only call SetHierarchical before any meshes are placed");
	mHierarchical = hierarchical;
}

void AInstancedMeshesActor::Place(UStaticMesh* mesh, const FTransform& transform, bool NavigationRelevant) {
	_GetOrCreateInstancedMeshFor(mesh, nullptr, NavigationRelevant).AddInstanceWorldSpace(transform);
}

void AInstancedMeshesActor::Place(UStaticMeshComponent* meshComp, const FTransform& transform, bool NavigationRelevant) {
	
	UInstancedStaticMeshComponent& InstComp = _GetOrCreateInstancedMeshFor(meshComp->GetStaticMesh(), meshComp, NavigationRelevant);
	InstComp.AddInstanceWorldSpace(transform);

#if WITH_EDITORONLY_DATA
	auto& NameListContainer = mInstancesNames.FindOrAdd(&InstComp);
	NameListContainer.NameList.Push(meshComp->GetOwner()->GetFName());
#endif

}

void AInstancedMeshesActor::SpawnStaticMeshesFromContainedInstances()
{
	
	auto* pCurrentLvl = this->GetLevel();
	auto* pWorld = this->GetWorld();

	FActorSpawnParameters Params;
	Params.OverrideLevel = pCurrentLvl;
	Params.bDeferConstruction = true;

	for (UInstancedStaticMeshComponent* IMComp : mInstances)
	{
		const int iNumInstances = IMComp->GetInstanceCount();

#if WITH_EDITORONLY_DATA
		auto NameListContainer = mInstancesNames.Find(IMComp);
#endif
		IMComp->UpdateComponentToWorld();

		for (int i = 0; i < iNumInstances; ++i)
		{
			FTransform OutInstanceTransform;
			IMComp->GetInstanceTransform(i,OutInstanceTransform,true);

			AStaticMeshActor* staticMeshActor = pWorld->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(),OutInstanceTransform, Params);
			UStaticMeshComponent* pSMComp = staticMeshActor->GetStaticMeshComponent();

			pSMComp->SetStaticMesh(IMComp->GetStaticMesh());

			const int iNumMatls = IMComp->GetNumMaterials();
			for (int j = 0; j < iNumMatls; ++j)
			{
				pSMComp->SetMaterial(i, IMComp->GetMaterial(i));
			}			
			
			pSMComp->SetCanEverAffectNavigation(IMComp->CanEverAffectNavigation());
			pSMComp->SetCollisionEnabled(IMComp->GetCollisionEnabled());
			pSMComp->SetGenerateOverlapEvents(IMComp->GetGenerateOverlapEvents());
			pSMComp->SetCollisionProfileName(IMComp->GetCollisionProfileName());
			pSMComp->CanCharacterStepUpOn = IMComp->CanCharacterStepUpOn;
			pSMComp->bUseDefaultCollision = IMComp->bUseDefaultCollision;
			pSMComp->UpdateCollisionFromStaticMesh();

			UGameplayStatics::FinishSpawningActor(staticMeshActor, OutInstanceTransform);

#if WITH_EDITORONLY_DATA
			if (NameListContainer && NameListContainer->NameList.Num() > 0)
			{			
				if (i < NameListContainer->NameList.Num())
				{
					staticMeshActor->SetActorLabel(NameListContainer->NameList[i].ToString());

					if (staticMeshActor->Rename(*NameListContainer->NameList[i].ToString(),nullptr,REN_Test))
					{
						staticMeshActor->Rename(*NameListContainer->NameList[i].ToString());
					}
					else
					{
						//Unreal gets very unhappy trying to rename actors the same as current ones, lets try to find a new name
						FString OrigName = NameListContainer->NameList[i].ToString();
						UE_LOG(LogLoad, Warning, TEXT("Could Not rename new staticMeshActor to %s, most likely due to there already being an actor with that name"), *OrigName);
						
						int32 RenTest = 0;
						while (!staticMeshActor->Rename(*(OrigName + TEXT("_") + FString::FromInt(RenTest)), nullptr, REN_Test))
						{
							++RenTest;
						}

						staticMeshActor->Rename(*(staticMeshActor->GetName() + TEXT("_") + FString::FromInt(RenTest)));
						UE_LOG(LogLoad, Warning, TEXT("Renamed Actor to : %s"), *staticMeshActor->GetName());
					}
				}
				else
				{
					UE_LOG(LogLoad, Warning, TEXT("NumInstances Mismatch with container, created mesh will be created with static name : %s"), *staticMeshActor->GetName());
				}
				
			}
#endif
		}
	}
}

void AInstancedMeshesActor::Serialize(FArchive& Ar)
{
#if WITH_EDITORONLY_DATA
	if (Ar.IsCooking())
	{
		//We dont want these in game at all, just wipe them during cook
		mInstancesNames = TMap< UInstancedStaticMeshComponent*, FInstancedMeshesActorNameList >();
	}
#endif

	Super::Serialize(Ar);
}

UInstancedStaticMeshComponent& AInstancedMeshesActor::_GetOrCreateInstancedMeshFor(UStaticMesh* mesh, UStaticMeshComponent* meshSourceComponent, bool NavigationRelevant) {
	
	auto& CompNameMapInstances = mInstanceLookup.FindOrAdd(mesh);

	FString MaterialNameComposite = mesh->GetName();

	if (meshSourceComponent)
	{
		if(NavigationRelevant)
			MaterialNameComposite += TEXT("_N");

		if (meshSourceComponent->GetCollisionEnabled() != ECollisionEnabled::NoCollision)
		{
			const FString CollEnabled[4] = { TEXT("_CN") ,TEXT("_CQ") ,TEXT("_CP"),TEXT("_CQP") };
			MaterialNameComposite += CollEnabled[meshSourceComponent->GetCollisionEnabled()];			

			if (meshSourceComponent->GetGenerateOverlapEvents())
				MaterialNameComposite += TEXT("_OL");

			const FString StepOn[ECanBeCharacterBase::ECB_MAX] = { TEXT("_SN") ,TEXT("_SY") ,TEXT("_SO") };
			MaterialNameComposite += StepOn[meshSourceComponent->CanCharacterStepUpOn];
			
			MaterialNameComposite += TEXT("_") + meshSourceComponent->GetCollisionProfileName().ToString() + TEXT("_");
		}

		const int iNumMatls = meshSourceComponent->GetNumMaterials();

		if (iNumMatls > 0)
		{
			for (int i = 0; i < iNumMatls; ++i)
			{
				MaterialNameComposite += GetNameSafe(meshSourceComponent->GetMaterial(i));
			}
		}
	}	
	
	if (auto* instancedMeshComponent = CompNameMapInstances.Find(MaterialNameComposite))
	{
		return **instancedMeshComponent;
	}

	UClass* instanceClass = mHierarchical ? UHierarchicalInstancedStaticMeshComponent::StaticClass() : UInstancedStaticMeshComponent::StaticClass();
	UHierarchicalInstancedStaticMeshComponent* meshComponent = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, instanceClass);

	meshComponent->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	meshComponent->SetStaticMesh(mesh);
	meshComponent->bUseDefaultCollision = true;
	meshComponent->SetCanEverAffectNavigation(NavigationRelevant);

	if (meshSourceComponent)
	{
		const int iNumMatls = meshSourceComponent->GetNumMaterials();
		for (int i = 0; i < iNumMatls; ++i)
		{
			meshComponent->SetMaterial(i, meshSourceComponent->GetMaterial(i));
		}

		meshComponent->SetCollisionEnabled(meshSourceComponent->GetCollisionEnabled());
		meshComponent->SetGenerateOverlapEvents(meshSourceComponent->GetGenerateOverlapEvents());
		meshComponent->SetCollisionProfileName(meshSourceComponent->GetCollisionProfileName());
		meshComponent->CanCharacterStepUpOn = meshSourceComponent->CanCharacterStepUpOn;
		meshComponent->bUseDefaultCollision = meshSourceComponent->bUseDefaultCollision;
	}

	meshComponent->RegisterComponent();
	CompNameMapInstances.Add(MaterialNameComposite, meshComponent);
	mInstances.Push(meshComponent);

	return *meshComponent;
}
