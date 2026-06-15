#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerExperienceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAffectXpSignature, FVector, Location, int32, Amount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUpSignature, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLevelUpWithPlayerSignature, int32, NewLevel, int32, PlayerID, APlayerCharacter*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDisplayXpChangedSignature);
DECLARE_MULTICAST_DELEGATE(FOnLevelChangedInternal);

class UDungeonsGameInstance;

namespace save {
	struct Character;
}

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UPlayerExperienceComponent : public UActorComponent
{

	GENERATED_BODY()

public:
	UPlayerExperienceComponent();

	void PreReplication(IRepChangedPropertyTracker & ChangedPropertyTracker) override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
	FOnAffectXpSignature OnAffectXp;

	UPROPERTY(BlueprintAssignable)
	FOnLevelUpSignature OnLevelUp;

	UPROPERTY(BlueprintAssignable)
	FOnLevelUpWithPlayerSignature OnLevelUpWithPlayer;

	UPROPERTY(BlueprintAssignable)
	FOnDisplayXpChangedSignature OnDisplayXpChanged;

	FOnLevelChangedInternal OnLevelChangedInternal;

	/** Character current xp */
	UFUNCTION(BlueprintCallable)
	int32 CurrentXp() const;

	/** Character current level */
	UFUNCTION(BlueprintCallable)
	int32 CurrentLevel() const;

	/** Progression on current xp-level [0, 1) */
	UFUNCTION(BlueprintCallable)
	float LevelProgress() const;
	
	void AddXP_OnlyFromServer(FVector location, int32 amount);

	UFUNCTION(Client, Reliable)
	void ClientRequestXp();

	int32 GetLocalCurrentLevel() const;

	void ForceUpdateXP();

	bool ConsumeLevels_OnlyFromOwningClient(int32 amount);

protected:

	void BeginPlay() override;

private:

	void InternalSetXpClamped(int32 newXpValue);
	void InternalAddXpClamped(int32 xpToAdd);

	UDungeonsGameInstance* GetGameInstance() const;

	int32 LocalGetXp() const;

	void LocalWriteXp(int32) const;

	int32 ClaimUnclaimedXp();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnLevelUp(int32 newLevel);
	
	UFUNCTION()
	void OnRep_XpChanged();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetXp(int32 xp);
	
	UPROPERTY(ReplicatedUsing = OnRep_XpChanged)
	int32 mXp;
	int32 mPreviousXP;

	int32 mXpUnclaimed;

	bool mXpReceivedFromClient;
	
};
