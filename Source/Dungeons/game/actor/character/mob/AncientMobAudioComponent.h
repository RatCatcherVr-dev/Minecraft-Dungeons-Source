#pragma once

#include "CoreMinimal.h"
#include "DungeonsGameInstance.h"
#include "Components/ActorComponent.h"

#include "AncientMobAudioComponent.generated.h"

/**
 * Generates drops based on its predefined settings.
 */
UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API UAncientMobAudioComponent : public USceneComponent {
	GENERATED_BODY()
public:

	UAncientMobAudioComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	class UAudioComponent* AncientAmbienceSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	class UAudioComponent* AncientMobDeath;

	virtual void BeginPlay() override;

	void OnMobDeath();
	
protected:
	
};
