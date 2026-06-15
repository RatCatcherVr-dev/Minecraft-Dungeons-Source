#include "Dungeons.h"
#include "DungeonsLevelStreamingDynamic.h"
#include "StaticToInstancedMeshConverter.h"

#if WITH_EDITOR

#include <EditorLevelUtils.h>
#include "Editor.h"

#endif


UDungeonsLevelStreamingDynamic::UDungeonsLevelStreamingDynamic(class FObjectInitializer const & initialiser)
	:
	Super(initialiser)
{
}


void UDungeonsLevelStreamingDynamic::PostLoad()
{

#if WITH_EDITOR
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("## UDungeonsLevelStreamingDynamic::PostLoad %s##\n"), *GetNameSafe(this));
	UE_LOG(LogDungeons, Warning, TEXT("## UDungeonsLevelStreamingDynamic::PostLoad %s##\n"), *GetNameSafe(this));


	FScriptDelegate funcDelegate;
	funcDelegate.BindUFunction(this, "OnLevelLoadedCall");
	OnLevelLoaded.AddUnique(funcDelegate);
	
#endif

	Super::PostLoad();
}


void UDungeonsLevelStreamingDynamic::OnLevelLoadedCall()
{
#if WITH_EDITOR
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("## UDungeonsLevelStreamingDynamic::OnLevelLoaded %s ##\n"), *GetNameSafe(this));
	
	
	TArray<AActor*> ActorsToDestroy;
	ConvertInstancedMeshes(false, ActorsToDestroy);

	auto world = GetWorld();

	for (auto* actor : ActorsToDestroy)
		world->EditorDestroyActor(actor, false);
		

	OnLevelLoaded.RemoveAll(this);
	
	GEngine->ForceGarbageCollection(true);
	GEngine->PerformGarbageCollectionAndCleanupActors();
#endif
}



void UDungeonsLevelStreamingDynamic::ConvertInstancedMeshes(bool ConvertTo, TArray<AActor*>& ActorsToDestroy)
{
#if WITH_EDITOR
	AStaticToInstancedMeshConverter Converter;
	if (ConvertTo)
	{
		Converter.ConvertTo(this, true, ActorsToDestroy);
	}
	else
	{
		Converter.ConvertFrom(this, ActorsToDestroy);
	}

#endif

}
