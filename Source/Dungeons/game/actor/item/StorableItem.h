#pragma once

#include "world/level/BlockPos.h"
#include "lovika/LevelCommon.h"
#include "game/component/ItemStashComponent.h"
#include "game/component/drop/ItemDropEffectComponent.h"
#include "game/item/InventoryItemData.h"
#include "game/actor/PropActor.h"
#include "game/actor/item/ThrowablePropActor.h"
#include "StorableItem.generated.h"

class APlayerCharacter;
class ItemType;
class USphereComponent;

UCLASS()
class DUNGEONS_API AStorableItem : public APropActor {
	GENERATED_BODY()
public:
	AStorableItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	USphereComponent* GetCollisionComponent() const;

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	/** Called when the actor falls out of the world 'safely' (below KillZ and such) */
	void FellOutOfWorld(const class UDamageType& dmgType) override;

	/** Called when the Actor is outside the hard limit on world bounds */
	void OutsideWorldBounds() override;

	//D11.KS
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Dungeons")
	bool lockItemToOwner = false;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool Rotate = false;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float RotationSpeed = 45.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float PendelSpan = HALF_PI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundCue* DropSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundCue* PickupSound;

	UFUNCTION(BlueprintPure, Category = "Dungeons")
	static USoundCue* GetStorableDropSoundForItemId(const FSerializableItemId& itemId);

	UFUNCTION(BlueprintPure, Category = "Dungeons")
	static USoundCue* GetStorablePickupSoundForItemId(const FSerializableItemId& itemId);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundCue* PickupDeniedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UParticleSystem* PickupEffect = nullptr;

	UFUNCTION(NetMulticast, Reliable)
	void OnPickup();

	void HandleLockedOwnerDropOut(ULocalPlayer* localPlayer);

	UFUNCTION(NetMulticast, Reliable)
	void OnPickupDenied();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void ReceiveOnPickup();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void ReceiveOnPickupDenied();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void ReInitialiseItemBP();
	virtual void ReInitialiseItem();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastApplyDropEffect(AActor* FromWhat);

	void ApplyDropEffect(AActor* fromWhat, bool doCustomImpulse = false) const;

	void CheckInDestroyingMaterial();

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons", Replicated)
	FInventoryItemData ItemData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	UItemDropEffectComponent* ItemDropEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons", meta = (ClampMin = "-1.0", ClampMax = "10.0"))
	float DisableTimer = 2.0f;
	float DisableTime_Current = 2.0f;

	class USimpleMovementComponent* movementComponent() { return SimpleMovementComponent; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Dungeons")
	class UMapPinComponent* MapPinComponent;

	EItemRarity GetRarity() const;

	bool stored;

	void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
		FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void DestroyOrReturnToPool();

	const ItemType& GetItemType() const;

	UPROPERTY(AssetRegistrySearchable)
	FName ItemIdName;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void HandleInteraction(ACharacter* interactor);

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	EThrowableType ThrowableType = EThrowableType::None;

	void SetInstanceClass(UClass* instanceClass);

	void EnableMovementComponentTick(bool val);

	void SetTimeToPool(float timeToPool);
	UFUNCTION()
	void TimeToPoolFinished();
	UFUNCTION()
	void OnRep_OnTimeToPoolSet();
	UFUNCTION(BlueprintImplementableEvent)
	void OnTimeToPoolSetBP();
protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	bool ShouldPlayDropSound() const;

	void Tick(float DeltaSeconds) override;

	void CheckAttachmentCollision();

	UFUNCTION(BlueprintImplementableEvent)
	void OnLanded();

	void OnPlayerInVisibleRangeChange(bool isVisible) override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	FSerializableItemId ItemId;

	UPROPERTY(VisibleAnywhere)
	class USimpleMovementComponent* SimpleMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Dungeons")
	USphereComponent* Sphere;

	void Serialize(FArchive& Ar) override;

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	bool ReturnToPool();

	virtual void OnLandedInternal();

	void AttachItemToFloor();

	bool HasValuableType() const;
	bool IsInUnsafeLocation() const;
	ABaseCharacter* GetTeleportTarget() const;
	void TeleportToSafeLocation();
	virtual void DisablePhysics();

	void Play(class USoundCue*) const;

	UPROPERTY(BlueprintReadWrite, Category = "Dungeons", ReplicatedUsing = OnRep_OnTimeToPoolSet)
	float TimeToPool = 0.f;



	//GC handle for the instance class that is used when this is an instant use item.
	UPROPERTY(Transient)
	UClass* InstanceClass;

	int32 AliveFrames;

	EMaterialTypeEnum OverlapMaterial;

	BlockPos PreviousOverlap;

	UPROPERTY()
	class AGameBP* Game;

	TOptional<int> OverrideStoreCount;

	UPROPERTY(Transient, Replicated)
	float ServerSpawnTimeSeconds = 0;

	FTraceHandle AttachmentCollisionHandle;
};


USTRUCT(BlueprintType)
struct DUNGEONS_API FStorableItemRow : public FTableRowBase {
	GENERATED_BODY()

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
		TSoftClassPtr<AStorableItem> StorableItemClass;
};
