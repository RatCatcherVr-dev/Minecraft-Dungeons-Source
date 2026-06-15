#pragma once

#include "game/component/AttackComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "game/Enchantments/EnchantmentType.h"
#include "BeaconAttackComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeaconAttackStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeaconAttackEnded);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UBeaconAttackComponent : public UAttackComponent
{
	GENERATED_BODY()
	
public:
	UBeaconAttackComponent();

	void BeginPlay() override;

	void TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) override;
		
	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void MulticastStart();

	UFUNCTION(BlueprintCallable)
	void Start();	

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FBeaconAttackStarted BeaconAttackStarted;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FBeaconAttackEnded BeaconAttackEnded;
		
private:
	TArray<UChildActorComponent*> beamChildren;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> beamClass;

	UPROPERTY(EditDefaultsOnly)
	float delay = .5f;

	UPROPERTY(EditDefaultsOnly)
	float duration = 5.f;

	UPROPERTY(EditDefaultsOnly)
	float sweepAngleSpan = 90.f;

	float startTime;

	static float SweepOnce(float fraction);

	static float SweepTwice(float fraction);
};
