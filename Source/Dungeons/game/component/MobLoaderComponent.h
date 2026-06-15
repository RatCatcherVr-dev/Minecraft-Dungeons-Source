// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "world/entity/EntityTypes.h"
#include <Engine/StreamableManager.h>
#include "MobLoaderComponent.generated.h"


namespace io {
	struct MobGroup;
}

UCLASS( ClassGroup=(Custom), Within=GameBP, meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UMobLoaderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMobLoaderComponent();

	TSet<EntityType> ExtractMissionMobs() const;

	//Will expand requested mobs with dependencies defined in AdditionalTypesToLoad and load
	TSharedPtr<FStreamableHandle> AsyncLoadMobs(const TSet<EntityType>&, FStreamableDelegate DelegateToCall = {}, TAsyncLoadPriority Priority = FStreamableManager::DefaultAsyncLoadPriority);

	//Tries to load the specified mobs
	TSharedPtr<FStreamableHandle> AsyncLoadMobsExact(const TSet<EntityType>&, FStreamableDelegate DelegateToCall = {}, TAsyncLoadPriority Priority = FStreamableManager::DefaultAsyncLoadPriority);

	bool HasLoadedMob(EntityType) const;
	UClass* GetClass(EntityType) const;
	
	static TSet<EntityType> ExtractEntitiesFromGroup(const io::MobGroup&);
	
private:
	TSet<EntityType> AddAdditionalTypesToLoad(const TSet<EntityType>&) const;
	
	class AGameBP* GetGame() const;
	
	void OnMobsLoaded(TSet<EntityType>, FStreamableDelegate);
	
	UPROPERTY(Transient)
	TMap<EntityType, UClass*> LoadedMobBPs;


	TMap<EntityType, TArray<EntityType>> AdditionalTypesToLoad;
};
