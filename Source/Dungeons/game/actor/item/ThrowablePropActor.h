#pragma once

#include "CoreMinimal.h"
#include "game/actor/PropActor.h"
#include "repgraph/Actor_RepSpatializeStatic.h"
#include "game/util/Pushback.h"
#include "AbilitySystemComponent.h"
#include "game/item/ItemType.h"
#include "ThrowablePropActor.generated.h"

class AMobCharacter;
class ABasePlayerController;
class APlayerCharacter;
class UAbilitySystemComponent;

UENUM(BlueprintType)
enum class EThrowableType : uint8 {
	None,
	TNT,
	Trident,
	Conduit
};

UCLASS()
class DUNGEONS_API AThrowablePropActor : public APropActor_RepSpatializeStatic
{
	GENERATED_BODY()

public:
	AThrowablePropActor(const class FObjectInitializer& OI);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	virtual void ThrowInMouseDir() {};

	UFUNCTION(BlueprintCallable)
	virtual void ThrowInPlayerForwardDir();

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ThrowServer(const FVector& dir);

	void Throw(const FVector& dir);

	UFUNCTION()
	virtual	void OnRep_IsActivated();

	UFUNCTION(BlueprintImplementableEvent)
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent)
	void OnEffectExecuted();

	virtual void UpdateParentDead();

	virtual void Activate(const FVector& dir);


	void BeginPlay() override;

	void Consume();

	EThrowableType ThrowType = EThrowableType::None;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<class UActorShake> DenialShake;

protected:
	void Tick(float DeltaSeconds) override;

	virtual FGameplayEffectSpec GetGameplayEffectSpec(UAbilitySystemComponent* AbilitySystemComponent, AActor* owner, const float scaledDamage)
	{
		return FGameplayEffectSpec();
	};

	UFUNCTION()
	void OnOwnerDestroyed(AActor* destroyedActor = nullptr);
	void Fallen();
	virtual void OnFallen();

	FVector GetMouseClickedWorldPosition(ABasePlayerController* controller) const;
	bool IsFirstInStack() const;
	virtual uint16 DealDamageWithinRadius(const int radius, AActor* owner);
	void Explode();
	void Emplace();
	virtual void ExecuteEffectGameplayCue(const AActor* owner);
	FGameplayCueParameters GetEffectGameplayCue(const AActor* owner);

	void DealDamageToMob(const AMobCharacter* mob, const FVector& ItemLocation, AActor* owner, const float scaledDamage);
	FPushback CreatePushback(const AActor* entity, const FVector& ItemLocation) const;
	static float CalculateLaunchMagnitude(const AActor* entity, const FVector& ItemLocation);

	virtual void DealDamageToPlayer(const APlayerCharacter* player, AActor* owner, const float scaledDamage);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Dungeons")
	int32 EffectDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Dungeons")
	float EffectRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Dungeons")
	float EffectTime;

	UPROPERTY(Transient, Replicated)
	FTimerHandle EffectTimerHandler;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Dungeons")
	float BasePushbackMagnitude = 15.0f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsActivated, Category = "Dungeons")
	bool IsActivated = false;

	bool PendingActivatedUpdate = false;

	class URangedAttackComponent* OwnerRangedAttackComponent = nullptr;

	FGameplayTag EffectTag;

	ESlotType SlotType = ESlotType::None;

	UPROPERTY()
	class UArrowItemSlot* Slot = nullptr;

	void RemoveThrowable();

	bool AffectedByGravity = true;
};
