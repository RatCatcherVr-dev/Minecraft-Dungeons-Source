#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/TotemBaseItemInstance.h"
#include "BuffGrantTotem.generated.h"

class UAreaBuffComponent;

UCLASS()
class DUNGEONS_API ABuffGrantTotemActor : public ATotemBaseActor {
	GENERATED_BODY()
public:
	ABuffGrantTotemActor();

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void TryStartDestroyCountdown() override;

	UPROPERTY(Replicated)
	int Duration = -1;
protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	float GetHealthFraction() override;

	virtual void PreBuffComponentBeginPlay(UAreaBuffComponent* BuffComponent) {}

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<class UAreaBuffGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Multiplier = 0.66f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Radius = 800.0f;

	TWeakObjectPtr<UAreaBuffComponent> OwnerAreaBuffComponent;
private:
	FTimerHandle DestructionTimerHandle;
};

UCLASS()
class DUNGEONS_API ABuffGrantTotemInstance : public ATotemBaseItemInstance {
	GENERATED_BODY()
};
