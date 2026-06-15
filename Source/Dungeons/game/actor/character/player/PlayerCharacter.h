#pragma once

#include "game/actor/character/BaseCharacter.h"
#include "GameFramework/PlayerState.h"
#include "game/item/ItemSlot.h"
#include "CommonTypes.h"
#include "PlayerCommonTypes.h"
#include "game/component/InteractableComponent.h"
#include "DungeonsGameMode.h"
#include "lovika/BlockCuboid.h"
#include "lovika/world/level/terrain/TerrainPos.h"
#include "lovika/io/LevelFileCommonTypes.h"
#include "game/level/ambience/Ambience.h"
#include <GameplayEffectTypes.h>
#include "game/component/RefreshSuppliesComponent.h"
#include "game/component/LovikaSpringArmComponent.h"
#include "game/merchant/ui/MerchantWidgetBase.h"
#include "game/util/ActorQuery.h"
#include "interfaces/PlayerCharacterDataProvider.h"
#include <Animation/AnimInstance.h>
#include <StatTracker.h>
#include "MotionDelayBuffer.h"
#include "game/merchant/MerchantDef.h"
#include "game/reward/RewardData.h"
#include "game/item/ArrowItemSlot.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class EAliveState : uint8 {
	Alive,
	Reviving,
	Down,
	Dead
};
ENUM_NAME(EAliveState);

DECLARE_MULTICAST_DELEGATE(FOnAliveStateChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerAliveStateChanged);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerNumberChanged, int, playerNumber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerStateChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOwnedByHostChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackVariantAnimationsAssigned);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerLoadedInLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDodgeRollingChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDodgeRollingCooldownChanged, bool, nowOnCooldown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerMultiDodgeChanged, bool, hasMultiDodge);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerHealthChanged, const FGameplayTagContainer&, tags);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerEnterFreezingWater);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerWillRespawn, float, respawnTimeSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerGameplayEffectUiAdded, FActiveGameplayEffectHandle, handle, class UGameplayEffectUIData*, uiData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerGameplayEffectUiRemoved, FActiveGameplayEffectHandle, handle, class UGameplayEffectUIData*, uiData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerGameplayEffectUiStackCountChanged, FActiveGameplayEffectHandle, handle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBonusMissionUnlocked, ELevelNames, levelName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMerchantUnlocked, TSubclassOf<UMerchantDef>, merchantDef);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNormalDamageReceived);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeakDamageReceived);

DECLARE_MULTICAST_DELEGATE(FOnPlayerRevive);
DECLARE_MULTICAST_DELEGATE(FOnPlayerDeath);
DECLARE_MULTICAST_DELEGATE(FOnPlayerDown);
DECLARE_MULTICAST_DELEGATE(FOnPlayerDeathPrevented);
DECLARE_MULTICAST_DELEGATE(FOnPlayerTeleportedInternal);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerDamageTypeReceived, const FGameplayTag&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerPopped, APlayerCharacter*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMapBlocksRevealed, int);



class UEquipmentComponent;
class UEquipmentDisplayComponent;
class UWalletComponent;
class UCameraComponent;
class USceneComponent;
class ADungeonsGameMode;
class UStatTrackerComponent;
namespace game { struct DifficultyRecommendation; }

UENUM()
enum class APlayerCharacterInternal_ELoadedType {
	NotLoaded,
	LoadedIntroReady,
	LoadedHot
};

UCLASS(meta = (ShortTooltip = "The C++ player character."))
class DUNGEONS_API APlayerCharacter : public ABaseCharacter, public IPlayerCharacterDataProvider, public IGameplayCueInterface {
	GENERATED_BODY()
public:
	
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);
	void PreInitializeComponents() override;
	
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;	

	void Tick(float deltaTime) override;

	UEquipmentComponent* GetEquipmentComponent() const;
	UEquipmentDisplayComponent* GetEquipmentDisplayComponent() const;
	class UTeleportComponent* GetTeleportComponent() const;
	class UPlayerAvatarComponent* GetPlayerAvatarComponent() const;
	class UPlayerCharacterMovementComponent* GetPlayerCharacterMovementComponent() const;
	class ULovikaSpringArmComponent* GetCameraSpringArm() const;
	class ULovikaSpringArmComponent* GetLocalCoopCameraSpringArm() const;
	class UCharacterSerializeComponent* GetCharacterSerializeComponent() const;
	class UCosmeticsComponent* GetCosmeticsComponent() const;
	class UCosmeticsDisplayComponent* GetCosmeticsDisplayComponent() const;
	class UMissionProgressComponent* GetMissionProgressComponent() const;
	class UWalletComponent* GetWalletComponent() const;
	class UItemStashComponent* GetItemStashComponent() const;
	class UPickupItemComponent* GetPickupItemComponent() const;
	class UElytraComponent* GetElytraComponent() const;
	class UOxygenComponent* GetOxygenComponent() const;
	class UClientEventHubComponent* GetClientEventHubComponent() const;
	class UAdventureHubComponent* GetAdventureHubComponent() const;

	void BeginPlay() override;


	void BindEquipmentSlots();

	void PlayXPPickupAudioComponent();
	class UAudioComponent* CreateXPPickupAudioComponent();

	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	virtual void RemoveInstanceTracking() override;

	void PossessedBy(AController* NewController) override;
	void UnPossessed() override;

	void OnPawnPossessed();
	void OnLocalPawnPossessed();

	void OnAttackVariantsUpdated() const;

	void OnItemSlotChanged(UItemSlot* itemSlot) const;

	void PreRespawn();

	void Respawn();

	void RespawnDelayed(float seconds);

	virtual void BeginDestroy() override;

	virtual bool DissasembleCharacter() override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	ETeleportState GetTeleportState() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EAliveState GetAliveState() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool InWorldAndAlive() const { return GetAliveState() == EAliveState::Alive && GetWorldState() == ECharacterWorldState::InWorld; };
	
	void TryInterruptTeleport(bool force = false) const;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerAliveStateChanged OnPlayerAliveStateChanged;
		
	FOnAliveStateChanged OnAliveStateChanged;

	void OnRep_PlayerState() override;

	int32 GetPlayerId() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FText& GetLocalPlayerDisplayText() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FString GetBasePlayerDisplayName() const;

	void InitialSetup(ABasePlayerController* playerController);

	FVector GetClosestNavigablePoint();

	UFUNCTION()
	void IsOwnedByHostChanged();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsOwnedByHost() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnFootstep();

	bool CanSpawnExtraFootstep(EDLCName levelDLCRequired);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Player")
	float RollSpeed = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Player")
	float RollZ = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Dodge")
	float DodgeCooldownShared = 0.3f;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsDodgeOnCooldown() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsDodgeRolling() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasDodgesLeft() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int32 NumDodgesLeft() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsOnGround();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetDodgeCooldownFraction() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UEffectBasedCooldownProvider* GetDodgeCooldownProvider() const;
	
	void OnDodgeCooldownTagChange(const FGameplayTag tag, const int32 tagCount);
	void OnMultiDodgeTagChange(const FGameplayTag tag, const int32 tagCount);
	static const FName DodgeRechargeName;
	
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerDodgeRollingChanged OnPlayerDodgeRollingChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerDodgeRollingCooldownChanged OnPlayerDodgeRollingCooldownChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerMultiDodgeChanged OnPlayerMultiDodgeChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerLoadedInLevel OnPlayerLoadedInLevel;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerWillRespawn OnPlayerWillRespawn;

	void PlayerLoadedInLevel(bool introReady);
	UFUNCTION(BlueprintPure)
	bool IsLoadedInLevel() const;
	bool IsAllowedToSeeIntro() const;
	void RotatePlayerTowardsActor(AActor* target);
	bool IsRotatedTowardsActor(AActor* target, float degreeTolerance = 1.0f) const;
	bool IsRotatedTowardsLocation(const FVector& targetLocation, float degreeTolerance = 1.0f) const;
	void RotatePlayerTowardsAttackTarget(AActor* attacktarget);
	bool IsRotatedTowardsAttackTarget(AActor* attacktarget, float degreeTolerance = 1.0f) const;

	void RotatePlayerTowardsLocation(const FVector& location) const;
	void RotatePlayer(const FRotator& rotator) const;

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void ServerlaunchPlayer(FVector velocity);
	
	UFUNCTION(BlueprintCallable, Server, WithValidation, Reliable)
	void ActivateSupplies(URefreshSuppliesComponent* component);

	void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	// used to get 'isAlive' when have access to PlayerCharacter*, cache to avoid Casting e.g. per frame?
	class ABasePlayerState* GetDungeonsBasePlayerState() const;

	//Stop player characters from getting destroyed.
	void FellOutOfWorld(const UDamageType& dmgType) override;
	void OutsideWorldBounds() override;

	FName GetSkinId() const;
	void GenerateLoadout(ECharacterLoadoutType) const;

	UFUNCTION(Category = "Dungeons", BlueprintCallable)
	int32 GetCharacterLevel() const;
	
	int32 GetLevel_Implementation() const override;
	int32 GetTotalEquippedGearPower_Implementation() const override;
	int32 GetOwnedEmeralds_Implementation() const override;
	int32 GetOwnedGold_Implementation() const override;
	FText GetCharacterName_Implementation() const override;
	FName GetCharacterSkinId_Implementation() const override;

	UFUNCTION(NetMulticast, Reliable)
	void RespawnPlayerMulticast();

	//D11.RR - Used to revive players during cutscenes. 
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void ForceQuickRevive();

	bool IsRespawning() const;
	bool IsRevivingPlayer() const;

	void HandleRevive() override;
	void HandleResurrection();
	void HandleDeath() override;
	virtual void HandleDown();

	virtual void HandleDamageTypeReceived(float amount, const FGameplayTag& tag);

	FOnPlayerRevive OnPlayerRevive;	

	FOnPlayerDeath OnPlayerDeath;

	FOnPlayerDown OnPlayerDown;	

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerEnterFreezingWater OnPlayerEnterFreezingWater;

	FOnPlayerDamageTypeReceived OnPlayerDamageTypeReceived;

	FOnPlayerDeathPrevented OnPlayerDeathPrevented;

	FOnPlayerTeleportedInternal OnPlayerTeleportedInternal;

	FOnPlayerPopped OnPlayerPopped;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnBonusMissionUnlocked OnMissionUnlocked;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnMerchantUnlocked OnMerchantUnlocked;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float TeleportPlayerToActor(const APlayerState* targetPlayerState);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void TeleportToFriend(const APlayerCharacter* player);

	void HandleDamageReceived(float amount) override;

	UFUNCTION()
	void OnPlayerAction(const ABasePlayerController* playerController) const;

	UFUNCTION()
	void OnPlayerTeleported();
	

	UFUNCTION(BlueprintImplementableEvent)
	void OnLocalPlayerTeleported();	

	// #D11.CM
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<APlayerCharacter*> GetTeleportCandidates() const;

	// #D11.CM
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<APlayerCharacter*> GetTeleportDependents() const;
	
	bool IsImmortal() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|UI")
	FAvatarData GetPlayerAvatarData() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|UI")
	FColor GetPlayerColor() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons|UI")
	int GetPlayerNumber() const;

	UFUNCTION()
	void ApplyMaterialToMesh(UMaterialInstance* material);
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsAllowedToPerformAction() const;

	// #D11.CM
	bool TryCoopRespawn();

	void RespawnQuick();

	void RespawnFrozen();

	void OnRespawnFrozenFailed();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanDodge() const;

	TArray<FInventoryItemData> GetInventory() const;
	
	bool IsStuck() const;
	TArray<FVector> GetLikelyRelevantDoorLocations() const;
	TArray<FVector> GetNearbySpawnLocations() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void RestartAtCheckpoint();

	void TeleportToSafety();

	ABasePlayerController* GetPlayerController() const;
	UInteractableComponent* GetPlayerInteractableComponent() { return InteractableComponent; }

	const FAmbienceIDGroup& GetAmbience() const;
	void SetAmbience(FAmbienceIDGroup);
	const FAmbienceAudioIDGroup& GetAmbienceAudio() const;
	void SetAmbienceAudio(FAmbienceAudioIDGroup);

	void SetRevealRadiusScale(float scale);

	void MakePlayerClickable();
	void MakePlayerNonClickable() const;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerGameplayEffectUiAdded OnPlayerGameplayEffectUiAdded;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerGameplayEffectUiRemoved OnPlayerGameplayEffectUiRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerGameplayEffectUiStackCountChanged OnPlayerGameplayEffectUiStackCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnNormalDamageReceived OnNormalDamageReceived;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnWeakDamageReceived OnWeakDamageReceived;

	FOnMapBlocksRevealed OnMapBlocksRevealed;
		
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetIsInInventory(bool inInventory);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsInInventory() const;

	const game::DifficultyRecommendation& GetDifficultyRecommendation() const;

	bool Server_IsReadyToPlay() const;

	UFUNCTION(BlueprintCallable)
	void SetReadyToPlay();

	void BroadcastBlocksRevealed(int blocksCount) const;

	void ResetArrowCount();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UItemSlot* GetItemSlotOfType(ESlotType type, int index) const;

	// #D11.CM
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Player")
	float mPostPopRadius = 150;

	// #D11.CM
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanPop() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool PopToPlayer(const APlayerCharacter* targetPlayer, float popRadiusAngle = 0.0);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool PopToLocation(const FVector& targetLocation, float popRadiusAngle = 0.0);

	void StartPoppingWarning();
	void StopPoppingWarning();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Popping")
	float mPopCombatCooldown = 1.5;

	// #D11.CM - Returns if the player has been in combat within the pop cooldown time.
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasBeenInCombat() { return (GetWorld()->GetTimeSeconds() - mLastCombatTime) < mPopCombatCooldown; };

	// #D11.CM
	void OnAttack();

	/** Used for the Explorer enchantment to avoid the enchantment being activated from an async task
	  * @param amount The number of blocks which have been revealed*/
	void NotifyBlocksRevealed(int amount);

	void OnExternalDodgeRollStart(const FVector& dodgeDirection, FPredictionKey key);
	void OnExternalDodgeRollEnd(FPredictionKey);

	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey key);
	void OnDodgeRollEnd(FPredictionKey);

	void SetCurrentCheckpoint(const BlockCuboid&);
	void SetPreviousCheckpoint(const BlockCuboid&);
	void ClearCurrentCheckpoint();
	TOptional<BlockCuboid> GetCurrentCheckpoint() const;
	TOptional<BlockCuboid> GetPreviousCheckpoint() const;

	int GetCurrentDungeonInstanceId() const;

	// D11.SSN
	UPROPERTY()
	float fullTextDisplayTime = 0.0f;

	// D11.SSN
	UFUNCTION()
	UStatTrackerComponent* GetStatTracker() const;
	void TryInitStatTracker(APlayerController* playerController);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasPendingRewardItem() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int NumPendingRewards() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void ClaimPendingRewardItem() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SalvagePendingRewardItem() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FRewardData GetPendingRewardItem() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	ERewardType GetPendingRewardItemRewardType() const;

	// D11.SSN
	bool GetShowInventoryFullWarning() const { return showInventoryFullWarning; };
	void SetInventoryFullWarning();

	TOptional<FString> GetOnlineUserId() const;

	UFUNCTION(BlueprintNativeEvent)
	void OnOtherPlayerJoined();

	UFUNCTION(BlueprintNativeEvent)
	void OnOtherPlayerLeft();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerNumberUpdated();	

	void SteppedOnNewBlock(const FullBlock& block) override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsIcyBreath() const;

	UFUNCTION(Client, Reliable)
	void ClientUnlockBonusMission(ELevelNames level);

	UFUNCTION(Client, Reliable)
	void ClientUnlockMerchant(TSubclassOf<UMerchantDef> merchantDef);

	void Dodge(const FVector& direction);

	FORCEINLINE class UDamageNumberBatchingComponent* GetDamageNumberBatchingComponent() { return DamageNumberBatchingComponent; }

	// D11.SSN
	UFUNCTION()
	void OnRep_TNTKillCount();
	UFUNCTION()
	void OnRep_OpenedChestRarity();
	UFUNCTION()
	void OnRep_IceWandKillCount();
	UFUNCTION()
	void OnRep_OcelotArmourTracking();
	UFUNCTION()
	void OnRep_IceWeaponTracking();
	UFUNCTION()
	void OnRep_EmeraldArmourTracking();
	UFUNCTION()
	void OnRep_SpinBladeHitCount();
	UFUNCTION()
	void OnRep_TridentKillCount();
	UFUNCTION()
	void OnRep_ElytraAttackHitCount();
	UFUNCTION()
	void OnRep_VoidEffectMagnitude();

	// D11.SSN
	UPROPERTY(Transient, ReplicatedUsing = OnRep_TNTKillCount)
	uint16 TNTKillCount;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_OpenedChestRarity)
	EItemRarityChanceCategory LastOpenedChestRarity;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_IceWandKillCount)
	uint16 IceWandKillCount;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_OcelotArmourTracking)
	bool OcelotArmourTracking = false;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_IceWeaponTracking)
	bool IceWeaponTracking = true;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_EmeraldArmourTracking)
	bool EmeraldArmourTracking = false;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_SpinBladeHitCount)
	uint16 SpinBladeHitCount;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_TridentKillCount)
	uint16 TridentKillCount;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_ElytraAttackHitCount)
	uint16 ElytraAttackHitCount;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_VoidEffectMagnitude)
	float VoidEffectMagnitude = 0.0f;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnLlamaOffering();
	UFUNCTION(Client, Reliable)
	void Client_OnLlamaOffering();
	UFUNCTION()
	void OnIceWandGlowSquid();
	UFUNCTION(Client, Reliable)
	void Client_OnIceWandGlowSquid();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnEnterBubbleColumn(bool entered);
	UFUNCTION(Client, Reliable)
	void Client_OnEnterBubbleColumn(bool entered);
	UFUNCTION()
	void OnBubbledEnemy(bool bubbled);
	UFUNCTION(Client, Reliable)
	void Client_OnBubbledEnemy(bool bubbled);
	UFUNCTION()
	void OnLostConduit();
	UFUNCTION(Client, Reliable)
	void Client_OnLostConduit();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void OnSquideasterEggActive();
	UFUNCTION(Client, Reliable)
	void Client_OnSquidEasterEggActive();
	UFUNCTION()
	void TryVoidAchievement();
	UFUNCTION(Client, Reliable)
	void Client_TryVoidAchievement();
	
	void SetCapsuleCollisionResponse(ESynchornizedCollisionResponse response) override;

	void ApplyDownedPushImmunity();
	void RemoveDownedPushImmunity();

	//D11.GM
	UFUNCTION()
	void OnLevitate(const FGameplayTag tag, const int32 tagCount) override;
	UFUNCTION(Client, Reliable)
	void Client_OnLevitate();

private:
	void NotifyDodgeRollStart(const FVector& dodgeDirection, FPredictionKey key);
	void NotifyDodgeRollEnd(FPredictionKey);

	void UpdateLastTerrainPos();
	void UpdateGeneratedLevelOnControllingClient();
	void UpdateBlocksRevealed();	

	void HandleActiveGameplayEffectAdded(UAbilitySystemComponent* abilitySystem, const FGameplayEffectSpec& spec, FActiveGameplayEffectHandle handle);
	void HandleActiveGameplayEffectRemoved(const FActiveGameplayEffect& gameplayEffect);	
	void HandleActiveGameplayEffectStackCountChanged(FActiveGameplayEffectHandle handle, int32 newCount, int32 oldCount) const;
	
	bool IsInDevelopmentLevel() const;

	void SetImmortal(float seconds);
	void RemoveImmortality();
	
	EAliveState AliveState = EAliveState::Alive;

	void SetAliveState(EAliveState newState);

	void RefreshAliveState();

	void OnPartsDiscoveredChanged();
	void OnLivesLostThisSessionChanged();

	void HandlePreventDeath();

	game::Game* GetGame() const;

	void NotifyLocalPlayerJoined();

	void NotifyLocalPlayerLeft();

	UFUNCTION(Server, Reliable, WithValidation)
	void SetReadyToPlayInternal();

	UFUNCTION()
	void GameplayCue_Damage();

	UFUNCTION()
	void GameplayCue_Damage_Weak();

	void TryEquipElytraUponLevelLoad();

	bool bInInventory;

	bool bIsReadyToPlay = false;

	FActiveGameplayEffectHandle DownedPushImmunityHandle;

	bool bHasBegunPlayOnControllingClient = false;
	bool bHasGeneratedLevelOnControllingClient = false;

	FRotator cachedCameraRotation;
	FRotator cachedLMPCameraRotation;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|Elytra")
	AActor* ElytraLandMarker;

	UFUNCTION(BlueprintImplementableEvent)
	void OnCharacterDown();

	UFUNCTION(BlueprintImplementableEvent)
	bool LaunchCharacterToPos(FVector destination, float launchSpeed, bool traceFullPath = false);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	TMap<EMaterialTypeEnum, USoundCue*> FootstepSoundMap;

	UPROPERTY()
	USoundCue* LastStepSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	TMap<EMaterialTypeEnum, UParticleSystem*> FootstepParticleMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	TMap<EDLCName, USoundCue*> DLCExtraFootstepSoundMap;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnOwnedByHostChanged OnIsOwnedByHostChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
	UEquipmentComponent* EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
	UEquipmentDisplayComponent* EquipmentDisplayComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
	UWalletComponent* WalletComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
	class UPlayerAvatarComponent *PlayerAvatarComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
	class USoulComponent* SoulComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
	class UPlayerExperienceComponent* PlayerExperienceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category="Dungeons")
	class UMapPinComponent* MapPinComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
	class UTeleportComponent* TeleportComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Replicated, Category = "Dungeons")
	class UReviveComponent* ReviveComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundCue* XPPickupSound = nullptr;

	UPROPERTY()
	class UAudioComponent* XPPickupAudioComponent = nullptr;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	class UItemStashComponent* ItemStashComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Dungeons")
	class UPickupItemComponent* PickupItemComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Dungeons")
	class UKillTrackerComponent* KillTrackerComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Dungeons")

	class UMissionProgressComponent* MissionProgressComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeons")
	class UCharacterLazySaveComponent* CharacterLazySaveComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeons")
	class UCosmeticsComponent* CosmeticsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeons")
	class UCosmeticsDisplayComponent* CosmeticsDisplayComponent;

	UPROPERTY()
	class UDamageNumberBatchingComponent* DamageNumberBatchingComponent;

	UPROPERTY(Transient)
	class UClientEventHubComponent* ClientEventHubComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dungeons")
	class UAdventureHubComponent* AdventureHubComponent;

	// D11.SSN
	UPROPERTY(VisibleDefaultsOnly, Category = "Dungeons")
	UStatTrackerComponent* StatTrackerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UOxygenComponent* OxygenComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dungeons")
	class UElytraComponent* ElytraComponent;

	//Animations
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Animation")
	TSubclassOf<UAnimInstance> DefaultAnimationsInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Animation")
	class UAnimMontage* StunnedMontage;
		

	void OnPlayerNumberChangedInternal();

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Dungeons")
	class ULovikaSpringArmComponent *CameraSpringArm;
	
	UPROPERTY(BlueprintReadWrite, Transient, Category = "Dungeons")
	class ULovikaSpringArmComponent *LMPCameraSpringArm;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerHealthChanged OnPlayerHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerNumberChanged OnPlayerNumberChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPlayerStateChanged OnPlayerStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnAttackVariantAnimationsAssigned OnAttackVariantAnimationsAssigned;	

	void OnStunned(const FGameplayTag tag, const int32 tagCount);

	TArray<ECustomTraceChannels> GetNearMissSelectionChannels() override;

	UPROPERTY(BlueprintReadWrite)
		float AudioListenerTweenAlpha = 0.0f;

	UPROPERTY(BlueprintReadWrite)
		uint8 IsViewTarget:1;

private:
	bool ExecuteTaskOnLastSafeNavPositions(std::function<bool(const FVector&)> task);

	void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	
	void SetIsOverlappingKillzone(bool overlapping);
	void RefreshKillzoneCollision();
	void SetKillzoneCollisionEnabled(bool enabled);
	void SetIsWalking(bool landed);

	bool mKillzoneCollisionEnabled = false;
	bool mIsOverlappingKillzone = false;
	bool mIsWalking = true;
	bool mIsInPushVolume = false;

	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	

	void OnHealthChanged(const FOnAttributeChangeData& data) const;

	bool IsTimerActive(const FTimerHandle& timerHandle) const;

	void OnPushVolume(const FGameplayTag tag, const int32 tagCount);

	UFUNCTION()
	void HandleInteraction(ACharacter* interactor);

	void OnTerrainPosChanged(const TerrainPos& lastPos, const TerrainPos& newPos);

	void UnlockEndersentTiles();

	ADungeonsGameMode* GetGameMode() const;

	UPROPERTY()
	UAudioComponent* mPlayerDeathSoundComponent;

	UPROPERTY()
	UAudioComponent* mPlayeDownSoundComponent;

	UPROPERTY()
	UCameraComponent* mDetachedCameraReference;

	UPROPERTY()
	USceneComponent* mCameraParentComponent;

	TWeakObjectPtr<AActor> mGhostPostProcessActor;
	
	FTimerHandle mPreRespawnDelayHandle;
	FTimerHandle mRespawnDelayHandle;
	FTimerHandle mImmortalTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	UInteractableComponent* InteractableComponent;

	int mBlocksRevealedSinceLastTick = 0;

	bool mIsImmortal = false;

	TerrainPos mLastTerrainPos;

	FAmbienceIDGroup mAmbience;
	FAmbienceAudioIDGroup mAmbienceAudio;
	
	// To void 0.0 delays in timers
	float revealRadiusScale = 1.f;

	ECollisionResponse CachedCollisionResponse = ECR_Ignore;
	ECustomTraceChannels SkeletalMeshChannel = ECustomTraceChannels::TargetSelect;

	UFUNCTION()
	void OnRep_LoadedType();

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_LoadedType)
	APlayerCharacterInternal_ELoadedType mLoadedType = APlayerCharacterInternal_ELoadedType::NotLoaded;

	// #D11.CM
	float mLastCombatTime = 0;

	// #D11.CM
	const float mPopCooldownThreshold = 0.5;
	float mLastPopTime = 0;

	TOptional<BlockCuboid> mCurrentCheckpoint;
	TOptional<BlockCuboid> mPreviousCheckpoint;
	int mCurrentDungeonInstanceId = -1;

	// D11.SSN
	bool showInventoryFullWarning = false;
	FTimerHandle fullTimerHandle;
	void UnsetInventoryFullWarning();

	//#D11.CM
	FTimerHandle QuickRespawnTimerHandle;
	FTimerHandle SafetyTeleportTimerHandle;

	// #D11.CM
	TCircularHistoryBuffer<FVector> mLastSafePositions;
	FTimerHandle mLastSafePositionUpdateHandle;
	FTimerHandle mFrozenRespawnTimoutHandle;
	void UpdateSafePositions();
};
