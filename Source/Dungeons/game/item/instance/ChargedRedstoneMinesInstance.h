#pragma once

#include "CoreMinimal.h"
#include "AItemInstance.h"
#include "game/actor/item/ChargedRedstoneMineActor.h"
#include "ChargedRedstoneMinesInstance.generated.h"

UCLASS()
class DUNGEONS_API UChargedRedstoneMineDamageGameplayEffect : public UBaseItemDamageGameplayEffect {
	GENERATED_BODY()
public:
	UChargedRedstoneMineDamageGameplayEffect();

};

UCLASS()
class DUNGEONS_API AChargedRedstoneMinesInstance : public AItemInstance
{
	GENERATED_BODY()
public:
	AChargedRedstoneMinesInstance();
	void Activate(const FPredictionKey& predictionKey) override;

	float GetStats(EItemStats stat) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int DamagePerMine = 400;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int MineSpawnAmount = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MineSpawnCone = 130.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MineSpawnDistance = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MineSpawnAngle = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float TeamDamageFactor = 0.1f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AChargedRedstoneMineActor> ChargedRedstoneMineBP;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MineAliveTime = 12.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MineExplosionRadius = 250.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MineKnockbackStrength = 16.0f;
private:
	int SpawnMines();
};