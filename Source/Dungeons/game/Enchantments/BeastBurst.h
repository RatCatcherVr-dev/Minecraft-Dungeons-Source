#pragma once

#include "CoreMinimal.h"
#include "Synergy.h"
#include "BeastBurst.generated.h"

UCLASS()
class DUNGEONS_API UBeastBurstExplosionGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBeastBurstExplosionGameplayEffect();
};

UCLASS()
class DUNGEONS_API UBeastBurst : public UHealthPotionSynergy {
	GENERATED_BODY()
	
public:
	UBeastBurst();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UBeastBurstExplosionGameplayEffect> Effect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UParticleSystem* ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class USoundCue* ExplosionSound;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float ExplosionRadius = 700.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BaseDamageValue = 250.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float ExtraDamagePerLevel = 250.0f;
	
protected:
	void OnHealthPotionUsed() override;
private:
	void StartExplosion(ABaseCharacter* minion);
};
