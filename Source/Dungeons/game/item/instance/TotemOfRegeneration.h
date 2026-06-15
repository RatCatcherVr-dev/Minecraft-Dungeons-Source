#pragma once

#include "CoreMinimal.h"
#include "game/component/AreaBuffComponent.h"
#include "game/item/instance/BuffGrantTotem.h"
#include "TotemOfRegeneration.generated.h"

UCLASS()
class DUNGEONS_API UTotemOfRegenerationGameplayEffect : public UAreaBuffGameplayEffect {
	GENERATED_BODY()
public:
	UTotemOfRegenerationGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API ATotemOfRegenerationActor : public ABuffGrantTotemActor {
	GENERATED_BODY()
public:
	ATotemOfRegenerationActor();

	float TotalHealthHealedPerSecond = 0;

protected:
	void PreBuffComponentBeginPlay(UAreaBuffComponent* BuffComponent) override;
};

UCLASS()
class DUNGEONS_API ATotemOfRegenerationInstance : public ABuffGrantTotemInstance
{
	GENERATED_BODY()

	ATotemOfRegenerationInstance();

	float GetStats(EItemStats stat) const override;
protected:
	void PreTotemBeginPlay(ATotemBaseActor* totemActor) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float HealthHealedPerSecond = 40.0f;
};
