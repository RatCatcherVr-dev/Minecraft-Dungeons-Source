#pragma once

#include "CoreMinimal.h"
#include "BaseParticleAssetsComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent), DisplayName = "BaseParticleAssetsComponent")
class DUNGEONS_API UBaseParticleAssetsComponent : public UActorComponent {
	GENERATED_BODY()

	UBaseParticleAssetsComponent();
public:
	UPROPERTY(EditAnywhere)
	class UParticleSystem* Summon;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* PreSummon;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ShieldBreak;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* TeleportOut;
};
