#pragma once

#include "GameFramework/Actor.h"
#include "game/actor/item/BaseProjectile.h"
#include "game/actor/item/ItemEmitterActor.h"
#include "XPOrb.h"
#include "OxygenBubbleActor.h"
#include "character/player/PlayerCharacter.h"

#include "ProjectileManager.generated.h"

struct ProjectileCachedClassInst {
	ProjectileCachedClassInst(AActor* pInst = nullptr): WeakInstance(pInst), LoanedOutTime(-1.0f){};
	TWeakObjectPtr<AActor> WeakInstance;	//Weak ref to the class inst
	float LoanedOutTime = -1.0f;			//time > 0.0f currently on loan
	
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FProjectileCachedClass {
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, Category = "Dungeons|Projectiles")
	int32 MinimumCachedCount = 16;//minimum number of cached instances

	UPROPERTY(EditInstanceOnly, Category = "Dungeons|Projectiles")
	int32 MaximumCachedCount = -1; //maximum number of cached instances

	enum class EReuseSetting : int8
	{
		E_Unset = -1,
		E_Disabled = 0,
		E_Enabled = 1,
		E_Max
	};

	EReuseSetting ReuseOldestInstance = EReuseSetting::E_Unset; //re-use oldest instance for new spawn if MaximumCachedCount exceeded

	TArray< ProjectileCachedClassInst > InstanceList;
};


UCLASS()
class DUNGEONS_API AProjectileActorManager : public AActor {
	GENERATED_BODY()
public:
	AProjectileActorManager(const FObjectInitializer& ObjectInitializer);

	void						Tick(float deltaSeconds) override;	

	void						BeginPlay() override;
	
	void						EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//ensure this exists or spawn a new one
	static AProjectileActorManager*	CheckProjectileActorManagerExists(UWorld* pWorld);

	//projectile Props
	static ABaseProjectileProp*	Pop_ProjectileProp(TSubclassOf<ABaseProjectileProp> ProjectilePropClass, FTransform Transform, UWorld* pWorld);
	static void					Push_ProjectileProp(ABaseProjectileProp* ReturnedProjectileProp);
	
	//projectiles
	static ABaseProjectile*		Pop_Projectile(TSubclassOf<ABaseProjectile> ProjectileClass, FTransform Transform, UWorld* world);
	static ABaseProjectile*		Pop_Projectile(TSubclassOf<ABaseProjectile> ProjectileClass, FTransform Transform, APawn* pInstigator);
	static void					PreCache_ProjectilePropClass(TSubclassOf<ABaseProjectileProp> ProjectilePropClass, UWorld* world, int32 MinimumCachedCount = 16, int32 MaximumCachedCount = -1);
	static void					PreCache_ProjectileClass(TSubclassOf<ABaseProjectile> ProjectileClass, UWorld* world);
	static void					PreCache_PlayerProjectiles( TArray< TSubclassOf< ABaseProjectile > >& NewProjectiles, APlayerCharacter* pPlayerOwner);
	static void					Push_Projectile(ABaseProjectile* ReturnedProjectile);
	static void					Purge_Projectiles(APawn* pInstigator);

	//Souls
	static ASoul*				Pop_Soul(TSubclassOf<ASoul> SoulClass, FTransform Transform, UWorld* pWorld);
	static void					Push_Soul(ASoul* pReturned);

	//AItemEmitterActor
	static AItemEmitterActor*	Pop_ItemEmitterActor(TSubclassOf<AItemEmitterActor> EmitterClass,  UWorld* pWorld);
	static void					Push_ItemEmitterActor(AItemEmitterActor* pReturned);

	static UParticleSystemComponent* SpawnHitParticleEffect(UWorld* pWorld, class UParticleSystem* ParticleSystemTemplate, const FVector& Location, const FRotator& Rotation);

	//Storable Instances	
	static class AStorableItem*	TryPop_Storable(UWorld* pWorld, const ItemType&);
	static bool					TryPush_Storable(class AStorableItem* Returned);	

	//XP orbs
	static void					SpawnXPOrbs(UWorld* pWorld, AActor* pSource, APlayerCharacter* pTarget, int32 iCount);

	//#D11.CM - Oxygen Bubble
	static void					SpawnOxygenBubble(UWorld* pWorld, const FVector& pSpawnLocation, APlayerCharacter* pTarget);

protected:

	struct XPOrbSpawn
	{
		TWeakObjectPtr < AActor >			pSource;
		TWeakObjectPtr < APlayerCharacter >	pTarget;
		int32								iCount;
	};

	struct BubbleSpawnInfo
	{
		FVector								pSource;
		TWeakObjectPtr < APlayerCharacter >	pTarget;
	};


	AActor*						SpawnNewActorClassInstance(TSubclassOf<AActor> InstClass);

	//projectile Props
	ABaseProjectileProp*		PopAProjectileProp(TSubclassOf<ABaseProjectileProp> ProjectilePropClass, FTransform Transform);
	void						PushAProjectileProp(ABaseProjectileProp* pReturnedProjectileProp);
	void						PreCacheAProjectilePropClass(TSubclassOf<ABaseProjectileProp> ProjectilePropClass, int32 MinimumCachedCount = 16, int32 MaximumCachedCount = -1);

	//projectiles
	ABaseProjectile*			PopAProjectile(TSubclassOf<ABaseProjectile> ProjectileClass, FTransform Transform, APawn* pInstigator);	
	void						PushAProjectile(ABaseProjectile* pReturnedProjectile);
	void						PreCacheAProjectileClass(TSubclassOf<ABaseProjectile> ProjectileClass);
	void						PreCachePlayerProjectileClasses( TArray< TSubclassOf< ABaseProjectile > >& NewProjectiles, APlayerCharacter* pPlayerOwner);
	//Souls
	ASoul*						PopASoul(TSubclassOf<ASoul> SoulClass, FTransform Transform);
	void						PushASoul(ASoul* pReturned);

	//ItemEmitters
	AItemEmitterActor*			PopAnItemEmitter(TSubclassOf<AItemEmitterActor> ItemEmitterActorClass);
	void						PushAnItemEmitter(AItemEmitterActor* pReturned);

	//Pop
	template <class T>
	T*							PopAClassInst(TSubclassOf<T> ActorClass, FTransform Transform, TMap< TSubclassOf< T >, FProjectileCachedClass >& TargetStore);
	
	template <class T>
	void						PushAClassInst(T* pReturned, TMap< TSubclassOf< T >, FProjectileCachedClass >& TargetStore);

	template <class T>
	void						ReserveClassInstances(TMap< TSubclassOf< T >, FProjectileCachedClass >& TargetList);

	template <class T>
	bool						SpawnMissingCachedClassInstances(TMap< TSubclassOf< T >, FProjectileCachedClass >& TargetStore, std::function<void(T*)>PostSpawn = [](T*) {});
	
	bool						SpawnMissingParticleHitsCache();
	bool						SpawnMissingXPOrbsCache();
	bool						SpawnMissingOxygenBubblesCache();
	bool						SpawnMissingStorableCache(const ItemType& itemType, int32 cacheSize);

	void						PurgeAllPlayerProjectiles(APawn* pInstigator);
	void						PurgeProjectileCache(TSubclassOf<ABaseProjectile> TargetClass, FProjectileCachedClass& TargetStore);
	ABaseProjectile*			PopAPurgedPlayerProjectile(TSubclassOf<ABaseProjectile> ActorClass, TMap< TSubclassOf< ABaseProjectile >, FProjectileCachedClass >& TargetStore);
	bool						PurgedPlayerProjectileClean();

	//hiteffects
	UParticleSystemComponent*	PopHitParticleEffect(class UParticleSystem* ParticleSystemTemplate, const FVector& Location, const FRotator& Rotation);

	//Emeralds
	class AStorableItem*		PopAStorable(const ItemType&);
	void						PushAStorable(class AStorableItem* pReturn);

	//XPOrbs
	AXPOrb*						PopXPOrb();

	void						AddXPOrbSpawn(AActor* pSource, APlayerCharacter* pTarget, int32 iCount);

	//Oxygen Bubbles
	AOxygenBubbleActor*			PopOxygenBubble();
	void						AddOxygenBubbleSpawn(const FVector& pSource, APlayerCharacter* pTarget);

	void						StartInitialisation();

	UFUNCTION()
	virtual void				OnHitParticleSystemFinished(class UParticleSystemComponent* FinishedComponent);

	UFUNCTION()
	virtual void				OnPoolItemDestroyed(AActor* DestroyedActor);
	
	/*stored projectile instances, stored in lists, mapped by class type*/
	UPROPERTY(EditInstanceOnly, Category = "Dungeons|Projectiles")
	TMap< TSubclassOf<ABaseProjectile>, FProjectileCachedClass > MobProjectileInstances;

	/*stored player projectile instances, stored in lists, mapped by class type and player*/
	TMap < APlayerCharacter*, TMap< TSubclassOf<ABaseProjectile>, FProjectileCachedClass >  > PlayerProjectileInstances;

	/*stored player projectile instances that have been purged, removed one per frame, or re-used if required*/
	TMap< TSubclassOf<ABaseProjectile>, TArray< ProjectileCachedClassInst > > PlayerProjectilePurgedInstances;

	/*stored projectile prop instances, stored in lists, mapped by class type*/
	UPROPERTY(EditInstanceOnly, Category = "Dungeons|Projectiles")
	TMap< TSubclassOf<ABaseProjectileProp>, FProjectileCachedClass > ProjectilePropInstances;

	/*stored Soul instances, stored in lists, mapped by class type*/
	UPROPERTY(EditInstanceOnly, Category = "Dungeons|Soul")
	TMap< TSubclassOf<ASoul>, FProjectileCachedClass > SoulInstances;

	/*stored AItemEmitterActor instances, stored in lists, mapped by class type*/
	UPROPERTY(EditInstanceOnly, Category = "Dungeons|ItemEmitter")
	TMap< TSubclassOf<AItemEmitterActor>, FProjectileCachedClass > ItemEmitterInstances;

	
	UPROPERTY()
	UParticleSystem* mBaseHitParticleSystemTemplate;

	UPROPERTY(EditInstanceOnly, Category = "Dungeons|Projectiles")
	int32 DefaultProjectileHitParticlePoolSize = 64;

	UPROPERTY(EditInstanceOnly, Category = "Dungeons|Projectiles")
	int32 DefaultEmeraldPoolSize = 128;

	UPROPERTY(EditInstanceOnly, Category = "Dungeons|Projectiles")
	int32 DefaultGoldPoolSize = 16;

	UPROPERTY(EditInstanceOnly, Category = "Dungeons|Projectiles")
	int32 DefaultXPOrbPoolSize = 32;

	UPROPERTY(EditInstanceOnly, Category = "Dungeons|Projectiles")
	int32 DefaultOxygenBubblePoolSize = 16;

	
	TArray< TWeakObjectPtr<UParticleSystemComponent> > mProjectileHitParticleComponents;

	using WeakStorableArray = TArray<TWeakObjectPtr<class AStorableItem>>;
	TMap<FItemId, WeakStorableArray> mStorableInstancesMap;

	TArray< TWeakObjectPtr< AXPOrb > > mXPOrbInstances;

	TArray< XPOrbSpawn > mXPOrbSpawns;

	TArray< BubbleSpawnInfo > mOxygenBubbleSpawns;

	TArray< TWeakObjectPtr < AOxygenBubbleActor > > mOxygenBubbleInstances;

	bool HasStorableInstancesArray(const FItemId&) const;
	WeakStorableArray& GetStorableInstancesArray(const FItemId&);
	void EnableStorableInstancesCachingFor(const ItemType&);
};

