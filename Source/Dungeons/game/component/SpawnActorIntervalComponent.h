#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "SpawnActorIntervalComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorSpawnedDefered, AActor*, actor);

USTRUCT(BlueprintType)
struct FSpawnElement {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	bool bDistanceBased = true;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "!bDistanceBased"))
	float MinSpawnDelay;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "!bDistanceBased"))
	float MaxSpawnDelay;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bDistanceBased"))
	float MinSpawnDistance;
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = "bDistanceBased"))
	float MaxSpawnDistance;

	UPROPERTY(EditDefaultsOnly)
	float SpawnOffsetMin = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float SpawnOffsetMax = 200.f;

	UPROPERTY(EditDefaultsOnly)
	bool bRandomizeRotation = true;

	UPROPERTY(EditDefaultsOnly)
	bool bSpawnOnGround = true;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> Classes;
};

USTRUCT(BlueprintType)
struct FSpawnQueue {
	GENERATED_BODY()
public:
	void Reset();
	FSpawnElement& Current();
	FSpawnElement& Next();
	bool IsOnLast() const;

	UPROPERTY(EditDefaultsOnly)
	TArray<FSpawnElement> Elements;

	int CurrentIndex = 0;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API USpawnActorIntervalComponent : public UActorComponent {
	GENERATED_BODY()
public:
	USpawnActorIntervalComponent();

	void Activate(bool reset) override;
	void Deactivate() override;

	void TickComponent(float deltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetLoopQueue(bool loop);
	void SetSpawnQueue(FSpawnQueue queue);
	UFUNCTION(BlueprintCallable)
	void SetInstigator(class ABaseCharacter* instigator);

	UPROPERTY(EditDefaultsOnly)
	bool bLoopQueue = true;
	UPROPERTY(EditDefaultsOnly)
	FSpawnQueue SpawnQueue;

	UPROPERTY(BlueprintAssignable)
	FOnActorSpawnedDefered OnActorSpawnedDefered;
private:
	void OnSpawnTimed(FVector callLocation, float callTimeStamp, float duration);

	void Spawn(const FSpawnElement& elem, const FVector& location);

	ABaseCharacter* Instigator;
	FTimerHandle TimerHandle;
	TOptional<float> currentSpawnDistance;
	FVector LastOwnerLocation;
};