#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/BuffGrantTotem.h"
#include "game/component/AreaBuffComponent.h"
#include "TotemOfSpiritProtection.generated.h"

class USoulComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRevivedPlayer, APlayerCharacter*);

UCLASS()
class DUNGEONS_API UTotemOfSpiritProtectionGameplayEffect : public UAreaBuffGameplayEffect {
	GENERATED_BODY()
public:
	UTotemOfSpiritProtectionGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UTotemOfSpiritProtectionReviveGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UTotemOfSpiritProtectionReviveGameplayEffect();
};

UCLASS()
class DUNGEONS_API AReviveOnOverlapActor : public AActor {
	GENERATED_BODY()
public:
	AReviveOnOverlapActor();

	void BeginPlay() override;
	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnRevive(APlayerCharacter* player);

	FOnRevivedPlayer OnRevivedPlayer;

	TWeakObjectPtr<APlayerCharacter> TargetPlayer;

	float HealthAmount = 0;
private:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Overlap;
	UPROPERTY(VisibleAnywhere)
	class UMoveToTargetMovementComponent* Movement;
};

UCLASS()
class DUNGEONS_API ATotemOfSpiritProtectionActor : public ABuffGrantTotemActor {
	GENERATED_BODY()
public:
	ATotemOfSpiritProtectionActor();

	void BeginPlay() override;

	float HealthAmount = 0;

protected:
	void PreBuffComponentBeginPlay(UAreaBuffComponent* BuffComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AReviveOnOverlapActor> ReviveActorClass;
private:
	UFUNCTION()
	void OnPlayerDown(APlayerCharacter* player);
	UFUNCTION()
	void OnPlayerRevived(APlayerCharacter* revivedPlayer);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSpawnReviveActor(APlayerCharacter* targetPlayer, const FVector& spawnLocation);

	UPROPERTY()
	AReviveOnOverlapActor* ReviveActor;
};

UCLASS()
class DUNGEONS_API ATotemOfSpiritProtectionInstance : public ABuffGrantTotemInstance {
	GENERATED_BODY()
public:
	ATotemOfSpiritProtectionInstance();

	float GetStats(EItemStats stat) const override;
	
protected:
	void PreTotemBeginPlay(ATotemBaseActor* totemActor) override;
private:
	TWeakObjectPtr<USoulComponent> OwnerSoulComponent;
};
