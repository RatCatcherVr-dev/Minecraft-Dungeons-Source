#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/Actor.h"
#include "ActorShakeManager.generated.h"

class UActorShake;

USTRUCT()
struct FPooledActorShakes
{
	GENERATED_USTRUCT_BODY()
		
	UPROPERTY()
	TArray<class UActorShake*> PooledShakes;
};

UCLASS()
class DUNGEONS_API AActorShakeManager : public AActor
{
	GENERATED_BODY()

public:
	AActorShakeManager();
	/** List of active ActorShake instances */
	UPROPERTY()
	TArray<class UActorShake*> ActiveShakes;

	UPROPERTY()
	TMap<TSubclassOf<class UActorShake>, FPooledActorShakes> ExpiredPooledShakesMap;

	void SaveShakeInExpiredPool(class UActorShake* ShakeInst);
	UActorShake* ReclaimShakeFromExpiredPool(TSubclassOf<class UActorShake> CameraShakeClass);

	/** 
	 * Adds a new active actor shake to be applied. 
	 * @param ActorToShake - The actor to shake.
	 * @param NewShake - The class of camera shake to instantiate.
	 * @param PlaySpace - Which coordinate system to play the shake in.
	 * @param UserPlaySpaceRot - Coordinate system to play shake when PlaySpace == CAPS_UserDefined.
	 */
	virtual class UActorShake* AddActorShake(AActor* ActorToShake, TSubclassOf<class UActorShake> NewShake, float Scale, FRotator UserPlaySpaceRot = FRotator::ZeroRotator);
	
	/**
	 * Stops and removes the shake of the given class.
	 * @param Shake - the actor shake class to remove.
	 * @param bImmediately		If true, shake stops right away regardless of blend out settings. If false, shake may blend out according to its settings.
	 */
	virtual void RemoveActorShake(UActorShake* ShakeInst, bool bImmediately = true);

	/**
	 * Stops and removes all actor shakes of the given class. 
	 * @param bImmediately		If true, shake stops right away regardless of blend out settings. If false, shake may blend out according to its settings.
	 */
	virtual void RemoveAllActorShakesOfClass(TSubclassOf<class UActorShake> ShakeClass, bool bImmediately = true);

	/** 
	 * Stops and removes all actor shakes. 
	 * @param bImmediately		If true, shake stops right away regardless of blend out settings. If false, shake may blend out according to its settings.
	 */
	virtual void RemoveAllActorShakes(bool bImmediately = true);

	void Tick(float DeltaSeconds) override;

};
