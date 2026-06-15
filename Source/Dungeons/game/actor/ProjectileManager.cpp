#include "ProjectileManager.h"
#include "game/GameBP.h"
#include "item/StorableItem.h"
#include "game/actor/ProjectileManager.h"

const int InstantSpawnAll = WITH_EDITOR;

TAutoConsoleVariable<int32> CVarPlayerPurgedProjectilesRetentionCount(
	TEXT("Dungeons.Player.PlayerPurgedProjectilesRetentionCount"),
	4,
	TEXT("The default retention count of each projectile type after a purge.\n"),
	ECVF_Cheat);

AProjectileActorManager::AProjectileActorManager(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	if (!IsTemplate())
	{
		PrimaryActorTick.bCanEverTick = true;
	}
	else
	{
		PrimaryActorTick.bCanEverTick = false;
	}

	USphereComponent* Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("RootSphere"));

	Sphere->SetIsReplicated(false);
	Sphere->SetMobility(EComponentMobility::Movable);
	Sphere->SetSphereRadius(10.0f);
	Sphere->SetCanEverAffectNavigation(false);

	RootComponent = Sphere;


	static ConstructorHelpers::FObjectFinder<UParticleSystem> s_HitEffectBaseParticleTemplate(TEXT("ParticleSystem'/Game/Effects/ParticleSystems/Combat/P_ArrowHit_Generic.P_ArrowHit_Generic'"));
	check(s_HitEffectBaseParticleTemplate.Object); //need a default template
	if (s_HitEffectBaseParticleTemplate.Object != nullptr)
	{
		mBaseHitParticleSystemTemplate = (UParticleSystem*)s_HitEffectBaseParticleTemplate.Object;
	}

}

void AProjectileActorManager::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	//dont start spawning projectiles etc until world init done
	const auto game = actorquery::getFirstActor<AGameBP>(GetWorld());
	if (game && !game->IsInitialisationComplete())
	{
		return;
	}

	//spawn missing XPOrbs
	if (SpawnMissingXPOrbsCache())
	{
		return;
	}

	if (SpawnMissingOxygenBubblesCache())
	{
		return;
	}

	//spawn missing emeralds
	if (SpawnMissingStorableCache(game::item::type::Emerald, DefaultEmeraldPoolSize))
	{
		return;
	}

	//spawn missing gold
	if (SpawnMissingStorableCache(game::item::type::Gold, DefaultGoldPoolSize))
	{
		return;
	}
	
	//spawn missing particle hit effects from base
	if (SpawnMissingParticleHitsCache())
	{
		return;
	}

	if (SpawnMissingCachedClassInstances<ABaseProjectileProp>(ProjectilePropInstances))
	{
		return;
	}

	for (auto& PlayerProjectilesPair : PlayerProjectileInstances)
	{
		if (SpawnMissingCachedClassInstances<ABaseProjectile>(PlayerProjectilesPair.Value, [](ABaseProjectile* pNewInst) { pNewInst->ResetProjectile(); }))
		{
			return;
		}
	}
	
	if ( SpawnMissingCachedClassInstances<ABaseProjectile>(MobProjectileInstances, [](ABaseProjectile* pNewInst) { pNewInst->ResetProjectile(); }) )
	{
		return;
	}
	
	if (SpawnMissingCachedClassInstances<ASoul>(SoulInstances, [](ASoul* pNewInst) { pNewInst->StopSoul(false); }))
	{
		return;
	}

	if (SpawnMissingCachedClassInstances<AItemEmitterActor>(ItemEmitterInstances, [](AItemEmitterActor* pNewInst) { pNewInst->SetActorTickEnabled(false); }))
	{
		return;
	}

	if (PurgedPlayerProjectileClean())
	{
		return;
	}

	SetActorTickEnabled(false);
}


bool AProjectileActorManager::SpawnMissingParticleHitsCache()
{
	if (mProjectileHitParticleComponents.Num() < DefaultProjectileHitParticlePoolSize)
	{
		int iSpawnCount = (InstantSpawnAll) ? DefaultProjectileHitParticlePoolSize : 1;

		while (iSpawnCount--)
		{
			auto NewEmitter = UGameplayStatics::SpawnEmitterAtLocation(this->GetWorld(), mBaseHitParticleSystemTemplate, FVector(ForceInit), FRotator::ZeroRotator, FVector(1.f), false, EPSCPoolMethod::None);
			check(NewEmitter);

			NewEmitter->Activate(true); //reset then deactivate
			NewEmitter->Deactivate();
			NewEmitter->SetComponentTickEnabled(false);
			NewEmitter->SetVisibility(false);

			NewEmitter->OnSystemFinished.AddUniqueDynamic(this, &AProjectileActorManager::OnHitParticleSystemFinished);

			mProjectileHitParticleComponents.Push(NewEmitter);
		}

		return true;
	}

	return false;
}

bool AProjectileActorManager::SpawnMissingXPOrbsCache()
{
	if (mXPOrbInstances.Num() < DefaultXPOrbPoolSize)
	{
		int iSpawnCount = (InstantSpawnAll) ? DefaultXPOrbPoolSize : 4;

		while (iSpawnCount--)
		{
			if (mXPOrbInstances.Num() < DefaultXPOrbPoolSize)
			{
				AXPOrb* NewInst = Cast<AXPOrb>(SpawnNewActorClassInstance(AXPOrb::StaticClass()));
				mXPOrbInstances.Push(NewInst);
				NewInst->DisableOrb();
			}
		}
		
		return true;
	}

	if (mXPOrbSpawns.Num())
	{
		//do any delayed spawns
		const int iMaxOrbsPerTick = (InstantSpawnAll) ? mXPOrbSpawns.Num() : 2;
		int iOrbsLeft = iMaxOrbsPerTick;

		while (mXPOrbSpawns.Num() && iOrbsLeft--)
		{
			XPOrbSpawn& SpawnStruct = mXPOrbSpawns[0];

			if (SpawnStruct.iCount == 0)
			{
				++iOrbsLeft;
				mXPOrbSpawns.RemoveAtSwap(0);
			}
			else
			{
				if (SpawnStruct.pSource.IsValid() && SpawnStruct.pTarget.IsValid())
				{
					AXPOrb* pNewOrb = PopXPOrb();

					if (pNewOrb)
					{
						pNewOrb->LaunchOrb(SpawnStruct.pSource.Get(), SpawnStruct.pTarget.Get());
						--SpawnStruct.iCount;
					}
					else
					{
						//ran out of orbs
						mXPOrbSpawns.Reset();
						break;
					}
				}
				else
				{
					++iOrbsLeft;
					mXPOrbSpawns.RemoveAtSwap(0);
				}
			}
		}

		return true;
	}
	

	return false;
}

bool AProjectileActorManager::SpawnMissingOxygenBubblesCache() {
	if (mOxygenBubbleInstances.Num() < DefaultOxygenBubblePoolSize) {
		int iSpawnCount = (InstantSpawnAll) ? DefaultOxygenBubblePoolSize : 4;

		while (iSpawnCount--) {
			if (mOxygenBubbleInstances.Num() < DefaultOxygenBubblePoolSize) {
				AOxygenBubbleActor* NewInst = Cast<AOxygenBubbleActor>(SpawnNewActorClassInstance(AOxygenBubbleActor::StaticClass()));
				mOxygenBubbleInstances.Push(NewInst);
				NewInst->Disable();
			}
		}

		return true;
	}

	if (mOxygenBubbleSpawns.Num()) {
		//do any delayed spawns
		const int iMaxBubblesPerTick = (InstantSpawnAll) ? mOxygenBubbleInstances.Num() : 2;
		int iBubblesLeft = iMaxBubblesPerTick;

		while (mOxygenBubbleSpawns.Num() && iBubblesLeft--) {
			BubbleSpawnInfo& SpawnStruct = mOxygenBubbleSpawns[0];

			if (SpawnStruct.pTarget.IsValid()) {
				AOxygenBubbleActor* pNewBubble = PopOxygenBubble();

				if (pNewBubble) {
					pNewBubble->LaunchBubble(SpawnStruct.pSource, SpawnStruct.pTarget.Get());
				}
				else {
					//ran out of orbs
					mOxygenBubbleSpawns.Reset();
					break;
				}
			}

			mOxygenBubbleSpawns.RemoveAtSwap(0);
		}

		return true;
	}

	return false;
}

bool AProjectileActorManager::SpawnMissingStorableCache(const ItemType& itemType, int32 cacheSize)
{
	WeakStorableArray& instancesArray = GetStorableInstancesArray(itemType.getId());
	if (instancesArray.Num() < cacheSize)
	{
		auto pStorableClass = itemType.getStorableClass();
		if (pStorableClass) {
			int iPerLoopSpawnCount = (InstantSpawnAll) ? cacheSize : 2;

			while (iPerLoopSpawnCount--)
			{
				if (instancesArray.Num() < cacheSize)
				{
					AStorableItem* StorableInst = Cast<AStorableItem>(SpawnNewActorClassInstance(pStorableClass));
					ensureMsgf(StorableInst, TEXT(" Failed to spawn !"));
					StorableInst->ItemData = FInventoryItemData(itemType.getId());
					StorableInst->SetLifeSpan(0.0f);
					StorableInst->EnableMovementComponentTick(false);
					StorableInst->GetCollisionComponent()->SetSimulatePhysics(false);
					instancesArray.Push(StorableInst);
				}
			}
			return true;
		} else {
			ensureMsgf(pStorableClass, TEXT("No storable class for item %s, cannot pre-cache storables"), *itemType.getName());
			return false;
		}
	}

	return false;
}


void AProjectileActorManager::PurgeProjectileCache(TSubclassOf<ABaseProjectile> TargetClass, FProjectileCachedClass& TargetStore)
{
	if (TargetStore.InstanceList.Num() > 0)
	{
		//push the projectiles to the purge list
		TArray < ProjectileCachedClassInst > & PurgedProjectiles = PlayerProjectilePurgedInstances.FindOrAdd(TargetClass);
		TargetStore.InstanceList.RemoveAllSwap([&](const auto& candidate) { return !candidate.WeakInstance.IsValid(); }); //purge invalid ones				
		algo::for_each(TargetStore.InstanceList, [](ProjectileCachedClassInst& inst) { inst.LoanedOutTime = -1.0f; }); //invalidate loan time
		PurgedProjectiles.Append(TargetStore.InstanceList);
		TargetStore.InstanceList.Reset();
	}
}

ABaseProjectile* AProjectileActorManager::PopAPurgedPlayerProjectile(TSubclassOf<ABaseProjectile> ActorClass, TMap< TSubclassOf< ABaseProjectile >, FProjectileCachedClass >& TargetStore)
{
	ProjectileCachedClassInst* pOut = nullptr;

	TArray < ProjectileCachedClassInst > & PurgedProjectiles = PlayerProjectilePurgedInstances.FindOrAdd(ActorClass);

	if (PurgedProjectiles.Num() > 0)
	{
		//we have purged some of these, lets grab them back
		auto& CachedStore = TargetStore.FindOrAdd(ActorClass);

		auto& InstancesList = CachedStore.InstanceList;

		const int32 NumAvailable = InstancesList.Num();

		//pop the next valid projectile from the purge list		
		if (PurgedProjectiles.Num() > 0)
		{
			//theres ones left
			if (CachedStore.MaximumCachedCount < 0)
			{
				//just copy them all over
				InstancesList.Append(PurgedProjectiles);
				PurgedProjectiles.Reset();
				pOut = &InstancesList.Top();
			}
			else
			{
				//copy any left up to max cache
				while (PurgedProjectiles.Num() > 0 && InstancesList.Num() < CachedStore.MaximumCachedCount)
				{
					//Add to cache list
					ProjectileCachedClassInst InstCopy = PurgedProjectiles.Pop(false);
					InstancesList.Push(InstCopy);
					pOut = &InstancesList.Top();
				}
			}
		}
		

		if (pOut)
		{
			//we managed to get a valid one, lets fill the target store up from the purged list
			pOut->WeakInstance->SetActorTickEnabled(true);
			pOut->WeakInstance->SetActorHiddenInGame(false);
			pOut->LoanedOutTime = GetWorld()->GetTimeSeconds();
			StartInitialisation();
			return Cast<ABaseProjectile>(pOut->WeakInstance.Get());
		}

	}

	return nullptr;
}

bool AProjectileActorManager::PurgedPlayerProjectileClean()
{
	//purge a projectile per frame until we hit the retention value
	const int32 iMaxRetentionValue = CVarPlayerPurgedProjectilesRetentionCount.GetValueOnGameThread();

	bool bContinue = false;

	for (auto& RetentionPair : PlayerProjectilePurgedInstances)
	{
		if (RetentionPair.Value.Num() > iMaxRetentionValue)
		{
			ProjectileCachedClassInst WeakProjectile = RetentionPair.Value.Pop();
			if (WeakProjectile.WeakInstance.IsValid())
			{
				UE_LOG(LogDungeonsProjectile, Log, TEXT("##PurgedPlayerProjectileClean a Class %s\n"), *WeakProjectile.WeakInstance->GetClass()->GetName());

				WeakProjectile.WeakInstance->OnDestroyed.RemoveDynamic(this, &AProjectileActorManager::OnPoolItemDestroyed);
				WeakProjectile.WeakInstance->Destroy();
				bContinue = true;
				break;
			}
		}
	}
	
	return bContinue;
}

template <class T>
bool AProjectileActorManager::SpawnMissingCachedClassInstances(TMap< TSubclassOf< T >, FProjectileCachedClass >& TargetStore, std::function< void(T*) > PostSpawn)
{
	//spawn a missing cache projectile instance per tick
	for (TPair< TSubclassOf<T>, FProjectileCachedClass >& pairInstance : TargetStore)
	{
		TSubclassOf<T>& InstCachedClass = pairInstance.Key;
		FProjectileCachedClass& InstCachedStore = pairInstance.Value;

		if (InstCachedStore.InstanceList.Num() < InstCachedStore.MinimumCachedCount)
		{
			//spawn one of these arrow props
			T* NewInst = Cast<T>(SpawnNewActorClassInstance(InstCachedClass));

			PostSpawn(NewInst);

			//Add to cache list
			ProjectileCachedClassInst CachedInst(NewInst);
			InstCachedStore.InstanceList.Push(CachedInst);

			return true;
		}
	}

	return false;
}

AActor* AProjectileActorManager::SpawnNewActorClassInstance(TSubclassOf< AActor > ActorClass)
{
	UE_LOG(LogDungeonsProjectile, Verbose, TEXT("##SpawnNewActorClassInstance a Class %s\n"), *ActorClass->GetName());

	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParams.Owner = this;

	if (AActor* pNewInstance = this->GetWorld()->SpawnActor< AActor >(ActorClass, FTransform(), spawnParams))
	{

		pNewInstance->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		pNewInstance->SetActorTickEnabled(false);
		pNewInstance->SetActorHiddenInGame(true);
		
		//ensureMsgf((pNewInstance->InitialLifeSpan == 0.0f), TEXT(" These must live forever as they are cached !"));
		pNewInstance->SetLifeSpan(0.0f);

		pNewInstance->OnDestroyed.AddDynamic(this, &AProjectileActorManager::OnPoolItemDestroyed);

		return pNewInstance;
	}
	

	return nullptr;
}


template <class T>
T* AProjectileActorManager::PopAClassInst(TSubclassOf<T> ActorClass, FTransform Transform, TMap< TSubclassOf< T >, FProjectileCachedClass >& TargetStore)
{
	ProjectileCachedClassInst* pOut = nullptr;

	auto& CachedStore = TargetStore.FindOrAdd(ActorClass);

	auto& InstancesList = CachedStore.InstanceList;

	const int32 NumAvailable = InstancesList.Num();

	float fOldest = FLT_MAX;

	//find the next available instance
	if (CachedStore.ReuseOldestInstance == FProjectileCachedClass::EReuseSetting::E_Enabled)
	{
		for (auto& InstRef : InstancesList)
		{
			if (InstRef.WeakInstance.IsValid())
			{
				if (InstRef.LoanedOutTime < 0.0f)
				{
					//found one!
					pOut = &InstRef;
					break;
				}
				else if (InstRef.LoanedOutTime < fOldest)
				{
					pOut = &InstRef;
					fOldest = InstRef.LoanedOutTime;
				}
			}
		}

		//reset oldest connection/positional data as would previously have been done for a returned prop
		if (pOut)
		{
			pOut->WeakInstance->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			pOut->WeakInstance->SetActorRelativeTransform(FTransform::Identity, false, nullptr, ETeleportType::ResetPhysics);
			pOut->WeakInstance->SetActorTransform(FTransform::Identity, false, nullptr, ETeleportType::ResetPhysics);
			pOut->WeakInstance->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
		}
	}
	else
	{
		for (auto& InstRef : InstancesList)
		{
			if (InstRef.WeakInstance.IsValid() && InstRef.LoanedOutTime < 0.0f)
			{
				//found one!
				pOut = &InstRef;
				break;
			}
		}
	}

	

	if (pOut)
	{
		pOut->WeakInstance->SetActorTransform(std::move(Transform), false, nullptr, ETeleportType::ResetPhysics);
		pOut->WeakInstance->SetActorTickEnabled(true);
		pOut->WeakInstance->SetActorHiddenInGame(false);
	}
	else if (CachedStore.MaximumCachedCount <= 0 || NumAvailable < CachedStore.MaximumCachedCount)
	{
		//trigger initialization loop again to fill out cache
		StartInitialisation();

		//Not fully initialised, should tell us we need to initialize this type
	
		UE_LOG(LogDungeonsProjectile, Log, TEXT("##################################\n"));
		UE_LOG(LogDungeonsProjectile, Log, TEXT("##Spawning a Class %s\n"), *ActorClass->GetName());
		UE_LOG(LogDungeonsProjectile, Log, TEXT("##################################\n"));

		ProjectileCachedClassInst NewInst;
		NewInst.WeakInstance = Cast<T>(SpawnNewActorClassInstance(ActorClass));
		
		NewInst.WeakInstance->SetActorTransform(std::move(Transform), false, nullptr, ETeleportType::ResetPhysics);
		NewInst.WeakInstance->SetActorTickEnabled(true);
		NewInst.WeakInstance->SetActorHiddenInGame(false);

		//Add to blank placeholder for return cache list		
		CachedStore.InstanceList.Reserve(CachedStore.MinimumCachedCount);
		CachedStore.InstanceList.Push(NewInst);
		pOut = &CachedStore.InstanceList.Top();
	}

	pOut->LoanedOutTime = GetWorld()->GetTimeSeconds();
	return Cast<T>(pOut->WeakInstance.Get());
}


template <class T>
void AProjectileActorManager::PushAClassInst(T* pReturned, TMap< TSubclassOf< T >, FProjectileCachedClass >& TargetStore)
{
	if (!pReturned)
	{
		return;
	}

	//disable and remove arrow and attach it to this	
	pReturned->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	pReturned->SetActorRelativeTransform(FTransform::Identity, false, nullptr, ETeleportType::ResetPhysics);
	pReturned->SetActorTransform(FTransform::Identity, false, nullptr, ETeleportType::ResetPhysics);
	pReturned->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));

	pReturned->SetActorTickEnabled(false);
	pReturned->SetActorHiddenInGame(true);

	auto& CachedStore = TargetStore.FindOrAdd(pReturned->GetClass());

	auto& InstancesList = CachedStore.InstanceList;

	int32 NumAvailable = InstancesList.Num();

	if (NumAvailable < CachedStore.MinimumCachedCount)
	{
		InstancesList.Reserve(CachedStore.MaximumCachedCount);
		//trigger initialization loop again to fill out cache
		StartInitialisation();
	}

	//find out pooled projectile and reset its loan time
	for (auto& InstRef : InstancesList)
	{
		if (InstRef.WeakInstance.Get() == pReturned)
		{
			//found it
#if WITH_EDITOR
			check((InstRef.LoanedOutTime > 0.0f) && "NOT LOANED OUT! somethings trying to return this twice!");
#endif
			InstRef.LoanedOutTime = -1.0f;
			return;
		}
	}
	

	//if we get here somethings managed to spawn more outside of the pool :/, oh well, lets just take this one too :)
	ProjectileCachedClassInst NewInst(pReturned);
	InstancesList.Push(NewInst);
}

ABaseProjectileProp* AProjectileActorManager::PopAProjectileProp(TSubclassOf<ABaseProjectileProp> ProjectilePropClass, FTransform Transform)
{
	auto& CachedStore = ProjectilePropInstances.FindOrAdd(ProjectilePropClass);
	if (CachedStore.ReuseOldestInstance == FProjectileCachedClass::EReuseSetting::E_Unset)
	{
		if(ABaseProjectileProp* DefaultInst = GetMutableDefault<ABaseProjectileProp>(ProjectilePropClass))
		{
			CachedStore.ReuseOldestInstance = (DefaultInst->ReplaceOldInstances) ? FProjectileCachedClass::EReuseSetting::E_Enabled : FProjectileCachedClass::EReuseSetting::E_Disabled;
		}

	}

	return PopAClassInst<ABaseProjectileProp>(ProjectilePropClass, std::move(Transform), ProjectilePropInstances);
}

void AProjectileActorManager::PushAProjectileProp(ABaseProjectileProp* pReturnedProp)
{
	if (pReturnedProp)
	{
		pReturnedProp->Reset();
		PushAClassInst<ABaseProjectileProp>(pReturnedProp, ProjectilePropInstances);
	}	
}

void AProjectileActorManager::PreCacheAProjectilePropClass(TSubclassOf<ABaseProjectileProp> ProjectilePropClass, int32 MinimumCachedCount/* = 16*/, int32 MaximumCachedCount/* = -1*/)
{
	//just add the cache struct, the population tick will take care of the rest
	auto& CachedStore = ProjectilePropInstances.FindOrAdd(ProjectilePropClass);
	CachedStore.MinimumCachedCount = MinimumCachedCount;
	CachedStore.MaximumCachedCount = MaximumCachedCount;
	CachedStore.InstanceList.Reserve((CachedStore.MaximumCachedCount > 0) ? CachedStore.MaximumCachedCount : CachedStore.MinimumCachedCount);
	StartInitialisation();
}

ABaseProjectile* AProjectileActorManager::PopAProjectile(TSubclassOf<ABaseProjectile> ProjectileClass, FTransform Transform, APawn* pInstigator)
{
	ABaseProjectile* pOut = nullptr;

	if (APlayerCharacter* pChar = Cast<APlayerCharacter>(pInstigator))
	{
		auto& PlayerMap = PlayerProjectileInstances.FindOrAdd(pChar);

		if (PlayerMap.Find(ProjectileClass) == nullptr)
		{
			//new arrow type, technically shouldnt happen, but catches any external influences on the players projectile casting
			pOut = PopAPurgedPlayerProjectile(ProjectileClass, PlayerMap); //try purged list first
		}

		if (!pOut)
		{
			pOut = PopAClassInst<ABaseProjectile>(ProjectileClass, std::move(Transform), PlayerMap);
		}
	}
	else
	{
		pOut = PopAClassInst<ABaseProjectile>(ProjectileClass, std::move(Transform), MobProjectileInstances);
	}

	pOut->Instigator = pInstigator;
	pOut->InstigatorHadAuthority = pInstigator ? pInstigator->HasAuthority() : GetWorld()->IsServer();
	return pOut;
}

void AProjectileActorManager::PushAProjectile(ABaseProjectile* pReturnedProjectile)
{
	if (pReturnedProjectile)
	{
		if (APlayerCharacter* pChar = Cast<APlayerCharacter>(pReturnedProjectile->Instigator))
		{
			UClass* ProjectileClass = pReturnedProjectile->GetClass();

			pReturnedProjectile->DisableCollisions(); //disable collisions to prevent overlap gen during re-push to manager parent

			if (auto* PlayerMap = PlayerProjectileInstances.Find(pChar))
			{
				if (PlayerMap->Find(ProjectileClass) == nullptr)
				{
					pReturnedProjectile->ResetProjectile();
					//wrong current class type
					TArray < ProjectileCachedClassInst > & PurgedProjectiles = PlayerProjectilePurgedInstances.FindOrAdd(ProjectileClass);
					ProjectileCachedClassInst NewInst(pReturnedProjectile);
					PurgedProjectiles.Push(NewInst);					
					StartInitialisation();
					return;
				}
				
				PushAClassInst<ABaseProjectile>(pReturnedProjectile, *PlayerMap);
			}
			else
			{
				pReturnedProjectile->ResetProjectile();
				//no player cache for this projectile, add to purge list
				TArray < ProjectileCachedClassInst > & PurgedProjectiles = PlayerProjectilePurgedInstances.FindOrAdd(ProjectileClass);
				ProjectileCachedClassInst NewInst(pReturnedProjectile);
				PurgedProjectiles.Push(NewInst);
				StartInitialisation();
				return;
			}
		}
		else
		{
			pReturnedProjectile->ResetProjectile();
			PushAClassInst<ABaseProjectile>(pReturnedProjectile, MobProjectileInstances);
		}

		pReturnedProjectile->ResetProjectile();
	}
}

void AProjectileActorManager::PurgeAllPlayerProjectiles(APawn* pInstigator)
{
	
	if (APlayerCharacter* pChar = Cast<APlayerCharacter>(pInstigator))
	{
		//purge this players cached projectiles

		if (auto* pPlayeProjectilesMap = PlayerProjectileInstances.Find(pChar))
		{
			if (pPlayeProjectilesMap->Num() > 0)
			{
				for (auto& KeyPairVal : *pPlayeProjectilesMap)
				{
					PurgeProjectileCache(KeyPairVal.Key, KeyPairVal.Value);
				}

				pPlayeProjectilesMap->Reset();
			}
		}

		//remove cache
		PlayerProjectileInstances.Remove(pChar);
	}
	
}

void AProjectileActorManager::PreCacheAProjectileClass(TSubclassOf<ABaseProjectile> ProjectileClass)
{
	//just add the cache struct, the population tick will take care of the rest
	auto& CachedStore = MobProjectileInstances.FindOrAdd(ProjectileClass);
	CachedStore.InstanceList.Reserve((CachedStore.MaximumCachedCount > 0) ? CachedStore.MaximumCachedCount : CachedStore.MinimumCachedCount);
}

void AProjectileActorManager::PreCachePlayerProjectileClasses(TArray< TSubclassOf< ABaseProjectile > >& NewProjectiles, APlayerCharacter* pPlayerOwner)
{
	auto& PlayerMap = PlayerProjectileInstances.FindOrAdd(pPlayerOwner);

	TArray< TSubclassOf< ABaseProjectile > > OldProjectiles;
	PlayerMap.GetKeys(OldProjectiles);
	
	//remove new definitions from old definitions list
	for (auto& NewProjectileClass : NewProjectiles)
	{
		OldProjectiles.RemoveSwap(NewProjectileClass);
	}
		
	//purge old projectiles
	for (auto& OldProjectileClass : OldProjectiles)
	{
		PurgeProjectileCache(OldProjectileClass, PlayerMap[OldProjectileClass]);
		PlayerMap.Remove(OldProjectileClass);
	}

	//new projectiles, just make the caches, the init tick should fill them up in a frame rate friendly manner
	for (auto& NewProjectileClass : NewProjectiles)
	{
		auto& CachedStore = PlayerMap.FindOrAdd(NewProjectileClass);	
		CachedStore.InstanceList.Reserve((CachedStore.MaximumCachedCount > 0) ? CachedStore.MaximumCachedCount : CachedStore.MinimumCachedCount);
	}
	
	StartInitialisation();
}

ASoul* AProjectileActorManager::PopASoul(TSubclassOf<ASoul> SoulClass, FTransform Transform)
{
	return PopAClassInst<ASoul>(SoulClass, std::move(Transform), SoulInstances);
}

void AProjectileActorManager::PushASoul(ASoul* pReturned)
{
	if (pReturned)
	{
		PushAClassInst<ASoul>(pReturned, SoulInstances);
	}
}

AItemEmitterActor* AProjectileActorManager::PopAnItemEmitter(TSubclassOf<AItemEmitterActor> ItemEmitterActorClass)
{
	return PopAClassInst<AItemEmitterActor>(ItemEmitterActorClass, FTransform::Identity, ItemEmitterInstances);
}

void AProjectileActorManager::PushAnItemEmitter(AItemEmitterActor* pReturned)
{
	if (pReturned)
	{
		PushAClassInst<AItemEmitterActor>(pReturned, ItemEmitterInstances);
	}
}

void AProjectileActorManager::StartInitialisation()
{
	SetActorTickEnabled(true);
}

void AProjectileActorManager::OnHitParticleSystemFinished(class UParticleSystemComponent* FinishedComponent)
{	
	FinishedComponent->Activate(true); //reset then deactivate
	FinishedComponent->Deactivate();
	FinishedComponent->SetComponentTickEnabled(false);
	FinishedComponent->SetVisibility(false);
}

void AProjectileActorManager::OnPoolItemDestroyed(AActor* DestroyedActor)
{
	ensureMsgf(false, TEXT("DESTROYED!!!"));
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##!! OnPoolItemDestroyed a Class (%s) %s\n"), *DestroyedActor->GetClass()->GetName(),*DestroyedActor->GetName());
}

bool AProjectileActorManager::HasStorableInstancesArray(const FItemId& itemId) const {
	return mStorableInstancesMap.Contains(itemId);
}

AProjectileActorManager::WeakStorableArray& AProjectileActorManager::GetStorableInstancesArray(const FItemId& itemId) {
	check(mStorableInstancesMap.Contains(itemId) && "trying to get an instances array which is not initialized");
	return mStorableInstancesMap[itemId];
}

void AProjectileActorManager::EnableStorableInstancesCachingFor(const ItemType& itemType){
	mStorableInstancesMap.Add(itemType.getId(), {});
}

void AProjectileActorManager::BeginPlay()
{
	Super::BeginPlay();
	InstanceTracker< AProjectileActorManager >::AddInstance(GetWorld(), this);
	
	mXPOrbSpawns.Reserve(64); //reserve some to reduce array resizing

	EnableStorableInstancesCachingFor(game::item::type::Emerald);
	EnableStorableInstancesCachingFor(game::item::type::Gold);
	
	ReserveClassInstances(ProjectilePropInstances);
	ReserveClassInstances(MobProjectileInstances);
	ReserveClassInstances(SoulInstances);
	ReserveClassInstances(ItemEmitterInstances);


	StartInitialisation();
}


template <class T>
void AProjectileActorManager::ReserveClassInstances(TMap< TSubclassOf< T >, FProjectileCachedClass >& TargetList)
{
	for (TPair< TSubclassOf<T>, FProjectileCachedClass >& pairProp : TargetList)
	{
		FProjectileCachedClass& CachedStore = pairProp.Value;
		CachedStore.InstanceList.Reserve((CachedStore.MaximumCachedCount > 0) ? CachedStore.MaximumCachedCount : CachedStore.MinimumCachedCount);
	}
}

void AProjectileActorManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	InstanceTracker< AProjectileActorManager >::RemoveInstance(GetWorld(), this);
	Super::EndPlay(EndPlayReason);
}

AProjectileActorManager* AProjectileActorManager::CheckProjectileActorManagerExists(UWorld* pWorld)
{
	if (pWorld)
	{
		ensureMsgf((InstanceTracker< AProjectileActorManager >::GetList(pWorld).Num() == 1), TEXT("Please Make sure a single AProjectileActorManager has been added to the world "));

		if (InstanceTracker< AProjectileActorManager >::GetList(pWorld).Num() != 1)
		{
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##!!####!!####!!####!!####!!####!!####!!####!!####!!####!!####!!####!!####!!####!!#####!!##"));
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##!!##                                                                               ##!!##"));
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##!!## Please Make sure a single AProjectileActorManager has been added to the world ##!!##"));
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##!!##                                                                               ##!!##"));
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("##!!####!!####!!####!!####!!####!!####!!####!!####!!####!!####!!####!!####!!####!!#####!!##"));

			return nullptr;
		}

		return InstanceTracker< AProjectileActorManager >::GetList(pWorld)[0];
	}

	return nullptr;	
}

ABaseProjectileProp* AProjectileActorManager::Pop_ProjectileProp(TSubclassOf<ABaseProjectileProp> ProjectilePropClass, FTransform Transform, UWorld* pWorld)
{	
	AProjectileActorManager* pManager = CheckProjectileActorManagerExists(pWorld);
	check(pManager);

	if (pManager)
	{
		return pManager->PopAProjectileProp(ProjectilePropClass, std::move(Transform));
	}

	return nullptr;
}

void AProjectileActorManager::Push_ProjectileProp(ABaseProjectileProp* ReturnedProjectileProp)
{
	if (ReturnedProjectileProp)
	{
		AProjectileActorManager* pManager = CheckProjectileActorManagerExists(ReturnedProjectileProp->GetWorld());
		check(pManager);

		if (pManager)
		{
			pManager->PushAProjectileProp(ReturnedProjectileProp);
		}
	}	
}

ABaseProjectile* AProjectileActorManager::Pop_Projectile(TSubclassOf<ABaseProjectile> ProjectileClass, FTransform Transform, UWorld* world)
{
	AProjectileActorManager* pManager = CheckProjectileActorManagerExists(world);
	check(pManager);

	if (pManager)
	{
		return pManager->PopAProjectile(ProjectileClass, Transform, nullptr);
	}

	return nullptr;
}


ABaseProjectile* AProjectileActorManager::Pop_Projectile(TSubclassOf<ABaseProjectile> ProjectileClass, FTransform Transform, APawn* pInstigator)
{
	AProjectileActorManager* pManager = CheckProjectileActorManagerExists(pInstigator->GetWorld());
	check(pManager);

	if (pManager)
	{
		return pManager->PopAProjectile(ProjectileClass, Transform, pInstigator);
	}

	return nullptr;
}

void AProjectileActorManager::PreCache_ProjectilePropClass(TSubclassOf<ABaseProjectileProp> ProjectilePropClass, UWorld* world, int32 MinimumCachedCount /*= 16*/, int32 MaximumCachedCount /*= -1*/)
{
	AProjectileActorManager* pManager = CheckProjectileActorManagerExists(world);
	check(pManager);

	if (pManager)
	{
		pManager->PreCacheAProjectilePropClass(ProjectilePropClass, MinimumCachedCount, MaximumCachedCount);
	}
}

void AProjectileActorManager::PreCache_ProjectileClass(TSubclassOf<ABaseProjectile> ProjectileClass, UWorld* world)
{
	AProjectileActorManager* pManager = CheckProjectileActorManagerExists(world);
	check(pManager);

	if (pManager)
	{
		pManager->PreCacheAProjectileClass(ProjectileClass);
	}
}

void AProjectileActorManager::PreCache_PlayerProjectiles(TArray< TSubclassOf< ABaseProjectile > >& NewProjectiles, APlayerCharacter* pPlayerOwner)
{
	AProjectileActorManager* pManager = CheckProjectileActorManagerExists(pPlayerOwner->GetWorld());
	check(pManager);

	if (pManager)
	{
		return pManager->PreCachePlayerProjectileClasses(NewProjectiles, pPlayerOwner);
	}
}

void AProjectileActorManager::Push_Projectile(ABaseProjectile* ReturnedProjectile)
{
	if (ReturnedProjectile)
	{
		
		AProjectileActorManager* pManager = CheckProjectileActorManagerExists(ReturnedProjectile->GetWorld());
		check(pManager);

		if (pManager)
		{
			pManager->PushAProjectile(ReturnedProjectile);
		}
	}
}

void AProjectileActorManager::Purge_Projectiles(APawn* pInstigator)
{
	if (pInstigator)
	{
		AProjectileActorManager* pManager = CheckProjectileActorManagerExists(pInstigator->GetWorld());		
		if (pManager)
		{
			pManager->PurgeAllPlayerProjectiles(pInstigator);
		}
	}
}

ASoul* AProjectileActorManager::Pop_Soul(TSubclassOf<ASoul> SoulClass, FTransform Transform, UWorld* pWorld)
{
	AProjectileActorManager* pManager = CheckProjectileActorManagerExists(pWorld);
	check(pManager);

	if (pManager)
	{
		return pManager->PopASoul(SoulClass, std::move(Transform));
	}

	return nullptr;
}

void AProjectileActorManager::Push_Soul(ASoul* pReturned)
{
	if (pReturned)
	{

		AProjectileActorManager* pManager = CheckProjectileActorManagerExists(pReturned->GetWorld());
		check(pManager);

		if (pManager)
		{
			pManager->PushASoul(pReturned);
		}
	}
}

AItemEmitterActor* AProjectileActorManager::Pop_ItemEmitterActor(TSubclassOf<AItemEmitterActor> EmitterClass, UWorld* pWorld)
{
	AProjectileActorManager* pManager = CheckProjectileActorManagerExists(pWorld);
	check(pManager);

	if (pManager)
	{
		return pManager->PopAnItemEmitter(EmitterClass);
	}

	return nullptr;
}

void AProjectileActorManager::Push_ItemEmitterActor(AItemEmitterActor* pReturned)
{
	if (pReturned)
	{

		AProjectileActorManager* pManager = CheckProjectileActorManagerExists(pReturned->GetWorld());
		check(pManager);

		if (pManager)
		{
			pManager->PushAnItemEmitter(pReturned);
		}
	}
}

UParticleSystemComponent* AProjectileActorManager::SpawnHitParticleEffect(UWorld* pWorld, class UParticleSystem* ParticleSystemTemplate, const FVector& Location, const FRotator& Rotation)
{
	AProjectileActorManager* pManager = CheckProjectileActorManagerExists(pWorld);
	check(pManager);

	if (pManager)
	{
		return pManager->PopHitParticleEffect(ParticleSystemTemplate, Location, Rotation);
	}

	return nullptr;
}

AStorableItem* AProjectileActorManager::TryPop_Storable(UWorld* pWorld, const ItemType& itemType) {
	AProjectileActorManager* pManager = CheckProjectileActorManagerExists(pWorld);
	check(pManager);
	if (pManager && pManager->HasStorableInstancesArray(itemType.getId())) {
		return pManager->PopAStorable(itemType);
	}
	return nullptr;
}

bool AProjectileActorManager::TryPush_Storable(class AStorableItem* Returned) {
	if (Returned) {
		AProjectileActorManager* pManager = CheckProjectileActorManagerExists(Returned->GetWorld());
		check(pManager);
		if (pManager && pManager->HasStorableInstancesArray(Returned->GetItemType().getId())) {			
			pManager->PushAStorable(Returned);
			return true;			
		}
	}
	return false;
}

void AProjectileActorManager::SpawnXPOrbs(UWorld* pWorld, AActor* pSource, APlayerCharacter* pTarget, int32 iCount)
{
	if (pSource && pTarget && iCount > 0)
	{
		AProjectileActorManager* pManager = CheckProjectileActorManagerExists(pWorld);
		check(pManager);

		if (pManager)
		{
			pManager->AddXPOrbSpawn(pSource, pTarget, iCount);
		}
	}
}

void AProjectileActorManager::SpawnOxygenBubble(UWorld* pWorld, const FVector& pSpawnLocation, APlayerCharacter* pTarget) {
	AProjectileActorManager* pManager = CheckProjectileActorManagerExists(pWorld);
	check(pManager);

	if (pManager) {
		pManager->AddOxygenBubbleSpawn(pSpawnLocation, pTarget);
	}
}

UParticleSystemComponent* AProjectileActorManager::PopHitParticleEffect(class UParticleSystem* ParticleSystemTemplate, const FVector& Location, const FRotator& Rotation)
{
	//find a particle component thats available
	for (auto ParticlesInstance : mProjectileHitParticleComponents)
	{
		if (!ParticlesInstance->IsComponentTickEnabled())
		{
			//found one
			FTransform spawnTransform(Rotation, Location);
			ParticlesInstance->SetTemplate(ParticleSystemTemplate);
			ParticlesInstance->SetWorldTransform(spawnTransform);

			ParticlesInstance->SetVisibility(true);
			ParticlesInstance->Activate(true);
			ParticlesInstance->SetComponentTickEnabled(true);
			return ParticlesInstance.Get();
		}
	}

	UE_LOG(LogDungeonsProjectile, Log, TEXT("######!!####### Ran out of Projectile Hit Particle components, consider upping the DefaultProjectileHitParticlePoolSize "));
	return nullptr;
}

AStorableItem* AProjectileActorManager::PopAStorable(const ItemType& itemType)
{
	AStorableItem* pOut = nullptr;
	WeakStorableArray& instancesArray = GetStorableInstancesArray(itemType.getId());

	for (TWeakObjectPtr< AStorableItem >& pFreeInst : instancesArray)
	{
		if (pFreeInst.IsValid() && !pFreeInst->IsActorTickEnabled())
		{
			pOut = pFreeInst.Get();
			break;
		}
	}


	if (!pOut)
	{
		if (const auto pStorableClass = itemType.getStorableClass())
		{
			UE_LOG(LogDungeonsProjectile, Log, TEXT("### AProjectileActorManager::PopAStorable : %s\n"), *pStorableClass->GetName());
			auto* StorableInst = Cast<AStorableItem>(SpawnNewActorClassInstance(pStorableClass));
			ensureMsgf(StorableInst, TEXT(" Failed to spawn !"));
			StorableInst->ItemData = FInventoryItemData(itemType.getId());
			instancesArray.Push(StorableInst);
			StorableInst->SetLifeSpan(0.0f);
			pOut = StorableInst;
		}
	}

	if (pOut)
	{
		pOut->SetActorTickEnabled(true);
		pOut->SetActorHiddenInGame(false);
		pOut->EnableMovementComponentTick(true);
		pOut->GetCollisionComponent()->SetSimulatePhysics(true);
		pOut->mCanMove = true;
		pOut->AddDataInstanceTracking();

		if (itemType.getId() == game::item::type::Emerald.getId()) {
			if (auto* gameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode())) {
				gameMode->TrackEmeraldSpawned();
			}
		}

	}

	return pOut;
}

void AProjectileActorManager::PushAStorable(AStorableItem* pReturn)
{
	if (pReturn)
	{
		pReturn->SetOwner(this);
				
		pReturn->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);

		pReturn->SetActorTickEnabled(false);
		pReturn->SetActorHiddenInGame(true);
		pReturn->EnableMovementComponentTick(false);
		pReturn->GetCollisionComponent()->SetSimulatePhysics(false);
		pReturn->RemoveDataInstanceTracking();
	}
	
}

AXPOrb* AProjectileActorManager::PopXPOrb()
{
	for (auto Instance : mXPOrbInstances)
	{
		if (Instance.IsValid() &&!Instance->IsActorTickEnabled())
		{
			//found one			
			Instance->SetActorHiddenInGame(false);
			Instance->SetActorTickEnabled(true);
			return Instance.Get();
		}
	}

	return nullptr;
}

AOxygenBubbleActor* AProjectileActorManager::PopOxygenBubble()
{
	for (auto Instance : mOxygenBubbleInstances)
	{
		if (Instance.IsValid() &&!Instance->IsActorTickEnabled())
		{
			Instance->SetActorHiddenInGame(false);
			Instance->SetActorTickEnabled(true);
			return Instance.Get();
		}
	}

	return nullptr;
}

void AProjectileActorManager::AddXPOrbSpawn(AActor* pSource, APlayerCharacter* pTarget, int32 iCount)
{
	XPOrbSpawn NewOrbs;
	NewOrbs.pSource = pSource;
	NewOrbs.pTarget = pTarget;
	NewOrbs.iCount = iCount;
	mXPOrbSpawns.Push(NewOrbs);
	SetActorTickEnabled(true);
}

void AProjectileActorManager::AddOxygenBubbleSpawn(const FVector& pSource, APlayerCharacter* pTarget)
{
	BubbleSpawnInfo newBubble;
	newBubble.pSource = pSource;
	newBubble.pTarget = pTarget;
	mOxygenBubbleSpawns.Push(newBubble);
	SetActorTickEnabled(true);
}
