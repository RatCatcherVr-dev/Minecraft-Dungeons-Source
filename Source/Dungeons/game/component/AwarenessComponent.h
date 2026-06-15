#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AwarenessComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAwareBegin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAwareEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExpand);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnContract);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggered);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UAwarenessComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAwarenessComponent();

	void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetFraction() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsTriggered() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsAware() const;

	void ForceTrigger();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool debugView = false;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float expandRate = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float contractRate = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float maxRange = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float maxScale = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float awareRange = 1000.f;
	
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnAwareBegin OnAwareBegin;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnAwareEnd OnAwareEnd;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnExpand OnExpand;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnContract OnContract;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnTriggered OnTriggered;

	AActor* child = nullptr;

	float fraction = 0.f;
	bool isAware = false;
	bool isTriggered = false;
	float lastTime = 0.f;
};
