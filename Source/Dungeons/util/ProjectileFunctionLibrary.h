#pragma once
#include <Kismet/BlueprintFunctionLibrary.h>
#include "ProjectileFunctionLibrary.generated.h"

class ABaseProjectileProp;
class AActor;

UCLASS()
class DUNGEONS_API UProjectileFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Dungeons|Projectiles")
	static ABaseProjectileProp* SpawnAttachedArrowProp(AActor* fromActor, AActor* attachTarget, TSubclassOf<ABaseProjectileProp> ArrowPropClass, FVector WorldCollisionPosition = FVector::ZeroVector);

	UFUNCTION(BlueprintPure, Category = "Dungeons|Projectiles", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static ABaseProjectileProp* SpawnProjectileProp(const UObject* WorldContextObject, TSubclassOf<ABaseProjectileProp> ArrowPropClass, const FTransform& SpawnTransform);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Projectiles", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void PreCacheProjectileProp(const UObject* WorldContextObject, TSubclassOf<ABaseProjectileProp> PropClass, int32 MinimumCachedCount = 16, int32 MaximumCachedCount = -1);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Projectiles")
	static void DeSpawnAttachedArrowProp(ABaseProjectileProp* ArrowPropActor);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Projectiles", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UParticleSystemComponent* SpawnHitParticleEffect(const UObject* WorldContextObject, class UParticleSystem* ParticleSystemTemplate, const FVector& Location, const FRotator& Rotation);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Projectiles", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void SpawnXPOrbs(const UObject* WorldContextObject, AActor* pSource, class APlayerCharacter* pTarget, int32 iCount);	

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Projectiles", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void SpawnOxygenBubble(const UObject* WorldContextObject, const FVector& pSource, class APlayerCharacter* pTarget);
	
	static TArray<FName> AttachableBones;

private:
	static FRandomStream DefaultRandom;
};