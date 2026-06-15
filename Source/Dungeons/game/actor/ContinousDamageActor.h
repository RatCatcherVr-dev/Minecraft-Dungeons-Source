#pragma once
#include "CoreMinimal.h"
#include <GameplayEffectTypes.h>
#include "ContinousDamageActor.generated.h"

class ABaseCharacter;

UCLASS(BlueprintType)
class DUNGEONS_API AContinousDamageActor : public AActor {
	GENERATED_BODY()
public:
	AContinousDamageActor();

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetGameplayEffectSpec(const FGameplayEffectSpecHandle& spec);

	UFUNCTION(BlueprintImplementableEvent)
	void OnDamageDisabled();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dungeons")
	float TotalLifeSpan = 7.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Dungeons")
	float DamageDuration = 5.0f;

	class UContinousDamageComponent* GetDamageComponent() const { return DamageComponent; }

protected:
	UPROPERTY(VisibleDefaultsOnly)
	class UContinousDamageComponent* DamageComponent = nullptr;

private:
	FTimerHandle DamageExpireHandle;

	void OnDisableDamage();
};

