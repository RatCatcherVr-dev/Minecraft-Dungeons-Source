#pragma once

#include "game/abilities/cues/DungeonsGameplayCueNotify_Actor.h"
#include "Dungeons.h"
#include "game/component/BeamAttackComponent.h"
#include "BaseGuardianBeamNotify.generated.h"

UCLASS()
class DUNGEONS_API ABaseGuardianBeamNotify : public ADungeonsGameplayCueNotify_Actor {
	GENERATED_BODY()

	ABaseGuardianBeamNotify();

public:
	void Tick(const float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	bool Piercing = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float BeamRange = 10000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	USceneComponent* RootSceneComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	UParticleSystemComponent* BeamVFX = nullptr;
};