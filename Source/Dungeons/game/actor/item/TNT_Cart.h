#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "game/util/ActorQuery.h"
#include "GameplayEffect.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "game/abilities/effects/WorldDamageGameplayEffect.h"
#include "TNT_Cart.generated.h"

class ABasePlayerController;
class APlayerCharacter;
struct FPushback;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UTNT_Cart : public UActorComponent
{
	GENERATED_BODY()
public:
	UTNT_Cart();

protected:
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Dungeons")
	int32 ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Dungeons")
	int32 ExplosionDamage;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Explode(const ABaseCharacter* BaseCharacter);

private:
	void ExecuteExplosionGameplayCue(const AActor* owner, const ABaseCharacter* BaseCharacter);

	void DealDamageWithinRadius(const int radius, AActor* owner);

	void DealDamageToMob(const AMobCharacter* mob, const FVector& tntLocation, AActor* owner, const float scaledDamage);

	void DealDamageToPlayer(const APlayerCharacter* player, AActor* owner, const float scaledDamage);

	FPushback CreatePushback(const AActor* entity, const FVector& tntLocation) const;

	static float CalculateLaunchMagnitude(const AActor* entity, const FVector& tntLocation);
};
