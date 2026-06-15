#pragma once

#include "BaseProjectile.h"
#include "game/item/SerializableItemId.h"
#include "Arrow.generated.h"

UCLASS()
class DUNGEONS_API AArrow : public ABaseProjectile {
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	FSerializableItemId ConsumesStorableItemId;


	virtual TOptional<FItemId> GetProjectileItemType() const override {
		return static_cast<FItemId>(ConsumesStorableItemId);
	}

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	AArrow(const FObjectInitializer& ObjectInitializer);

	USphereComponent* SphereComponent;

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Dungeons|Projectiles")
	bool bShouldSpawnArrowProp = true;
	bool bShouldSpawnArrowPropDefault = true;

	virtual void ResetProjectile() override;

	void ToggleMagical_Implementation(bool magical) override;


	UFUNCTION(BlueprintCallable, Category = "GameplayEffect")
	void UpdateProjectileComponent(AActor* pOwner);


	void LaunchProjectile(AActor* pOwner) override;
};


UCLASS()
class DUNGEONS_API AArrowProp : public ABaseProjectileProp {
	GENERATED_BODY()

		
public:
	AArrowProp(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Dungeons")
	USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dungeons")
	UStaticMeshComponent* ArrowMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	class UArrowVobbleComponent* ArrowVobbleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	class USoundCue* UnChargedSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	class USoundCue* ChargedSound;

	UPROPERTY()
	class UAudioComponent* HitSoundComponent = nullptr;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dungeons")
	bool EnableVobble = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dungeons")
	bool Charged = false;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Initialise(bool bCharged, float fPropLifetime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnInitializedProp(bool bCharged, float fPropLifetime);

	virtual void InitialiseProp(bool bCharged, float fPropLifetime) override;

	virtual void BeginPlay() override;
	
protected:
	
	class UAudioComponent* CreateHitAudioComponent();

};