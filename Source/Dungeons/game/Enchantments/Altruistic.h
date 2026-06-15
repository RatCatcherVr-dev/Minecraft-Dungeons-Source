#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Altruistic.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHealedActor, AActor*, float);

UCLASS()
class DUNGEONS_API AHealOnOverlapActor : public AActor {
	GENERATED_BODY()
public:
	AHealOnOverlapActor();

	TWeakObjectPtr<UHealthComponent> TargetHealthComponent;
	float healAmount;

	float magnitude = 1.0f;

	void BeginPlay() override;
	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnHeal(UHealthComponent* target);

	FOnHealedActor OnHealedActor;
private:
	UFUNCTION()
	void OnTargetDied(AActor* OtherActor);

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Overlap;
	UPROPERTY(VisibleAnywhere)
	class UMoveToTargetMovementComponent* Movement;
};

UCLASS()
class DUNGEONS_API UAltruistic : public UEnchantment
{
	GENERATED_BODY()
public:

	UAltruistic();

	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnSoul(AActor* owner, const FVector& spawnLocation, float healAmount, float magnitude);
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float PercentageOfDamageToHeal = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float AffectionRadius = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AHealOnOverlapActor> ActorClass;
};