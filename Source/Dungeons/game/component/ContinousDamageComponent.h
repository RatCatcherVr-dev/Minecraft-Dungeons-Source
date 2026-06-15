#pragma once
#include <CoreMinimal.h>
#include <GameplayEffectTypes.h>
#include <GameplayEffect.h>
#include "ContinousDamageComponent.generated.h"

class ABaseCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UContinousDamageComponent : public UActorComponent {
	GENERATED_BODY()
public:
public:
	UContinousDamageComponent();

	void SetDamagePerSecond(float dps);
	void SetInstigator(APawn* instigator);
	void SetGameplayEffectSpec(const FGameplayEffectSpecHandle& spec);
	void SetEffectClass(TSubclassOf<class UGameplayEffect> effectClass);

	void BeginPlay() override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DisableDamage();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeons")
	float DamagePerSecond = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeons")
	TSubclassOf<class UGameplayEffect> EffectClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeons")
	bool bRemoveEffectEndOverlap = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeons")
	float EffectDuration = 5.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dungeons")
	bool bContinousApplication = false;
private:
	FTimerHandle DamageExpireHandle;

	TOptional<FGameplayEffectSpecHandle> PredefinedSpec;

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	FActiveGameplayEffectHandle ApplyDamageEffect(ABaseCharacter* character) const;
	bool RemoveDamageEffect(ABaseCharacter* character, FActiveGameplayEffectHandle handle) const;
	void RemoveAllDamagedCharacters();

	bool CanDamage(ABaseCharacter* character) const;

	void DisableOverlaps();

	ABaseCharacter* GetInstigator() const;

	UPROPERTY(Transient)
	TMap<ABaseCharacter*, FActiveGameplayEffectHandle> CharactersInCollision;

	UPROPERTY(Transient)
	APawn* Instigator = nullptr;
};

