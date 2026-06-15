#include "Dungeons.h"
#include "StaticToInstancedMeshConverter.h"
#include "InstancedMeshesActor.h"
#include <Engine/LevelStreaming.h>
#include <Engine/StaticMeshActor.h>
#include "game/conversion.h"
#include "world/level/BlockPos.h"

namespace {

const FName NoInstancingTag = "NoInstancing";

UStaticMeshComponent* GetConvertibleStaticMeshComponent(AActor* actor) {
	if (auto* staticMeshActor = Cast<AStaticMeshActor>(actor)) {
		if (staticMeshActor->ActorHasTag(NoInstancingTag)) {
			return nullptr;
		}
		auto* mesh = staticMeshActor->GetStaticMeshComponent();
		if (mesh->ComponentHasTag(NoInstancingTag)) {
			return nullptr;
		}
		static const TSet<FName> SkippedNames {
			"Plane",
			"Cube",
			"SM_Ivy_16x16"
		};
		if (!mesh->GetStaticMesh()) {
			return nullptr;
		}
		if (SkippedNames.Contains(mesh->GetStaticMesh()->GetFName())) {
			return nullptr;
		}
		else
		{
			FString name;
			mesh->GetStaticMesh()->GetFName().ToString(name);
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("\nMesh %s is not skipped by instancing\n"), *name );
		}
		
		// D11.DH
		// Don't instance hidden meshes
		if (mesh->bHiddenInGame || actor->bHidden)
		{
			return nullptr;
		}
		
		return mesh;
	}
	return nullptr;
}

TAutoConsoleVariable<int32> CVarInstancingEnabled(
	TEXT("Dungeons.Instancing.Enabled"),
	0,
	TEXT("Sets mesh instancing to enabled or disabled\n")
	TEXT("0: disabled\n")
	TEXT("1: enabled\n"),
	ECVF_Default
);


TAutoConsoleVariable<int32> CVarInstancingSubRegionSize(
	TEXT("Dungeons.Instancing.SubRegion"),
	64,
	TEXT("Sets mesh instancing subRegion size, to stipulate areas as single actor is responsible for"),
	ECVF_Default
);

}

void AStaticToInstancedMeshConverter::Convert(ULevelStreaming* level) {
	if (!CVarInstancingEnabled.GetValueOnGameThread()) {
		return;
	}
	TArray<AActor*> ActorsToDestroy;
	AStaticToInstancedMeshConverter::ConvertTo(level, false, ActorsToDestroy);

	for (auto* pActor : ActorsToDestroy)
	{
		pActor->Destroy();
	}
}

void AStaticToInstancedMeshConverter::ConvertTo(ULevelStreaming* level, bool Editor, TArray<AActor*>& ActorsToDestroy) {
	
	int32 RegionSize = CVarInstancingSubRegionSize.GetValueOnGameThread();
	
	if (RegionSize == 0)
	{
		//standard, 1 static mesh instance actor per level
		AInstancedMeshesActor* instancedMeshes = nullptr;

		for (auto* actor : level->GetLoadedLevel()->Actors) {
			if (auto* staticMeshComponent = GetConvertibleStaticMeshComponent(actor)) {

				const bool NavRelevant = (staticMeshComponent->CanEverAffectNavigation() && actor->IsComponentRelevantForNavigation(staticMeshComponent));

				if (!instancedMeshes) {
					instancedMeshes = level->GetWorld()->SpawnActor<AInstancedMeshesActor>(actor->GetActorLocation(), FRotator::ZeroRotator);
				}
				instancedMeshes->Place(staticMeshComponent->GetStaticMesh(), actor->GetTransform(), NavRelevant);
				ActorsToDestroy.Push(actor);
			}
		}
	}
	else
	{

		//responsibility split over areas
		TMap< FString, AInstancedMeshesActor* > instancedMeshActors;
		TArray<AActor*> SourceActors = level->GetLoadedLevel()->Actors;

		for (auto* actor : SourceActors) {
			if (auto* staticMeshComponent = GetConvertibleStaticMeshComponent(actor)) {

				const bool NavRelevant = (staticMeshComponent->CanEverAffectNavigation() && actor->IsComponentRelevantForNavigation(staticMeshComponent));

				if (NavRelevant)
				{
					FPlatformMisc::LowLevelOutputDebugStringf(TEXT("### NAV CHECK ACTOR %s  is nav relevant, should it be? if not it will be bloating the nav memory\n"), *GetNameSafe(actor));
				}

				FTransform ActorTransform = actor->GetTransform();

				BlockPos ActorBlockPos = conversion::ueToBlock(ActorTransform.GetTranslation());

				FString subRegionName(level->GetLoadedLevel()->GetName() + (Editor ? "_E_" : "") + FString::FromInt(ActorBlockPos.x / RegionSize) + TEXT("_") + FString::FromInt(ActorBlockPos.y / RegionSize) + TEXT("_") + FString::FromInt(ActorBlockPos.z / RegionSize) + ((NavRelevant) ? "_NAV" : ""));

				AInstancedMeshesActor* instancedMeshes = instancedMeshActors.FindRef(subRegionName);

				if (!instancedMeshes)
				{
					FActorSpawnParameters Params;
					Params.Name = *subRegionName;
					Params.OverrideLevel = level->GetLoadedLevel();
					AInstancedMeshesActor* pNewActor = level->GetWorld()->SpawnActor<AInstancedMeshesActor>(actor->GetActorLocation(), FRotator::ZeroRotator, Params);
					instancedMeshes = instancedMeshActors.Add(subRegionName, pNewActor);
					#if WITH_EDITOR
					pNewActor->SetActorLabel(subRegionName,false);
					#endif
				}
				
				instancedMeshes->Place(staticMeshComponent, actor->GetTransform(), NavRelevant);
				ActorsToDestroy.Push(actor);
			}
		}

	}

}

void AStaticToInstancedMeshConverter::ConvertFrom(ULevelStreaming* level, TArray<AActor*>& ActorsToDestroy) {
	
	TArray<AActor*> SourceActors = level->GetLoadedLevel()->Actors;
	for (auto* actor : SourceActors) 
	{
		if (AInstancedMeshesActor* pInstActor = Cast<AInstancedMeshesActor>(actor))
		{
			//create actors from instances and kill this one
			pInstActor->SpawnStaticMeshesFromContainedInstances();
			ActorsToDestroy.Push(actor);
		}
	}

}




