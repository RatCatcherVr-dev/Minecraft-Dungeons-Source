#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include <GameplayEffect.h>
#include "AreaBuffComponent.generated.h"

USTRUCT()
struct DUNGEONS_API FOverlappingCharacterDelegateHandles {
	GENERATED_BODY()

	FOverlappingCharacterDelegateHandles() = default;

	void Unbind(ABaseCharacter* character) const;
	static void Unbind(TMulticastDelegate<void> multicastDelegate, TOptional<FDelegateHandle> handle);

	TOptional<FDelegateHandle> OnDeathHandle;
	TOptional<FDelegateHandle> OnReviveHandle;
	TOptional<FDelegateHandle> OnTeamChangedHandle;
	TOptional<FDelegateHandle> OnAliveStateChangedHandle;
	TOptional<FDelegateHandle> OnGameplayEffectRemovedHandle;
};

UCLASS()
class DUNGEONS_API UAreaBuffGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UAreaBuffGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UAreaBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAreaBuffComponent();

protected:
	void BeginPlay() override;

	void EndPlay(EEndPlayReason::Type reason) override;
public:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UAreaBuffGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TMap<FGameplayAttribute, float> Attributes;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float AreaSize = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool bBuffOwner = true;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool bBuffFriendlyMobs = true;

	TWeakObjectPtr<ABaseCharacter> EffectApplicationOwner;

	TArray<TWeakObjectPtr<ABaseCharacter>> GetOverlappingCharacters();
private:
	void OnGameplayEffectRemoved(const FActiveGameplayEffect& activeEffect, ABaseCharacter* target);

	FOverlappingCharacterDelegateHandles AddListenerForOverlappingCharacter(ABaseCharacter* character);
	void UnbindListenersForOverlappingCharacters();
	void UnbindListenersForOverlappingCharacter(ABaseCharacter* character);

	UFUNCTION()
	void OnSphereOverlapBegin(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);	

	bool CharacterEffectPersistsThroughDeath(const ABaseCharacter* otherCharacter) const;

	bool IsValidTargetCharacter(const ABaseCharacter* otherCharacter) const;

	UFUNCTION()
	void TryUpdateBuffedCharacter(ABaseCharacter* character);

	UFUNCTION()
	void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void TryAddBuffedCharacter(ABaseCharacter* character);
	FGameplayEffectSpec CreateBuffSpec(TSubclassOf<UGameplayEffect> effect) const;

	void TryRemoveBuffedCharacter(ABaseCharacter* character);

	ABaseCharacter* GetOwnerCharacter() const;

	UAbilitySystemComponent* OwnerAbilitySystem;

	USphereComponent* Sphere;

	TMap<ABaseCharacter*, FOverlappingCharacterDelegateHandles> OverlappingCharacterHandles;
};
