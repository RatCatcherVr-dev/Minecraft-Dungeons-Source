#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "GameplayEffect.h"
#include "ReviveComponent.generated.h"

UCLASS()
class DUNGEONS_API UReviveFriendGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UReviveFriendGameplayEffect();
};

/**
 * Allows owning player to revive other downed players.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UReviveComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UReviveComponent();	

	/**
	 * Get percentage of revive time passed.
	 */
	UFUNCTION(BlueprintCallable)
	float GetProgress() const;

	/** Will not be valid on the server unless called on locally controller player */
	bool IsCurrentlyRevivingLocal() const;

	bool IsTargetingPlayer(const APlayerCharacter* player) const;

	void Revive(APlayerCharacter* target);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCancelRevive();

	void EndPlay(const EEndPlayReason::Type) override;

private:	
	UFUNCTION(Client, Reliable)
	void ClientEndRevive();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartRevive(APlayerCharacter* target);

	void ExecuteRevive(TWeakObjectPtr<APlayerCharacter> target);


	void OnAnyPlayerAction(const ABasePlayerController*);
protected:
	void BeginPlay() override;	

	UPROPERTY(EditDefaultsOnly)
	UAnimSequenceBase* RevivingAnimation;

	UPROPERTY(EditDefaultsOnly)
	UAnimSequenceBase* ReviveSuccessAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Audio)
	class USoundCue* ReviveSoundCue;
private:
	ABasePlayerController* GetRevivingPlayerController() const;
	UHealthComponent* GetRevivingPlayerHealthComponent() const;

	FTimerHandle ReviveTimerHandle;

	APlayerCharacter* RevivingPlayer;
	APlayerCharacter* TargetedPlayer = nullptr;
	
	float StartTimeStamp = 0.f;

	/**
	 * Time to complete a revive in seconds.
	 */
	float mReviveTime;

	bool bCancelReviveOnDamageReceived = false;
};
