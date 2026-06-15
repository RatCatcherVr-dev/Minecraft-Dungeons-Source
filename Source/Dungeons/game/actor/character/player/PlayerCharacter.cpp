#include "Dungeons.h"
#include "PlayerCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/component/PlayerExperienceComponent.h"
#include "game/component/TeleportComponent.h"
#include "game/avatar/PlayerAvatarComponent.h"
#include "game/Game.h"
#include "game/util/ActorQuery.h"
#include "BasePlayerController.h"
#include "BasePlayerState.h"
#include "Camera/CameraComponent.h"
#include "game/component/LovikaSpringArmComponent.h"
#include "game/level/GameTiles.h"
#include "GameplayTagsManager.h"
#include <AbilitySystemComponent.h>
#include "DungeonsGameMode.h"
#include "DungeonsGameInstance.h"
#include "DungeonsGameState.h"
#include <Kismet/KismetMathLibrary.h>
#include "game/item/instance/MeleeWeaponGearItemInstance.h"
#include "game/actor/item/ItemDropChanceActor.h"
#include "game/ArmorProperties/ArmorPropertiesComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/OxygenAttributeSet.h"
#include "game/component/EquipmentComponent.h"
#include "game/component/EquipmentDisplayComponent.h"
#include "game/component/SoulComponent.h"
#include "game/component/ReviveComponent.h"
#include "game/component/RagdollOnDeathComponent.h"
#include "game/component/MapPinComponent.h"
#include "game/component/WalletComponent.h"
#include "game/component/KillTrackerComponent.h"
#include "game/component/MissionProgressComponent.h"
#include "game/component/ItemStashComponent.h"
#include "game/component/PickupItemComponent.h"
#include "game/component/CharacterLazySaveComponent.h"
#include "game/util/LocationQuery.h"
#include "world/level/block/Block.h"
#include "lovika/LevelCommon.h"
#include "game/util/LocalizationDefs.h"
#include "util/EnumUtil.h"
#include "game/component/PlayerCharacterMovementComponent.h"
#include "game/GameBP.h"
#include "game/Conversion.h"
#include "game/GameProgress.h"
#include "game/component/AttackComponent.h"
#include "GameplayTagContainer.h"
#include "game/abilities/effects/StaggerGameplayEffect.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/util/ComponentUtils.h"
#include "game/actor/item/TNTBoxItem.h"
#include "game/actor/item/ConduitItem.h"
#include "game/component/cosmetics/CosmeticsComponent.h"
#include "game/component/cosmetics/CosmeticsDisplayComponent.h"
#include "client/renderer/block/BlockGraphicsPack.h"
#include "game/component/mode/RespawnAsTeamUsingLivesComponent.h"
#include "game/component/RangedAttackComponent.h"
#include "Kismet/GameplayStatics.h"
#include "online/sessions/OnlineUtil.h"
#include "game/UEffectBasedCooldownProvider.h"
#include "game/Enchantments/FrozenSolid.h"
#include "game/Enchantments/PushVolumeResistance.h"
#include <NavigationSystem.h>
#include "game/component/DamageNumberBatchingComponent.h"
#include "lovika/RegionPredicates.h"
#include "game/util/UnlockKeyUtils.h"
#include "game/component/OxygenComponent.h"
#include "game/difficulty/Difficulty.h"
#include "game/component/ElytraComponent.h"
#include "game/item/instance/RangedWeaponGearItemInstance.h"
#include "gamemodes/LobbyGameMode.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "online/crossplay/Identity.h"
#include "game/util/DungeonsGearUtilLibrary.h"
#include "game/actor/character/DungeonsAbilitySystemComponent.h"
#include "game/actor/ProjectileManager.h"
#include <GameplayEffectExtension.h>
#include "game/reward/RewardData.h"
#include "game/item/ArrowItemSlot.h"
#include "ClientEventHub.h"
#include "game/component/AdventureHubComponent.h"
#include "game/actor/character/player/PlayerCoopCamera.h"
#include "game/component/blocktriggers/VoidBlockTrigger.h"

DECLARE_CYCLE_STAT(TEXT("STAT_APlayerCharacter_Tick_TickDodge"), STAT_APlayerCharacter_Tick_TickDodge, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_APlayerCharacter_Tick_TerrainPos"), STAT_APlayerCharacter_Tick_TerrainPos, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_APlayerCharacter_Tick Super::Tick"), STAT_APlayerCharacter_TickSuperTick, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_APlayerCharacter_Tick_"), STAT_APlayerCharacter_Tick_, STATGROUP_PlayerController);

DECLARE_CYCLE_STAT(TEXT("STAT_APlayerCharacter_Tick_ueToTerrain"), STAT_APlayerCharacter_Tick_ueToTerrain, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_APlayerCharacter_Tick_OnTerrainPosChanged"), STAT_APlayerCharacter_Tick_OnTerrainPosChanged, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_APlayerCharacter_Tick_ServerClientGeneretedLevel"), STAT_APlayerCharacter_Tick_ServerClientGeneretedLevel, STATGROUP_PlayerController);

const FName APlayerCharacter::DodgeRechargeName = "GameplayCue.StatusEffect.DodgeRecharge";
static FContextSensitiveAction s_reviveAction;

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerCharacterMovementComponent>(CharacterMovementComponentName)) {

	s_reviveAction = FContextSensitiveAction({ dungeonsloc::GetText("action_revive"), FName("MainAttackGamepad") });

	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
	EquipmentDisplayComponent = CreateDefaultSubobject<UEquipmentDisplayComponent>(TEXT("EquipmentDisplayComponent"));
	SoulComponent = CreateDefaultSubobject<USoulComponent>(TEXT("SoulComponent"));
	PlayerExperienceComponent = CreateDefaultSubobject<UPlayerExperienceComponent>(TEXT("PlayerExperienceComponent"));
	TeleportComponent = CreateDefaultSubobject<UTeleportComponent>(TEXT("TeleportComponent"));
	ReviveComponent = CreateDefaultSubobject<UReviveComponent>(TEXT("ReviveComponent"));
	MapPinComponent = CreateDefaultSubobject<UMapPinComponent>(TEXT("MapPinComponent"));
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("PlayerClickyComponent"));
	PlayerAvatarComponent = CreateDefaultSubobject<UPlayerAvatarComponent>(TEXT("PlayerAvatarComponent"));
	MissionProgressComponent = CreateDefaultSubobject<UMissionProgressComponent>(TEXT("MissionProgressComponent"));
	KillTrackerComponent = CreateDefaultSubobject<UKillTrackerComponent>(TEXT("KillTrackerComponent"));
	ClientEventHubComponent = CreateDefaultSubobject<UClientEventHubComponent>(TEXT("ClientEventHubComponent"));
	WalletComponent = CreateDefaultSubobject<UWalletComponent>(TEXT("WalletComponent"));
	ItemStashComponent = CreateDefaultSubobject<UItemStashComponent>(TEXT("ItemStashComponent"));
	PickupItemComponent = CreateDefaultSubobject<UPickupItemComponent>(TEXT("PickupItemComponent"));
	CharacterLazySaveComponent = CreateDefaultSubobject<UCharacterLazySaveComponent>(TEXT("CharacterLazySaveComponent"));
	CosmeticsComponent = CreateDefaultSubobject<UCosmeticsComponent>(TEXT("CosmeticsComponent"));
	CosmeticsDisplayComponent = CreateDefaultSubobject<UCosmeticsDisplayComponent>(TEXT("CosmeticsDisplayComponent"));
	DamageNumberBatchingComponent = CreateDefaultSubobject<UDamageNumberBatchingComponent>(TEXT("DamageNumberBatcher"));
	OxygenComponent = CreateDefaultSubobject<UOxygenComponent>(TEXT("OxygenComponent"));
	ElytraComponent = CreateDefaultSubobject<UElytraComponent>(TEXT("ElytraComponent"));
	ElytraComponent->SetupAttachment(GetMesh(), TEXT("J_Shoulders"));
	InteractableComponent->SetType(EClickyEnum::CE_Interactable);
	InteractableComponent->SetBalloontipBehavior(EBalloonTipBehavior::Hidden);
	InteractableComponent->Deactivate();
	AdventureHubComponent = CreateDefaultSubobject<UAdventureHubComponent>(TEXT("AdventureHubComponent"));

	// for each actor that needs an Equipment component :
	CreateDefaultSubobject<UItemSlot>(TEXT("MeleeWeaponItemSlot"))->SlotTypeId = ESlotType::MeleeWeapon;
	CreateDefaultSubobject<UItemSlot>(TEXT("ArmorItemSlot"))->SlotTypeId = ESlotType::Armor;
	CreateDefaultSubobject<UItemSlot>(TEXT("RangedWeaponItemSlot"))->SlotTypeId = ESlotType::RangedWeapon;
	UItemSlot* slot1 = CreateDefaultSubobject<UItemSlot>(TEXT("ArtifactItemSlot1"));
	slot1->SlotTypeId = ESlotType::ActivePermanent;
	slot1->GetEffectsBasedCooldownProvider()->CooldownTag = FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Slot1"));
	UItemSlot* slot2 = CreateDefaultSubobject<UItemSlot>(TEXT("ArtifactItemSlot2"));
	slot2->SlotTypeId = ESlotType::ActivePermanent;
	slot2->GetEffectsBasedCooldownProvider()->CooldownTag = FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Slot2"));
	UItemSlot* slot3 = CreateDefaultSubobject<UItemSlot>(TEXT("ArtifactItemSlot3"));
	slot3->SlotTypeId = ESlotType::ActivePermanent;
	slot3->GetEffectsBasedCooldownProvider()->CooldownTag = FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Slot3"));
	UItemSlot* healthPotion = CreateDefaultSubobject<UItemSlot>(TEXT("HealthPotionSlot"));
	healthPotion->SlotTypeId = ESlotType::HealthPotion;
	healthPotion->GetEffectsBasedCooldownProvider()->CooldownTag = FGameplayTag::RequestGameplayTag(TEXT("Cooldown.SlotHealthPotion"));

	CreateDefaultSubobject<UCosmeticsSlot>(TEXT("CapeSlot"))->SetType(ECosmeticType::Cape);
	CreateDefaultSubobject<UCosmeticsSlot>(TEXT("PetSlot"))->SetType(ECosmeticType::Pet);

	// replication config
	bOnlyRelevantToOwner = false;
	bAlwaysRelevant = true;
	bReplicateMovement = true;
	bNetLoadOnClient = true;
	bNetUseOwnerRelevancy = false;
	bReplicates = true;

	NetCullDistanceSquared = 225000000.0;
	NetUpdateFrequency = 25.0f;
	MinNetUpdateFrequency = 10.0f;
	NetPriority = 10.0f;

	NetDormancy = ENetDormancy::DORM_Never;
}

void APlayerCharacter::PreInitializeComponents() {
	ABaseCharacter::PreInitializeComponents();

	AbilitySystem->AddSet<UHealthAttributeSet>();
	AbilitySystem->AddSet<UOxygenAttributeSet>();
}

bool APlayerCharacter::IsInDevelopmentLevel() const {
	return !Game.IsValid();
}

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	IsViewTarget = 0u;
	mLastSafePositions.Resize(8);

	InstanceTracker< APlayerCharacter >::AddInstance(GetWorld(),this);

	if (GetDungeonsBasePlayerState() != nullptr) {
		OnRep_PlayerState();
	}

	CreateXPPickupAudioComponent();


	GetAbilitySystemComponent()->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Stunned"))).AddUObject(this, &APlayerCharacter::OnStunned);
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.PushVolume")).AddUObject(this, &APlayerCharacter::OnPushVolume);
	
	GetAbilitySystemComponent()->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &APlayerCharacter::HandleActiveGameplayEffectAdded);

	if (IsLocallyControlled()) {
		for (const auto& data : AbilitySystem->GetActiveEffectsWithUiData()) {
			AbilitySystem->OnGameplayEffectStackChangeDelegate(data.Handle)->AddUObject(this, &APlayerCharacter::HandleActiveGameplayEffectStackCountChanged);
		}
	}
	GetAbilitySystemComponent()->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &APlayerCharacter::HandleActiveGameplayEffectRemoved);	
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(GetDodgeCooldownProvider()->CooldownTag).AddUObject(this, &APlayerCharacter::OnDodgeCooldownTagChange);
	GetAbilitySystemComponent()->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.MultiDodge")).AddUObject(this, &APlayerCharacter::OnMultiDodgeTagChange);

	CachedCollisionResponse = GetMesh()->GetCollisionResponseToChannel(static_cast<ECollisionChannel>(ECustomTraceChannels::TargetSelect));

	if (HasAuthority()) {
		if (auto* itemDropActor = actorquery::getFirstActor<AItemDropChanceActor>(GetWorld())) {
			itemDropActor->RegisterPlayer(this);
		}

		if (IsInDevelopmentLevel()) {
			//No game present, this means we are not playing in a lovika level - so just setup the default debugging loadout.
			GenerateLoadout(ECharacterLoadoutType::DefaultLoadout);
		}
	}

	// Start the "Safe Position" timer
	GetWorldTimerManager().SetTimer(mLastSafePositionUpdateHandle, this, &APlayerCharacter::UpdateSafePositions, 0.5f, true, 0.5f);

	// bind health component onHit:
	if (auto* healthComponent = GetHealthComponent()) {
		healthComponent->OnChanged.AddUObject(this, &APlayerCharacter::OnHealthChanged);
		healthComponent->OnDeath.AddUObject(this, &APlayerCharacter::RefreshAliveState);
		healthComponent->OnRevive.AddUObject(this, &APlayerCharacter::RefreshAliveState);
		OnPlayerHealthChanged.Broadcast(FGameplayTagContainer());
	}

	if (auto* cameraSpringArm = componentutils::GetComponentByTag<ULovikaSpringArmComponent>(this, FName{ "CameraSpringArm" })) {
		CameraSpringArm = cameraSpringArm;
	}

	if (auto* teleportComponent = FindComponentByClass<UTeleportComponent>()) {
		teleportComponent->OnTeleported.AddUObject(this, &APlayerCharacter::OnPlayerTeleported);
	}

	if (HasAuthority()) {
		FScopedPredictionWindow(AbilitySystem, FPredictionKey::CreateNewServerInitiatedKey(AbilitySystem), true);
	}

	//D11.KS - Fix for local coop reviving.
	//if (!IsLocallyControlled())
	{
		InteractableComponent->OnInteracted.AddDynamic(this, &APlayerCharacter::HandleInteraction);
		InteractableComponent->SetContextSensitiveAction(s_reviveAction);

		// #D11.CM - Bumped the prompt offset slightly on the revive prompt.
		FVector2D bumpedOffset{ InteractableComponent->GetContextPromptOffset().X, InteractableComponent->GetContextPromptOffset().Y + 50 };
		InteractableComponent->SetContextPromptOffset(bumpedOffset);
		MakePlayerNonClickable();
	}

	if (auto* avatarComponent = GetPlayerAvatarComponent()) {
		avatarComponent->OnApplyMeshMaterial.AddUObject(this, &APlayerCharacter::ApplyMaterialToMesh);
		avatarComponent->RefreshMeshMaterial();
	}

	if (auto* equipmentComponent = GetEquipmentComponent()) {
		auto gearSlots = equipmentComponent->GetGearSlots();

		if (auto* equipmentDisplayComponent = GetEquipmentDisplayComponent()) {
			equipmentDisplayComponent->SetDefaultAnimationsInstance(DefaultAnimationsInstance);
			equipmentDisplayComponent->UpdatePlayerAnimation();
		}

		for (auto& slot : gearSlots) {
			slot->OnItemInstanceReplicatedInternal.AddUObject(this, &APlayerCharacter::OnItemSlotChanged);
		}
	}

	const auto game = actorquery::getFirstActor<AGameBP>(GetWorld());
	if (game && game->IsInitialisationComplete())
	{
		//We have finished initialisation, our physics should be stable do, we can do binds here
		BindEquipmentSlots();
	}

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnCapsuleBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnCapsuleEndOverlap);

#if PLATFORM_SWITCH
	if (auto* sceneCaptureComponent = FindComponentByClass<USceneCaptureComponent2D>()) {
		sceneCaptureComponent->Deactivate();
	}
#endif

	if (auto* gameState = Cast<ADungeonsGameState>(GetWorld()->GetGameState())) {
		gameState->OnLivesLostThisSessionChanged.AddUObject(this, &APlayerCharacter::OnLivesLostThisSessionChanged);
		gameState->OnPartsDiscoveredChanged.AddUObject(this, &APlayerCharacter::OnPartsDiscoveredChanged);
		gameState->OnRevivePossibleChanged.AddUObject(this, &APlayerCharacter::RefreshAliveState);
		gameState->AddPlayerCharacter(this);
	}

	UnlockEndersentTiles();

	RefreshAliveState();
}


void APlayerCharacter::BindEquipmentSlots()
{
	if (auto* equipmentComponent = GetEquipmentComponent()) {
		auto gearSlots = equipmentComponent->GetGearSlots();

		if (auto* equipmentDisplayComponent = GetEquipmentDisplayComponent()) {
			equipmentDisplayComponent->BindToGearSlots(gearSlots);
			equipmentDisplayComponent->UpdatePlayerAnimation();
		}
	}

	if (auto* cosmeticsComponent = GetCosmeticsComponent()) {
		if (auto* cosmeticsDisplayComponent = GetCosmeticsDisplayComponent()) {
			cosmeticsDisplayComponent->BindToSlots(cosmeticsComponent->GetSlots());
		}
	}
}

void APlayerCharacter::PlayXPPickupAudioComponent()
{
	if (auto* pAudioComp = CreateXPPickupAudioComponent())
	{
		pAudioComp->Play();
	}
}

class UAudioComponent* APlayerCharacter::CreateXPPickupAudioComponent()
{
	if (XPPickupSound && !XPPickupAudioComponent)
	{
		XPPickupAudioComponent = UGameplayStatics::SpawnSoundAttached(XPPickupSound, GetRootComponent());
		if (XPPickupAudioComponent)
		{
			XPPickupAudioComponent->bAutoDestroy = false;
			XPPickupAudioComponent->Stop();
			XPPickupAudioComponent->bAutoManageAttachment = true;
		}
	}

	return XPPickupAudioComponent;
}

void APlayerCharacter::EndPlay(EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	InstanceTracker<APlayerCharacter>::RemoveInstance(GetWorld(),this);

	if (auto* gameState = Cast<ADungeonsGameState>(GetWorld()->GetGameState())) {
		gameState->RemovePlayerCharacter(this);
	}

	GetWorldTimerManager().ClearAllTimersForObject(this);

	if (HasAuthority()) {
		if (auto* itemDropActor = actorquery::getFirstActor<AItemDropChanceActor>(GetWorld())) {
			itemDropActor->UnRegisterPlayer(this);
		}

	}
}

void APlayerCharacter::RemoveInstanceTracking()
{
	Super::RemoveInstanceTracking();
	InstanceTracker< APlayerCharacter >::RemoveInstance(GetWorld(), this);
}

void APlayerCharacter::BroadcastBlocksRevealed(int blocksCount) const {
	OnMapBlocksRevealed.Broadcast(blocksCount);
}

UItemSlot* APlayerCharacter::GetItemSlotOfType(ESlotType type, int index) const {
	auto slotsFound = GetEquipmentComponent()->GetSlotsOfType(type);
	if (slotsFound.IsValidIndex(index)) {
		return slotsFound[index];
	}
	return nullptr;
}

bool APlayerCharacter::CanPop() const {
	if (GetWorld()->GetTimeSeconds() - mLastPopTime > mPopCooldownThreshold) {
		return GetWorldState() == ECharacterWorldState::InWorld && Cast<ABasePlayerController>(GetPlayerController())->CurrentCharacterScreenStatus > ELocalPlayerCameraStatus::InSafeZone;
	}

	return false;
}

bool APlayerCharacter::PopToLocation(const FVector& targetLocation, float popRadiusAngle /* = 0 */)
{
	// Get our designated pop position along our radius
	FVector destination = targetLocation + (mPostPopRadius * FVector(Math::cos(popRadiusAngle), Math::sin(popRadiusAngle), 0));

	// #D11.CM - Try popping to our designated position
	if (
		GetTeleportComponent()->PopToPosition(destination, targetLocation) ||
		GetTeleportComponent()->TeleportToRaw(targetLocation, FRotator::ZeroRotator)
		) {
		mLastPopTime = GetWorld()->GetTimeSeconds();

		// Apply popping gameplay effect
		AbilitySystem->ApplyGameplayEffectToSelf(Cast<UPoppingEffect>(UPoppingEffect::StaticClass()->GetDefaultObject()), 1, AbilitySystem->MakeEffectContext());

		OnPlayerPopped.Broadcast(this);
		return true;
	}

	return false;
}

bool APlayerCharacter::PopToPlayer(const APlayerCharacter* targetPlayer, float popRadiusAngle /* = 0 */) {
	if (PopToLocation(targetPlayer->GetActorLocation()))
	{
		const UPlayerCharacterMovementComponent* targetMovementComponent = targetPlayer->GetPlayerCharacterMovementComponent();
		UPlayerCharacterMovementComponent* movementComponent = GetPlayerCharacterMovementComponent();
		if (movementComponent && targetMovementComponent && targetMovementComponent->IsMovingWithElytra())
		{
			ElytraComponent->SetIsFlying(true);
			movementComponent->SetMovementMode(MOVE_Custom, (uint8)ECustomMovementType::Gliding);
			movementComponent->Velocity = targetMovementComponent->Velocity;
			movementComponent->UpdateComponentVelocity();
		}

		return true;
	}

	return false;
}

void APlayerCharacter::StartPoppingWarning(){
	auto cueTag = FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Popping.Warning");
	if (!AbilitySystem->IsGameplayCueActive(cueTag)) {
		AbilitySystem->AddGameplayCue(cueTag);
	}
}

void APlayerCharacter::StopPoppingWarning() {
	AbilitySystem->RemoveGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Popping.Warning"));
}

void APlayerCharacter::OnAttack()
{
	mLastCombatTime = GetWorld()->GetTimeSeconds();
}

void APlayerCharacter::NotifyBlocksRevealed(int amount)
{
	mBlocksRevealedSinceLastTick += amount;
}


FName APlayerCharacter::GetSkinId() const {
	if (const auto* avatar = PlayerAvatarComponent) {
		return avatar->GetSkinId();
	}
	return NAME_None;
}

void APlayerCharacter::GenerateLoadout(const ECharacterLoadoutType loadout) const {
	//Default 1 health potion
	EquipmentComponent->GetSlot(ESlotType::HealthPotion).EquipItem(FInventoryItemData(game::item::type::HealthPotion.getId()));

	if(Cast<ALobbyGameMode>(GetGameMode())) {
		SoulComponent->ServerAddSouls(SoulComponent->GetMaxSoulCount());
	}

	switch (loadout) {
	case ECharacterLoadoutType::DefaultLoadout:		
		//Default arrow count should be higher than the pickups during gameplay		
		EquipmentComponent->GetSlot(ESlotType::Arrow).EquipItem(FInventoryItemData(game::item::type::Arrow.getId()), game::item::type::Arrow.getStoreCount()*2);
		break;
	case ECharacterLoadoutType::TutorialLoadout:
		//No other extra starting items;
		break;
	default:
		break;
	}
}

int32 APlayerCharacter::GetCharacterLevel() const {
	if (const auto* experienceComponent = FindComponentByClass<UPlayerExperienceComponent>()) {
		return experienceComponent->CurrentLevel();
	}
	return 0;
}

int32 APlayerCharacter::GetLevel_Implementation() const {
	return GetCharacterLevel();
}

int32 APlayerCharacter::GetTotalEquippedGearPower_Implementation() const {
	if (const auto* equipmentComponent = GetEquipmentComponent()) {
		return equipmentComponent->GetTotalEquippedDisplayItemPower();
	}
	return 0;
}

int32 APlayerCharacter::GetOwnedEmeralds_Implementation() const {
	if (auto* wallet = WalletComponent) {
		return wallet->GetEmeraldBalance();
	}
	return 0;
}


int32 APlayerCharacter::GetOwnedGold_Implementation() const {
	if (auto* wallet = WalletComponent) {
		return wallet->GetGoldBalance();
	}
	return 0;
}

FName APlayerCharacter::GetCharacterSkinId_Implementation() const {
	return GetSkinId();
}

FText APlayerCharacter::GetCharacterName_Implementation() const {
	return FText::FromString("YEP-YEP-YEP-YEP-YEP");
}

bool APlayerCharacter::IsRespawning() const {
	return IsTimerActive(mPreRespawnDelayHandle) || IsTimerActive(mRespawnDelayHandle);
}

bool APlayerCharacter::IsRevivingPlayer() const
{
	return ReviveComponent->IsCurrentlyRevivingLocal();
}

void APlayerCharacter::ResetArrowCount() {
	const ItemType& arrowType = game::item::type::Arrow;
	const auto& rangedSlot = GetEquipmentComponent()->GetSlot(ESlotType::RangedWeapon);
	const auto* rangedWeapon = Cast<ARangedWeaponGearItemInstance>(rangedSlot.GetItem());

	// Ideally we should just use rangedWeapon->GetProjectileStackCount() here, but since we have to keep the faulty conversion in arrow slot due to balance we have to also 
	// convert the store count to what you will actually get here, when we start to use the correct reference count of 20 in the conversion we can fall back to using rangedWeapon->GetProjectileStackCount()
	const int compareCount = rangedWeapon ? FMath::FloorToInt((float)rangedWeapon->GetProjectileStackCount() / 30.f * (float)arrowType.getStoreCount()) /*rangedWeapon->GetProjectileStackCount()*/ : 0;

	auto& arrowSlot = GetEquipmentComponent()->GetSlot(arrowType.slotType());
	if ((arrowSlot.GetCount() < compareCount)) {
		arrowSlot.SetCount(0);
		arrowSlot.EquipItem(FInventoryItemData(arrowType.getId(), 1.f), arrowType.getStoreCount());
	}
}

bool APlayerCharacter::ExecuteTaskOnLastSafeNavPositions(std::function<bool(const FVector&)> task) {
	// Loop our last safe ground positions
	for (int i = 0; i < mLastSafePositions.Num(); i++) {
		const auto& safePos = mLastSafePositions[i];
		for (auto point : GetTeleportComponent()->GetNavPositionsAroundPoint(safePos)) {
			if (task(point)) {
				return true;
			}
		}
	}
	return false;
}

void APlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) {
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	const auto movementMode = GetCharacterMovement()->MovementMode;
	const auto customMovementMode = GetCharacterMovement()->CustomMovementMode;

	SetIsWalking(movementMode == EMovementMode::MOVE_Walking || movementMode == EMovementMode::MOVE_NavWalking);

	bool isCustomElytraMode = movementMode == EMovementMode::MOVE_Custom && (customMovementMode == (uint8)ECustomMovementType::Diving || customMovementMode == (uint8)ECustomMovementType::Gliding);

	//Negate fall damage from transition to Elytra movement mode.
	if (FallFromLocation.IsSet() && isCustomElytraMode) {
		FallFromLocation.Reset();
	}
}

void APlayerCharacter::SetIsWalking(bool walking) {
	if(mIsWalking != walking)	{
		mIsWalking = walking;
		RefreshKillzoneCollision();
	}
}

void APlayerCharacter::SetIsOverlappingKillzone(bool overlapping) {
	mIsOverlappingKillzone = overlapping;
	RefreshKillzoneCollision();
}

void APlayerCharacter::OnPushVolume(const FGameplayTag tag, const int32 tagCount) {
	mIsInPushVolume = tagCount > 0;
	SetKillzoneCollisionEnabled(!mIsInPushVolume);
}

void APlayerCharacter::RefreshKillzoneCollision() {
	if (mIsWalking && !mIsOverlappingKillzone && !mIsInPushVolume) {
		SetKillzoneCollisionEnabled(true);
	} else {
		SetKillzoneCollisionEnabled(false);
	}
}

void APlayerCharacter::SetKillzoneCollisionEnabled(bool enabled) {
	if (enabled != mKillzoneCollisionEnabled) {
		mKillzoneCollisionEnabled = enabled;
		GetCapsuleComponent()->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(ECustomTraceChannels::KillzoneBlocker), enabled ? ECollisionResponse::ECR_Block : ECollisionResponse::ECR_Overlap);
	}
}


void APlayerCharacter::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if(OtherComp && (OtherComp->GetCollisionObjectType() == static_cast<ECollisionChannel>(ECustomTraceChannels::KillzoneBlocker))) {
		SetIsOverlappingKillzone(true);
	}
}


void APlayerCharacter::OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherComp && (OtherComp->GetCollisionObjectType() == static_cast<ECollisionChannel>(ECustomTraceChannels::KillzoneBlocker))) {
		SetIsOverlappingKillzone(false);
	}
}



void APlayerCharacter::OnHealthChanged(const FOnAttributeChangeData& data) const {
	FGameplayTagContainer tags;
	if (auto modData = data.GEModData) {
		modData->EffectSpec.GetAllAssetTags(tags);
	}

	OnPlayerHealthChanged.Broadcast(tags);
}

bool APlayerCharacter::IsTimerActive(const FTimerHandle& timerHandle) const {
	return timerHandle.IsValid() && GetWorld()->GetTimerManager().IsTimerActive(timerHandle);
}

void APlayerCharacter::RespawnPlayerMulticast_Implementation() {
	const auto* playerState = Cast<ABasePlayerState>(GetDungeonsBasePlayerState());
	if (playerState) {
		const auto respawnTimeSeconds = playerState->GetRespawnSeconds();
		OnPlayerWillRespawn.Broadcast(respawnTimeSeconds);
		RespawnDelayed(respawnTimeSeconds);	
	}	
}

void APlayerCharacter::ForceQuickRevive()
{
	GetHealthComponent()->Revive();
}

void APlayerCharacter::OnAttackVariantsUpdated() const {
	OnAttackVariantAnimationsAssigned.Broadcast();
}

void APlayerCharacter::OnItemSlotChanged(UItemSlot* itemSlot) const {
	EquipmentDisplayComponent->UpdatePlayerAnimation();
}

void APlayerCharacter::HandleDeath() {
	TryInterruptTeleport();

	if (HasAuthority()) {
		GetEquipmentComponent()->ResetEquipment();
	}

	UDungeonsGameInstance* gi = GetGameInstance<UDungeonsGameInstance>();
	if (IsLocallyControlled() && GetPlayerController()->IsOwnedByInitialLocalPlayer() && gi) {
		if (gi->IsLocalCoop()) {
			const auto& coopCameras = InstanceTracker<APlayerCoopCamera>::GetList(GetWorld());
			if (coopCameras.Num() > 0) {
				mDetachedCameraReference = coopCameras[0]->Camera;
			}
		}
		else {
			mDetachedCameraReference = FindComponentByClass<UCameraComponent>();
		}

		check(mDetachedCameraReference);
		mCameraParentComponent = mDetachedCameraReference->GetAttachParent();
		mDetachedCameraReference->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}

	MakePlayerNonClickable();
	OnPlayerDeath.Broadcast();

	Super::HandleDeath();

	if (IsInDevelopmentLevel()) {
		//When playing in development levels - no game mode is available to handle respawn logic.
		RespawnPlayerMulticast();
	}
}

void APlayerCharacter::HandleDamageTypeReceived(float amount, const FGameplayTag& tag) {
	OnPlayerDamageTypeReceived.Broadcast(tag);
}

void APlayerCharacter::HandleDown() {
	TryInterruptTeleport();
	
	if (!URespawnAsTeamUsingLivesComponent::HasPlayerForcedDownState(GetWorld(), this)) {
		RemoveActiveEffects();
		MakePlayerClickable();
	}

	ApplyDownedPushImmunity();

	OnCharacterDown();
	OnPlayerDown.Broadcast();
}

bool APlayerCharacter::ActivateSupplies_Validate(URefreshSuppliesComponent* component) {
	return true;
}

void APlayerCharacter::ActivateSupplies_Implementation(URefreshSuppliesComponent* component) {
	component->TryDropSuppliesFor(this);
}

void APlayerCharacter::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) {	
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, mLoadedType);
	DOREPLIFETIME(APlayerCharacter, TNTKillCount);
	DOREPLIFETIME(APlayerCharacter, LastOpenedChestRarity);
	DOREPLIFETIME(APlayerCharacter, IceWandKillCount);
	DOREPLIFETIME(APlayerCharacter, OcelotArmourTracking);
	DOREPLIFETIME(APlayerCharacter, IceWeaponTracking);
	DOREPLIFETIME(APlayerCharacter, EmeraldArmourTracking);
	DOREPLIFETIME(APlayerCharacter, SpinBladeHitCount);
	DOREPLIFETIME(APlayerCharacter, TridentKillCount);
	DOREPLIFETIME(APlayerCharacter, ElytraAttackHitCount);
	DOREPLIFETIME(APlayerCharacter, VoidEffectMagnitude);
}

void APlayerCharacter::Tick(const float deltaTime) {
	SCOPE_CYCLE_COUNTER(STAT_APlayerCharacter_Tick_);

	Super::Tick(deltaTime);

	if (Game.IsValid()) {					
		UpdateLastTerrainPos();
	}

	UpdateBlocksRevealed();	

	if (showInventoryFullWarning) {
		fullTextDisplayTime += deltaTime;
	}
}

void APlayerCharacter::SetCapsuleCollisionResponse(ESynchornizedCollisionResponse response) {
	Super::SetCapsuleCollisionResponse(response);
	//Bit of a hack here. We flip the bool and the apply the setting to ensure its reapplied.
	if (response == ESynchornizedCollisionResponse::Default)  {
		mKillzoneCollisionEnabled = !mKillzoneCollisionEnabled;
		SetKillzoneCollisionEnabled(!mKillzoneCollisionEnabled);
	}
}

void APlayerCharacter::NotifyDodgeRollStart(const FVector& dodgeDirection, FPredictionKey key) {
	DungeonsGearUtilLibrary::OnDodgeRollStart(this, dodgeDirection, key);
}

void APlayerCharacter::NotifyDodgeRollEnd(FPredictionKey key) {
	DungeonsGearUtilLibrary::OnDodgeRollEnd(this, key);
}

void APlayerCharacter::UpdateLastTerrainPos() {
	const auto currentTerrainPos = conversion::ueToTerrain(GetActorLocation());
	if (currentTerrainPos != mLastTerrainPos) {
		OnTerrainPosChanged(mLastTerrainPos, currentTerrainPos);
		mLastTerrainPos = currentTerrainPos;
	}
}

void APlayerCharacter::UpdateBlocksRevealed() {
	if (mBlocksRevealedSinceLastTick > 0) {
		BroadcastBlocksRevealed(mBlocksRevealedSinceLastTick);
		mBlocksRevealedSinceLastTick = 0;
	}
}


bool APlayerCharacter::IsIcyBreath() const {
	if (const auto* game = GetGame()) {
		return game->missionDef().IsIcyBreath();
	}
	return false;
}

void APlayerCharacter::Dodge(const FVector& DodgeDirection) {
	if (auto movement = GetPlayerCharacterMovementComponent()) {				
		movement->SetWantsDodgeTo();
		GetController()->SetControlRotation(DodgeDirection.ToOrientationRotator());
		SetSharedCooldown(DodgeCooldownShared, DodgeCooldownShared);
		if(IsLocallyControlled()) CancelAllActions();
	}
}

bool APlayerCharacter::CanDodge() const {
	const auto worldTime = GetWorld()->GetTimeSeconds();
	return GetCanActFromSecondActive() <= worldTime
		&& !IsDodgeOnCooldown()
		&& !IsImmobile()
		&& !IsFrozenSolid()
		&& !GetCharacterMovement()->IsFalling()
		&& IsAllowedToPerformAction();
}

void APlayerCharacter::OnExternalDodgeRollStart(const FVector& dodgeDirection, FPredictionKey key) {
	NotifyDodgeRollStart(dodgeDirection, key);
}

void APlayerCharacter::OnExternalDodgeRollEnd(FPredictionKey key) {
	NotifyDodgeRollEnd(key);
}

void APlayerCharacter::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey key) {
	NotifyDodgeRollStart(dodgeDirection, key);
	OnPlayerDodgeRollingChanged.Broadcast();
}

void APlayerCharacter::OnDodgeRollEnd(FPredictionKey key) {
	NotifyDodgeRollEnd(key);
	OnPlayerDodgeRollingChanged.Broadcast();
}

void APlayerCharacter::SetCurrentCheckpoint(const BlockCuboid& region) {
	if (mCurrentCheckpoint.IsSet()) {
		SetPreviousCheckpoint(mCurrentCheckpoint.GetValue());
	}
	mCurrentCheckpoint = region;
}


void APlayerCharacter::SetPreviousCheckpoint(const BlockCuboid& region) {
	mPreviousCheckpoint = region;
}


void APlayerCharacter::ClearCurrentCheckpoint() {
	if (mCurrentCheckpoint.IsSet()) {
		SetPreviousCheckpoint(mCurrentCheckpoint.GetValue());
	}
	mCurrentCheckpoint.Reset();
}

TOptional<BlockCuboid> APlayerCharacter::GetCurrentCheckpoint() const {
	return mCurrentCheckpoint;
}

TOptional<BlockCuboid> APlayerCharacter::GetPreviousCheckpoint() const {
	return mPreviousCheckpoint;
}

int APlayerCharacter::GetCurrentDungeonInstanceId() const {
	return mCurrentDungeonInstanceId;
}

void APlayerCharacter::OnTerrainPosChanged(const TerrainPos& lastPos, const TerrainPos& newPos) {
	if (Game.IsValid()) {
		if (auto* game = Game->GetGame()) {
			auto* tile = game->tiles().getTile(*this);
			mCurrentDungeonInstanceId = tile ? tile->dungeon().instanceId() : -1;

			if (StatTrackerComponent && tile) {
				StatTrackerComponent->TileChange(tile->meta().id);
			}
		}

		if (const auto* gameState = Cast<ADungeonsGameState>(GetWorld()->GetGameState())) {
			if (gameState->IsLobby()) {
				return;	
			}

			const auto delta = newPos - lastPos;
			if (FMath::Max(FMath::Abs(delta.x), FMath::Abs(delta.y)) <= 1) {
				//Small movement - use delta for optimization
				Game.Get()->TerrainRevealAround(MakeWeakObjectPtr(this), newPos, revealRadiusScale, delta);
			} else {
				//Large movement - full reveal
				Game.Get()->TerrainRevealAround(this, newPos, revealRadiusScale);
			}
		}
	}
}

void APlayerCharacter::UnlockEndersentTiles()
{
	if (IsLocallyControlled())
	{
		bool ownsDLC = false;

		for (auto* player : InstanceTracker<APlayerCharacter>::GetList(GetWorld())) {
			if (player && player->IsLocallyControlled() && player->MissionProgressComponent && player->MissionProgressComponent->IsDLCOwned(EDLCName::TheEnd)) {
				ownsDLC = true;
				break;
			}
		}

		if (auto* serializeComponent = GetCharacterSerializeComponent()) {
			if (ownsDLC) {
				serializeComponent->AddUnlockKey("EndOwner");
			}
			else {
				serializeComponent->RemoveUnlockKey("EndOwner");
			}
		}
	}
}

static AAvatarInfo* sAvatarInfo = nullptr;
static AAvatarInfo* AvatarInfo(UWorld* world) {
	if (!sAvatarInfo)
	{
		sAvatarInfo = actorquery::getFirstActor<AAvatarInfo>(world);
	}
	return sAvatarInfo;
}

FAvatarData APlayerCharacter::GetPlayerAvatarData() const {
	return GetPlayerAvatarComponent()->GetPlayerAvatarData();
}

FColor APlayerCharacter::GetPlayerColor() const {
	return GetPlayerAvatarComponent()->GetPlayerColor();
}

int APlayerCharacter::GetPlayerNumber() const 
{
	if(auto Player = GetDungeonsBasePlayerState())
	{
		return Player->GetPlayerNumber();
	}
	return 0;
}

void APlayerCharacter::ApplyMaterialToMesh(UMaterialInstance* material) {
	GetMesh()->SetMaterial(0, material);
}

void APlayerCharacter::IsOwnedByHostChanged() {
	OnIsOwnedByHostChanged.Broadcast();
}

void APlayerCharacter::PossessedBy(AController* NewController) {
	//We know that pawn assigns the player state from controller in unreal here
	Super::PossessedBy(NewController);
	if(GetDungeonsBasePlayerState() != nullptr){
		OnRep_PlayerState();
	}

	NotifyLocalPlayerJoined();

	if (auto* experienceComponent = FindComponentByClass<UPlayerExperienceComponent>()) {
		experienceComponent->ForceUpdateXP();
	}

	//get rid of camera spring arm from players that dont need it any more.
	if (auto* pPlayerController = Cast<APlayerController>(NewController))
	{
		if (CameraSpringArm && !pPlayerController->IsPrimaryPlayer())
		{
			CameraSpringArm->DestroyComponent();
			CameraSpringArm = nullptr;
		}
	}
}

void APlayerCharacter::UnPossessed() {
	Super::UnPossessed();
	NotifyLocalPlayerLeft();
}

// DG: move to BeginPlay because we are the pawn ~~~
void APlayerCharacter::OnPawnPossessed() {
	EquipmentComponent->OnPawnPossessed();
	OnKilledOther.AddDynamic(KillTrackerComponent, &UKillTrackerComponent::HandleMobKill);
}

void APlayerCharacter::OnLocalPawnPossessed() {
	PlayerAvatarComponent->OnLocalPawnPossessed();
	WalletComponent->OnLocalPawnPossessed();
	ItemStashComponent->OnLocalPawnPossessed();
	if (CosmeticsComponent)
	{
		CosmeticsComponent->OnLocalPawnPossessed();
	}
	MissionProgressComponent->OnLocalPawnPossessed();
}

ABasePlayerState* APlayerCharacter::GetDungeonsBasePlayerState() const {
	return Cast<ABasePlayerState>(GetPlayerState());
}

void APlayerCharacter::OnRep_PlayerState() {
	Super::OnRep_PlayerState();

	
	//Player characters locally controlled on the server are considered host.
	if (auto state = static_cast<ABasePlayerState*>(GetDungeonsBasePlayerState())) {
		//vraket: Im not entirely sure of the paths that a PlayerState get repped on character
		//which is why im doing the AddUniqueDynamic instead of just addDynamic.
		state->OnPlayerIsHostChanged.AddUniqueDynamic( this, &APlayerCharacter::IsOwnedByHostChanged );	
		state->OnPlayerNumberChanged.AddUObject(this, &APlayerCharacter::OnPlayerNumberChangedInternal);
		OnPlayerNumberChangedInternal();

	
		if (GetWorld()->IsServer()) {
			//Only server can control who is considered "host"
			if (GetNetMode() == NM_DedicatedServer) {
				//We are the dedicated server
				//Give host status to "first joining player"
				const auto* game = GetWorld()->GetGameState();
				auto players = game->PlayerArray;
				if (players.Num() > 0 && players[0] == GetDungeonsBasePlayerState()) {
					//This is the first player state
					state->SetOwnedByHost(true);
				}
				else 
				{
					//This is not the first player state
					state->SetOwnedByHost(false);
				}			
			}
			else {
				if (IsLocallyControlled()) {
					//We are controlling this character.
					//Check that we're the first local player index.
					if (auto* playerController = GetPlayerController()) {
						if (playerController->GetLocalPlayerIndex() == 0) {
							//Give host status to ourselves.
							state->SetOwnedByHost(true); //Can only be called by server as well ->	UFUNCTION(Server, ...)
						}
						else {
							//This is not the first local controller, therefore not considered the host.
							state->SetOwnedByHost(false);
						}
					}
					else {
						//Should never occur at this point but adding just in case.
						state->SetOwnedByHost(false);
					}
				}
				else {
					//This is not a host, lets make sure!
					state->SetOwnedByHost(false);
				}
			}
		} else {
			IsOwnedByHostChanged();
		}
	}
	OnPlayerStateChanged.Broadcast();
}

int32 APlayerCharacter::GetPlayerId() const {
	if (ABasePlayerState* pPlayerState = GetDungeonsBasePlayerState()) {
		return pPlayerState->PlayerId;
	}
	return -1;
}

const FText& APlayerCharacter::GetLocalPlayerDisplayText() const {
	if (auto controller = Cast<APlayerControllerBase>(GetController())) {
		return controller->GetLocalPlayerDisplayText();
	}
	return FText::GetEmpty();
}

FString APlayerCharacter::GetBasePlayerDisplayName() const
{
	if (ABasePlayerState* pPlayerState = GetDungeonsBasePlayerState()) {
		return pPlayerState->GetPlayerDisplayName();
	}
	
	return FString();
}

void APlayerCharacter::InitialSetup(ABasePlayerController* playerController)
{
	// bind to aPlayerAction
	if (playerController) {
		playerController->OnAnyPlayerAction.AddUObject(this, &APlayerCharacter::OnPlayerAction);
		TryInitStatTracker(Cast<APlayerController>(playerController)); // D11.SSN
	}
}

bool APlayerCharacter::IsOwnedByHost() const {
	if (const auto* state = Cast<ABasePlayerState>(GetDungeonsBasePlayerState())) {
		return state->OwnedByHost;
	}
	return false;
}

void APlayerCharacter::OnPlayerNumberChangedInternal() {
	const auto playerNumber = GetDungeonsBasePlayerState()->GetPlayerNumber();
	OnPlayerNumberChanged.Broadcast(playerNumber);
	if(auto* avatarComponent = GetPlayerAvatarComponent()){
		avatarComponent->SetPlayerColorByPlayerNumber(playerNumber);
	}
	OnPlayerNumberUpdated();
}

void APlayerCharacter::OnFootstep() 
{
	UDungeonsAssetManager* assetManager = GetWorld()->GetGameInstance<UDungeonsGameInstance>()->GetDungeonsAssetManager();

	// player is submerged in liquid;
	USoundCue* swimSound = nullptr;
	const auto overlapMaterial = GetOverlapMaterial();
	if (overlapMaterial == EMaterialTypeEnum::Lava ||
		overlapMaterial == EMaterialTypeEnum::Water) {
		if (FootstepSoundMap.Contains(overlapMaterial)) {
			swimSound = FootstepSoundMap[overlapMaterial];
		}
	}
		
	// player is walking on block
	USoundCue* stepSound = nullptr;
	const auto stepMaterial = GetStepMaterial();
	if (stepMaterial != EMaterialTypeEnum::Air) {
		if (FootstepSoundMap.Contains(stepMaterial)) {
			stepSound = FootstepSoundMap[stepMaterial];
			LastStepSound = stepSound;
		}
		else {
			UE_LOG(LogDungeonsAudio, Warning, TEXT("Sound map does not contain material %s."), *GetEnumValueToString(stepMaterial));
			stepSound = LastStepSound;
		}
	}
	else {
		stepSound = LastStepSound;
	}

	auto position = GetActorLocation();
	position.Z -= GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	if (FootstepParticleMap.Contains(stepMaterial)) {
		if (auto* stepParticles = FootstepParticleMap[stepMaterial]) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), stepParticles, position);
		}
	}

	if (auto game = GetGame()) {
		if (game->settings().levelName != ELevelNames::Invalid)
		{
			const MissionDef& mission = missions::get(game->settings().levelName);
			if (mission.getRequiredDLC().IsSet())
			{
				EDLCName DLCName = mission.getRequiredDLC().GetValue();
				auto levelDLCRequired = missions::get(game->settings().levelName).getRequiredDLC().GetValue();
				if (DLCExtraFootstepSoundMap.Contains(levelDLCRequired))
				{
					if (auto* DLCSound = DLCExtraFootstepSoundMap[levelDLCRequired]) {
						bool bCanSpawn = CanSpawnExtraFootstep(levelDLCRequired);
						if (bCanSpawn)
						{
							UGameplayStatics::PlaySoundAtLocation(GetWorld(), DLCSound, position);
						}
					}
				}
			}
		}
	}

	// if inside liquid, play it, otherwise do step sound
	if (swimSound) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), swimSound, position);
	}
	else if (stepSound) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), stepSound, position);
	}
}

bool APlayerCharacter::CanSpawnExtraFootstep(EDLCName levelDLCRequired)
{
	return (levelDLCRequired != EDLCName::Oceans) || (levelDLCRequired == EDLCName::Oceans && IsUnderwater());
}

void APlayerCharacter::RespawnDelayed(float seconds) {
	if (HasAuthority()) {
		const auto world = GetWorld();
		world->GetTimerManager().ClearTimer(mPreRespawnDelayHandle);

		const auto spawnAnimationTimeAdjustedTimerDelay = Math::max(0.01f, seconds); //Should never be <= 0 as that will remove the timer.
		world->GetTimerManager().SetTimer(mPreRespawnDelayHandle, this, &APlayerCharacter::PreRespawn, spawnAnimationTimeAdjustedTimerDelay, false);
	}
}

void APlayerCharacter::BeginDestroy()
{
	Super::BeginDestroy();
	
#if !WITH_EDITOR
	//we dont care about this in the editor
	if (!IsTemplate())
	{
		AProjectileActorManager::Purge_Projectiles(this); //purge our projectile cache
	}
#endif
}

bool APlayerCharacter::DissasembleCharacter()
{
	//just count player character as dissassembled for instant destruction
	return true;
}

void APlayerCharacter::RefreshAliveState() {
	const auto NewAliveState = [&]() {
		if (const auto healthComponent = this->GetHealthComponent()) {
			if (healthComponent->IsAlive()) {
				return EAliveState::Alive;
			}

			if (healthComponent->IsDeathPrevented()) {
				return EAliveState::Reviving;
			}					


			if (URespawnAsTeamUsingLivesComponent::HasAnyPlayerForcedDownState(GetWorld())) {
				return EAliveState::Down;
			}

			if (const ADungeonsGameState* gameState = Cast<ADungeonsGameState>(GetWorld()->GetGameState())){
				if (gameState->CanAnyoneReviveMe(this)) {
					return EAliveState::Down;
				}
				return EAliveState::Dead;
			}
		}
		return EAliveState::Dead;
	}();

	SetAliveState(NewAliveState);
}

void EditLocallyControlledCurrentMissionState(APlayerCharacter& player, const std::function<void(FMissionState&, const FLevelSettings&)>& f) {
	if (player.IsLocallyControlled()) {
		if (const auto* gameInstance = player.GetGameInstance<UDungeonsGameInstance>()) {
			const auto& levelSettings = gameInstance->GetLevelSettingsLastStarted();
			if (auto* missionState = player.GetCharacterSerializeComponent()->EditMissionState(levelSettings.getLevelName(), levelSettings.missionState.guid)) {
				f(*missionState, levelSettings);
			}
		}
	}
}

void APlayerCharacter::OnPartsDiscoveredChanged() {
	EditLocallyControlledCurrentMissionState(*this, [this](FMissionState& missionState, const FLevelSettings&) {
		if (const auto* gameState = GetWorld()->GetGameState<ADungeonsGameState>()) {
			missionState.partsDiscovered = gameState->GetPartsDiscovered();
		}
	});
}

void APlayerCharacter::OnLivesLostThisSessionChanged() {
	EditLocallyControlledCurrentMissionState(*this, [this](FMissionState& missionState, const FLevelSettings& levelSettings) {
		if (const auto* gameState = GetWorld()->GetGameState<ADungeonsGameState>()) {
			missionState.livesLost = gameState->GetLivesLostThisSession() + levelSettings.missionState.livesLost;
		}
	});
}

void APlayerCharacter::SetAliveState(const EAliveState newState) {
	if (AliveState != newState) {
		const auto oldState = AliveState;
		if (newState == EAliveState::Down && oldState == EAliveState::Alive) {
			AliveState = newState;
			HandleDown();
		} else if (newState == EAliveState::Dead) {
			AliveState = newState;
			HandleDeath();
		} else if (newState == EAliveState::Alive) {
			AliveState = newState;
			HandleRevive();
		} else if (newState == EAliveState::Reviving) {
			AliveState = newState;
			HandlePreventDeath();
		} else {
			//No change
			return;
		}

		OnPlayerAliveStateChanged.Broadcast(); //Blueprints
		OnAliveStateChanged.Broadcast(); //Internal
		
		UE_LOG(LogTemp, Log, TEXT("Player %s EAliveState changed %s."), *GetName(), *GetEnumValueToString(newState));
	}
}

EAliveState APlayerCharacter::GetAliveState() const {
	return AliveState;
}

void APlayerCharacter::TryInterruptTeleport(const bool force) const {
	auto myTeleportComponent = GetTeleportComponent();

	//D11.SC - this Server_AbortTeleportTo function is causing 2.6ms spikes on the game thread in single player
	if (myTeleportComponent->TeleportState() != ETeleportState::Idle)
	{
		myTeleportComponent->Server_AbortTeleportTo(force);
	}
}

void APlayerCharacter::TeleportToFriend(const APlayerCharacter* targetPlayer) {
	if (!targetPlayer) {
		return;	
	}

	RotatePlayerTowardsLocation(targetPlayer->GetActorLocation());
	if (auto* playerController = GetPlayerController()) {
		GetMovementComponent()->StopMovementImmediately();
		playerController->CancelCurrentInputActions();
	}
	GetTeleportComponent()->Server_TeleportToFriend(targetPlayer);

}

float APlayerCharacter::TeleportPlayerToActor(const APlayerState* targetPlayerState) {
	if (!targetPlayerState) {
		return 0.0f;	
	}

	const auto* targetPlayer = Cast<APlayerCharacter>(targetPlayerState->GetPawn());
	RotatePlayerTowardsLocation(targetPlayer->GetActorLocation());
	if (auto* playerController = GetPlayerController()) {
		GetMovementComponent()->StopMovementImmediately();
		playerController->CancelCurrentInputActions();
	}

	GetTeleportComponent()->Server_TeleportToFriend(targetPlayer);
	
	if (auto* teleportAnimation = GetTeleportComponent()->TeleportAnimation) {
		const auto sectionLength = teleportAnimation->GetSectionLength(teleportAnimation->GetSectionIndex("TeleportFinish"));
		return teleportAnimation->GetPlayLength() - sectionLength;
	}

	return 0.0f;
}

void APlayerCharacter::SetRevealRadiusScale(float scale) {
	revealRadiusScale = scale;
}

void APlayerCharacter::HandleDamageReceived(float amount) {
	mLastCombatTime = GetWorld()->GetTimeSeconds(); // #D11.CM
	Super::HandleDamageReceived(amount);
}

void APlayerCharacter::OnPlayerAction(const ABasePlayerController* playerController) const {
	TryInterruptTeleport();
}

bool APlayerCharacter::IsImmortal() const {
	return mIsImmortal;
}

bool APlayerCharacter::IsAllowedToPerformAction() const {
	static FGameplayTag Stunned = FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Stunned"));
	static FGameplayTag GuardianEye = FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.GuardianEye"));

	if (const auto* abilitySystemComponent = GetAbilitySystemComponent()) {
		const auto stunned = abilitySystemComponent->HasMatchingGameplayTag(Stunned);
		const auto laser = abilitySystemComponent->HasMatchingGameplayTag(GuardianEye);

		return IsAlive()
			&& !stunned
			&& !laser
			&& GetWorldState() == ECharacterWorldState::InWorld
			&& GetTeleportState() != ETeleportState::Flying
			&& GetTeleportState() != ETeleportState::Landing;
	}
	return false;
}

void APlayerCharacter::RespawnQuick() {
	auto gi = GetGameInstance<UDungeonsGameInstance>();

	// Respawn
	if (gi->IsLocalCoop() && gi->IsAnyPlayerInWorld()) {
		// Attempt a coop respawn if we have valid targets.
		if (!TryCoopRespawn()) {
			if (gi->IsAnyPlayerAliveAndInWorld()) {
				if (!GetWorldTimerManager().IsTimerActive(QuickRespawnTimerHandle)) {
					GetWorldTimerManager().SetTimer(QuickRespawnTimerHandle, this, &APlayerCharacter::RespawnQuick, 0.5, true, 1.0f);
				}
				return;
			}
			else {
				// We haven't been able to respawn, and all other players are down so can't move to give us space. Kill us.
				SetWorldState(ECharacterWorldState::InWorld);
				Kill();
				return;
			}
		}
	}
	else {
		GetTeleportComponent()->TeleportAfterDeath();
	}

	SetWorldState(ECharacterWorldState::InWorld);

	if (UGameplayStatics::GetCurrentLevelName(GetWorld()) != "Lobby") {
		const auto healthComponent = FindComponentByClass<UHealthComponent>();
		healthComponent->Revive(healthComponent->GetCurrentHealthPercentage() * .75);
	}

	// Clear our respawn timers.
	GetWorldTimerManager().ClearTimer(mFrozenRespawnTimoutHandle);
	GetWorldTimerManager().ClearTimer(QuickRespawnTimerHandle);
	GetWorldTimerManager().ClearTimer(SafetyTeleportTimerHandle);
}

void APlayerCharacter::RespawnFrozen() {
	// If we bounced from ice to ice, just go through a regular respawn.
	if (GetWorldTimerManager().IsTimerActive(mFrozenRespawnTimoutHandle)) {
		OnRespawnFrozenFailed();
		return;
	}

	if (!IsAlive()) {
		OnRespawnFrozenFailed();
		return;
	}

	// Stop player movement and actions
	if (auto* playerController = GetPlayerController()) {
		playerController->StopMovement();
		playerController->CancelCurrentInputActions();
	}

	auto success = ExecuteTaskOnLastSafeNavPositions([&](const FVector& point) {
			FVector launchVelocity;
			if (UGameplayStatics::SuggestProjectileVelocity(this, launchVelocity, GetActorLocation(), point, 3500.0f, true, 0.f, 0.f, ESuggestProjVelocityTraceOption::DoNotTrace)) {
				// Set us in world.
				SetWorldState(ECharacterWorldState::InWorld);

				// Launch character
				LaunchCharacter(launchVelocity, false, false);

				// Apply frozen solid effect.
				FGameplayEffectSpec spec(Cast<UFrozenSolidGameplayEffect>(UFrozenSolidGameplayEffect::StaticClass()->GetDefaultObject()), AbilitySystem->MakeEffectContext(), 1);
				spec.SetSetByCallerMagnitude(effects::DurationName, 5.f);
				AbilitySystem->ApplyGameplayEffectSpecToSelf(spec);

				// Start a restart timer;
				GetWorldTimerManager().SetTimer(mFrozenRespawnTimoutHandle, this, &APlayerCharacter::OnRespawnFrozenFailed, 10.0f, false);
				return true;
			}
			return false;
		}
	);
	if (!success) {
		// Couldn't launch the character, respawn as normal.
		OnRespawnFrozenFailed();
	}
}

void APlayerCharacter::OnRespawnFrozenFailed() {

	// Clear our respawn timers
	GetWorldTimerManager().ClearTimer(mFrozenRespawnTimoutHandle);
	GetWorldTimerManager().ClearTimer(QuickRespawnTimerHandle);
	GetWorldTimerManager().ClearTimer(SafetyTeleportTimerHandle);

	// Remove our frozen status
	FGameplayTagContainer tag;
	tag.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Immobile.FrozenSolid"));
	AbilitySystem->RemoveActiveEffectsWithTags(tag);

	// Respawn as normal
	RespawnQuick();
	return;
}

// #D11.CM - If we're in local coop, try to pop us to our leader, not the door.
bool APlayerCharacter::TryCoopRespawn() {
	const auto gi = GetGameInstance<UDungeonsGameInstance>();

	if (gi->GetLocalPlayers().Num() > 1) {
		auto getNoLeaderPosition = [&]() {
			for (auto player : InstanceTracker<APlayerCharacter>::GetList(GetWorld())) {
				if (player != this) {
					if (locationquery::isLocationOnSolidBlock(*GetWorld(), player->GetActorLocation() - player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())) {
						return PopToPlayer(player);
					}
				}
			}
			return ExecuteTaskOnLastSafeNavPositions(
				[&](const FVector& point) {
					return PopToLocation(point);
				}
			);
		};

		if (auto* leader = gi->GetPoppingLeader()) {
			if (leader != this && locationquery::isLocationOnSolidBlock(*GetWorld(), leader->GetActorLocation() - leader->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())) {
				return PopToPlayer(leader);
			}
			else {
				return getNoLeaderPosition();
			}
		}
		else {
			return getNoLeaderPosition();
		}
	}

	return false;
}

TArray<FInventoryItemData> APlayerCharacter::GetInventory() const {
	TArray<FInventoryItemData> inventory;
	if (const auto* playerController = GetPlayerController()) {
		if (const auto itemStash = playerController->GetItemStashComponent()) {
			inventory = itemStash->GetAsInventoryDataArray();
		}
	}
	return inventory;
}

bool APlayerCharacter::IsStuck() const {
	return algo::none_of(GetLikelyRelevantDoorLocations(), RETLAMBDA(IsLocationReachable(it))) &&
       algo::none_of(GetNearbySpawnLocations(), RETLAMBDA(IsLocationReachable(it)));
}

TArray<FVector> APlayerCharacter::GetLikelyRelevantDoorLocations() const {
	TArray<FVector> positions;
	
	auto* const game = GetGame();
	const auto& currentTile = game->progress().current(*this);

	positions.Add(game->tiles().getStartPos());

	if (const auto previousDoorPosition = game->tiles().getPreviousDoorPosition(currentTile)) {
		positions.Add(previousDoorPosition.GetValue());
	}

	if (const auto nextDoorPosition = game->tiles().getNextDoorPosition(currentTile)) {
		positions.Add(nextDoorPosition.GetValue());	
	}

	return positions;
}

TArray<FVector> APlayerCharacter::GetNearbySpawnLocations() const {
	auto* const game = GetGame();
	const auto& currentTile = game->progress().current(*this);

	const auto regions = currentTile.tilePlacement().filterRegions(regionpredicates::isSpawn());
	return algo::map_tarray(regions, RETLAMBDA(conversion::posToUe(centerFloor(it.area()))));
}

void APlayerCharacter::NotifyLocalPlayerJoined() {
	for (auto localPlayer : GetGameInstance<UDungeonsGameInstance>()->GetLocalPlayers()) {
		if (auto playerController = localPlayer->GetPlayerController(GetWorld())) {
			if (playerController == GetController()) {
				continue;
			}
			if (auto playerCharacter = Cast<APlayerCharacter>(playerController->GetPawn())) {
				playerCharacter->OnOtherPlayerJoined();
			}
		}
	}
}

void APlayerCharacter::NotifyLocalPlayerLeft() {
	for (auto localPlayer : GetGameInstance<UDungeonsGameInstance>()->GetLocalPlayers()) {
		if (auto playerController = localPlayer->GetPlayerController(GetWorld())) {
			if (playerController == GetController()) {
				continue;
			}
			if (auto playerCharacter = Cast<APlayerCharacter>(localPlayer->GetPlayerController(GetWorld())->GetPawn())) {
				playerCharacter->OnOtherPlayerLeft();
			}
		}
	}
}

void APlayerCharacter::OnOtherPlayerJoined_Implementation() {
}

void APlayerCharacter::OnOtherPlayerLeft_Implementation() {
}

game::Game* APlayerCharacter::GetGame() const {
	return actorquery::getGame(GetWorld());
}

void APlayerCharacter::TeleportToSafety() {
	if (GetGameInstance<UDungeonsGameInstance>()->IsLocalCoop()) {
		if (!TeleportComponent->TeleportToFirstLivingPlayer()) {
			// We're in coop and we might get a player to teleport to, start timer.
			if (!GetWorldTimerManager().IsTimerActive(SafetyTeleportTimerHandle)) {
				GetWorldTimerManager().SetTimer(SafetyTeleportTimerHandle, this, &APlayerCharacter::TeleportToSafety, 0.5f, true, 1.0f);
			}
			return;
		}
	} else if (!TeleportComponent->TeleportToSafety()) {
		UE_LOG(LogDungeons, Warning, TEXT("Unable to teleport player to safety."));
		return;
	}

	// Clear our respawn timers.
	GetWorldTimerManager().ClearTimer(mFrozenRespawnTimoutHandle);
	GetWorldTimerManager().ClearTimer(QuickRespawnTimerHandle);
	GetWorldTimerManager().ClearTimer(SafetyTeleportTimerHandle);

	// Ensure we're in the world.
	SetWorldState(ECharacterWorldState::InWorld);
}

void APlayerCharacter::RestartAtCheckpoint() {

	// Clear our respawn timers.
	GetWorldTimerManager().ClearTimer(mFrozenRespawnTimoutHandle);
	GetWorldTimerManager().ClearTimer(QuickRespawnTimerHandle);
	GetWorldTimerManager().ClearTimer(SafetyTeleportTimerHandle);

	TeleportComponent->Server_TeleportToCheckpoint();
}

void APlayerCharacter::HandlePreventDeath() {
	if (auto* playerController = GetPlayerController()) {	
		playerController->StopMovement();
		playerController->CancelCurrentInputActions();
	}

	if (IsStuck()) {			
		TeleportToSafety();
	}
	
	OnPlayerDeathPrevented.Broadcast();
}

ABasePlayerController* APlayerCharacter::GetPlayerController() const {
	return Cast<ABasePlayerController>(GetController());
}

const FAmbienceIDGroup& APlayerCharacter::GetAmbience() const {
	return mAmbience;
}

void APlayerCharacter::SetAmbience(FAmbienceIDGroup ambience) {
	mAmbience = std::move(ambience);
}

const FAmbienceAudioIDGroup& APlayerCharacter::GetAmbienceAudio() const {
	return mAmbienceAudio;
}

void APlayerCharacter::SetAmbienceAudio(FAmbienceAudioIDGroup ambience) {
	mAmbienceAudio = std::move(ambience);
}

void APlayerCharacter::HandleActiveGameplayEffectAdded(UAbilitySystemComponent* abilitySystem, const FGameplayEffectSpec& spec, FActiveGameplayEffectHandle handle)
{	
	auto effectUiData = spec.Def->UIData;
	if (effectUiData && IsLocallyControlled()) {
		OnPlayerGameplayEffectUiAdded.Broadcast(handle, effectUiData);
		abilitySystem->OnGameplayEffectStackChangeDelegate(handle)->AddUObject(this, &APlayerCharacter::HandleActiveGameplayEffectStackCountChanged);				
	}
}

void APlayerCharacter::HandleActiveGameplayEffectRemoved(const FActiveGameplayEffect& gameplayEffect) {
	const auto effectUiData = gameplayEffect.Spec.Def->UIData;
	if(effectUiData && IsLocallyControlled()) {
		//D11.RR - Avoidable achievement
		if (gameplayEffect.Spec.Def->GetClass() == UVoidBlockGameplayEffect::StaticClass())
		{
			if (GetHealthComponent()->GetCurrentHealth() > 0.0f)
			{
				TryVoidAchievement();
			}
		}

		OnPlayerGameplayEffectUiRemoved.Broadcast(gameplayEffect.Handle, effectUiData);
	}
}

void APlayerCharacter::HandleActiveGameplayEffectStackCountChanged(FActiveGameplayEffectHandle handle, int32 newCount, int32 oldCount) const {
	if (newCount != oldCount){
		if (newCount <= 0) {
			OnPlayerGameplayEffectUiStackCountChanged.Broadcast(handle);			
		} else {
			OnPlayerGameplayEffectUiStackCountChanged.Broadcast(handle);
		}
	}
}

void APlayerCharacter::SetImmortal(const float seconds) {
	mIsImmortal = true;
	if (seconds > 0) {
		GetWorld()->GetTimerManager().SetTimer(mImmortalTimerHandle, this, &APlayerCharacter::RemoveImmortality, 5., false);
	}
}

void APlayerCharacter::RemoveImmortality() {
	mIsImmortal = false;
}

void APlayerCharacter::OutsideWorldBounds() {
	UE_LOG(LogTemp, Error, TEXT("Player falling out of world! This normally destroys actors, but this is likely a bug."));
	ensure(false && "bad bad thing");
}

void APlayerCharacter::FellOutOfWorld(const UDamageType& dmgType) {
	UE_LOG(LogTemp, Error, TEXT("Player falling out of world! This normally destroys actors, but this is likely a bug."));
	Kill();
}

void APlayerCharacter::PreRespawn() {
	if( TeleportComponent->TeleportAfterDeath() ) {
		Respawn();
	} else {
		GetWorld()->GetTimerManager().SetTimer(mRespawnDelayHandle, this, &APlayerCharacter::PreRespawn, 0.1f, false);
		UE_LOG(LogTemp, Error, TEXT("Played failed to teleport when respawning - trying again."));
	}
	SetImmortal(4);
}

void APlayerCharacter::Respawn() {
	if (!HasAuthority()) {
		return;
	}

	HandleResurrection();

	auto* healthComponent = GetHealthComponent();
	if (healthComponent == nullptr || healthComponent->IsAlive()) {
		return;
	}

	healthComponent->OnResurrection();
	healthComponent->Revive();

	GetWorldTimerManager().ClearTimer(mFrozenRespawnTimoutHandle);
	GetWorldTimerManager().ClearTimer(SafetyTeleportTimerHandle);
	GetWorldTimerManager().ClearTimer(QuickRespawnTimerHandle);

	SetActorHiddenInGame(false);

	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

	ApplyInvulnerability();
	ResetArrowCount();

	// #D11.CM - Flush overlaps and re-update
	ClearComponentOverlaps();
	UpdateOverlaps(true);
}

void APlayerCharacter::HandleResurrection() {
	DungeonsGearUtilLibrary::OnAfterResurrection(this);
}

void APlayerCharacter::HandleRevive() {
	if (mDetachedCameraReference) {
		mDetachedCameraReference->AttachToComponent(mCameraParentComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		
		ULovikaSpringArmComponent* springArmComponent = Cast<ULovikaSpringArmComponent>(mCameraParentComponent);
		if (IsValid(mCameraParentComponent) && !mCameraParentComponent->IsPendingKillOrUnreachable() && springArmComponent) {
			springArmComponent->Reinitialize();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("APlayerCharacter::MulticastClientRespawned_Implementation - mCameraParentComponent (%p) is faulty for unknown reasons [isValid: %d, isPendingKillOrUnreachable: %d, didCastSuccessfully %d]"), mCameraParentComponent, IsValid(mCameraParentComponent), springArmComponent != nullptr/*, mCameraParentComponent->IsPendingKillOrUnreachable()*/);
		}
		mDetachedCameraReference = nullptr;
		mCameraParentComponent = nullptr;
	}

	MakePlayerNonClickable();

	RemoveDownedPushImmunity();

	OnPlayerRevive.Broadcast();

	// #D11.CM - Flush overlaps and re-update
	ClearComponentOverlaps();
	UpdateOverlaps(true);

	Super::HandleRevive();
}

void APlayerCharacter::SteppedOnNewBlock(const FullBlock& block) {
	Super::SteppedOnNewBlock(block);

	if (HasAuthority()) {
		GetGameMode()->OnActorSteppedOnNewBlock(this, BlockGraphicsHelper::getBlock(GetWorld(), *this, block.id).getMaterialType());
	}
}

UEquipmentComponent* APlayerCharacter::GetEquipmentComponent() const {
	return EquipmentComponent;
}

UEquipmentDisplayComponent* APlayerCharacter::GetEquipmentDisplayComponent() const {
	return EquipmentDisplayComponent;
}

UTeleportComponent* APlayerCharacter::GetTeleportComponent() const {
	return TeleportComponent;
}

UPlayerAvatarComponent* APlayerCharacter::GetPlayerAvatarComponent() const {
	return PlayerAvatarComponent;
}

class UPlayerCharacterMovementComponent* APlayerCharacter::GetPlayerCharacterMovementComponent() const{
	return Cast<UPlayerCharacterMovementComponent>(GetCharacterMovement());
}

class ULovikaSpringArmComponent* APlayerCharacter::GetCameraSpringArm() const{
	return CameraSpringArm;
}

class ULovikaSpringArmComponent* APlayerCharacter::GetLocalCoopCameraSpringArm() const {
	return LMPCameraSpringArm;
}

UCharacterSerializeComponent* APlayerCharacter::GetCharacterSerializeComponent() const {
	return CharacterLazySaveComponent;
}

UCosmeticsComponent* APlayerCharacter::GetCosmeticsComponent() const {
	return CosmeticsComponent;
}

UCosmeticsDisplayComponent* APlayerCharacter::GetCosmeticsDisplayComponent() const {
	return CosmeticsDisplayComponent;
}

UMissionProgressComponent* APlayerCharacter::GetMissionProgressComponent() const {
	return MissionProgressComponent;
}

UWalletComponent* APlayerCharacter::GetWalletComponent() const {
	return WalletComponent;
}

UItemStashComponent* APlayerCharacter::GetItemStashComponent() const {
	return ItemStashComponent;
}

UPickupItemComponent* APlayerCharacter::GetPickupItemComponent() const {
	return PickupItemComponent;
}

class UOxygenComponent* APlayerCharacter::GetOxygenComponent() const {
	return OxygenComponent;
}

UClientEventHubComponent* APlayerCharacter::GetClientEventHubComponent() const {
	return ClientEventHubComponent;
}

UAdventureHubComponent* APlayerCharacter::GetAdventureHubComponent() const {
	return AdventureHubComponent;
}

ETeleportState APlayerCharacter::GetTeleportState() const {
	return GetTeleportComponent()->TeleportState();
}

UElytraComponent* APlayerCharacter::GetElytraComponent() const {
	return ElytraComponent;
}

void APlayerCharacter::ServerlaunchPlayer_Implementation(FVector velocity) {
	GetMovementComponent()->StopMovementImmediately();
	TeleportTo(GetActorLocation() + FVector(0, 0, 30), GetActorRotation());
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	GetCharacterMovement()->Launch(velocity);
}

bool APlayerCharacter::ServerlaunchPlayer_Validate(FVector velocity) {
	return true;
}

bool APlayerCharacter::IsDodgeOnCooldown() const {
	static FGameplayTag CoolddownTag = FGameplayTag::RequestGameplayTag(DodgeRechargeName);
	return !HasDodgesLeft() && GetAbilitySystemComponent()->HasMatchingGameplayTag(CoolddownTag);
}

bool APlayerCharacter::IsDodgeRolling() const{
	return GetPlayerCharacterMovementComponent()->IsDodging();
}

bool APlayerCharacter::HasDodgesLeft() const
{
	static FGameplayTag MultiDodge = FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.MultiDodge"));
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(MultiDodge) && GetAbilitySystemComponent()->GetNumericAttribute(UMovementAttributeSet::DodgeChargesAttribute()) > 0;
}

int32 APlayerCharacter::NumDodgesLeft() const
{
	static FGameplayTag MultiDodge = FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.MultiDodge"));
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(MultiDodge))
	{
		//num dodges left attribute
		return (int32)GetAbilitySystemComponent()->GetNumericAttribute(UMovementAttributeSet::DodgeChargesAttribute());
	}

	//1 or 0 depending on cooldown
	static FGameplayTag CoolddownTag = FGameplayTag::RequestGameplayTag(DodgeRechargeName);
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(CoolddownTag) ? 0 : 1;	
}

bool APlayerCharacter::IsOnGround() {
	switch (GetStepMaterial())
	{
	case EMaterialTypeEnum::Air:
	case EMaterialTypeEnum::Fire:
	case EMaterialTypeEnum::Lava:
	case EMaterialTypeEnum::Water:
		return false;
	default:
		return true;
	}
}

float APlayerCharacter::GetDodgeCooldownFraction() const {
	return GetPlayerCharacterMovementComponent()->GetDodgeCooldownFraction();
}

UEffectBasedCooldownProvider* APlayerCharacter::GetDodgeCooldownProvider() const {
	return GetPlayerCharacterMovementComponent()->GetDodgeCooldownProvider();
}

void APlayerCharacter::OnDodgeCooldownTagChange(const FGameplayTag tag, const int32 tagCount) {
	OnPlayerDodgeRollingCooldownChanged.Broadcast(tagCount > 0);

	if (HasDodgesLeft())
	{
		GetPlayerCharacterMovementComponent()->ApplyDodgeCooldown(GetAbilitySystemComponent());
	}
}

void APlayerCharacter::OnMultiDodgeTagChange(const FGameplayTag tag, const int32 tagCount)
{
	OnPlayerMultiDodgeChanged.Broadcast(tagCount > 0);
}

void APlayerCharacter::OnRep_LoadedType() {
	if (IsLoadedInLevel()) {
		OnPlayerLoadedInLevel.Broadcast();
	}
}

void APlayerCharacter::TryEquipElytraUponLevelLoad()
{
	if (HasAuthority())
	{
		bool equipElytra = GetGame()->missionDef().ShouldStartWithElytra();

		if (!equipElytra)
		{
			auto& characterList = InstanceTracker<APlayerCharacter>::GetList(GetWorld());
			for (auto character : characterList)
			{
				if (character->GetElytraComponent()->IsEquipped())
				{
					equipElytra = true;
					break;
				}
			}
		}

		if (equipElytra)
		{
			ElytraComponent->Equip();
		}
	}
}

void APlayerCharacter::PlayerLoadedInLevel(bool introReady) {
	TryEquipElytraUponLevelLoad();
	mLoadedType = introReady ? APlayerCharacterInternal_ELoadedType::LoadedIntroReady : APlayerCharacterInternal_ELoadedType::LoadedHot;
	GetPlayerController()->PlayerLoadedInLevel();
	OnPlayerLoadedInLevel.Broadcast();
}

bool APlayerCharacter::IsLoadedInLevel() const {
	return mLoadedType != APlayerCharacterInternal_ELoadedType::NotLoaded;
}

bool APlayerCharacter::IsAllowedToSeeIntro() const {
	return mLoadedType == APlayerCharacterInternal_ELoadedType::LoadedIntroReady;
}

void APlayerCharacter::RotatePlayerTowardsActor(AActor* target) {
	if (target == nullptr) return;
	RotatePlayerTowardsLocation(target->GetActorLocation());
}

bool APlayerCharacter::IsRotatedTowardsActor(AActor* target, float degreeTolerance) const {
	return IsRotatedTowardsLocation(target->GetActorLocation(), degreeTolerance);
}

bool APlayerCharacter::IsRotatedTowardsLocation(const FVector & targetLocation, float degreeTolerance) const
{
	auto pawnLocation = GetActorLocation();
	pawnLocation.Z = targetLocation.Z;

	const auto targetRot = UKismetMathLibrary::FindLookAtRotation(pawnLocation, targetLocation);
	const auto myRot = GetActorRotation();

	const auto delta = FMath::FindDeltaAngleDegrees(myRot.Yaw, targetRot.Yaw);

	if (FMath::Abs(delta) <= degreeTolerance) {
		//We have arrived within our tolerance
		return true;
	}

	//Working on it.
	return false;
}

void APlayerCharacter::RotatePlayerTowardsAttackTarget(AActor * attacktarget) {
	if (attacktarget == nullptr) return;

	FVector targetLocation = attacktarget->GetActorLocation();
	if (const auto *baseChar = Cast<ABaseCharacter>(attacktarget)) {
		if (const auto *capsule = baseChar->GetClosestTargetableCapsule(GetActorLocation())) {
			targetLocation = capsule->GetComponentLocation();
		}
	}

	RotatePlayerTowardsLocation(targetLocation);
}

bool APlayerCharacter::IsRotatedTowardsAttackTarget(AActor * attacktarget, float degreeTolerance) const {
	FVector targetLocation = attacktarget->GetActorLocation();
	if (const auto *baseChar = Cast<ABaseCharacter>(attacktarget)) {
		if (const auto *capsule = baseChar->GetClosestTargetableCapsule(GetActorLocation())) {
			targetLocation = capsule->GetComponentLocation();
		}
	}

	return IsRotatedTowardsLocation(targetLocation, degreeTolerance);
}

void APlayerCharacter::RotatePlayerTowardsLocation(const FVector& location) const {
	auto pawnLocation = GetActorLocation();
	pawnLocation.Z = location.Z;
	const auto rot = UKismetMathLibrary::FindLookAtRotation(pawnLocation, location);
	RotatePlayer(rot);
}

void APlayerCharacter::RotatePlayer(const FRotator& rotator) const {
	if (auto* playerController = GetPlayerController()) {
		playerController->SetControlRotation(rotator);
		if (HasAuthority()) { // need to force client update if rotating a playerCharacter on server side
			playerController->ClientSetRotation(rotator);
		}
	}
}

ADungeonsGameMode* APlayerCharacter::GetGameMode() const {
	return Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode());
}

void APlayerCharacter::OnStunned(const FGameplayTag tag, const int32 tagCount) {
	if (tagCount) {
		if (HasAuthority() || IsLocallyControlled()) {
			if (!GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Levitation"))))
			{
				GetMovementComponent()->StopMovementImmediately();
			}
			TArray<UAttackComponent*> attackComponents;
			GetComponents(attackComponents);

			for (auto component : attackComponents) {
				component->Stop();
			}
			if (HasAuthority()) {
				TryInterruptTeleport();

				if (auto reviveComponent = FindComponentByClass<UReviveComponent>()) {
					reviveComponent->ServerCancelRevive();
				}

				auto abilitySystem = GetAbilitySystemComponent();
				const FGameplayTagContainer tags(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Stunned")));
				const auto query = FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(tags);
				const auto activeEffects = abilitySystem->GetActiveEffects(query);
			
				auto max = algo::max_element_by(activeEffects, [&](const FActiveGameplayEffectHandle& h) {
					return abilitySystem->GetActiveGameplayEffect(h)->GetDuration();
				});

				// #D11.CM - If we don't already have stun immunity, give us a base line stun immunity
				if (abilitySystem->HasAnyMatchingGameplayTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("Immunity.Stun")))) {
					auto spec = effects::CreateGameplayEffectSpec<UTemporaryStunimmunityGameplayEffect>(abilitySystem, 1.f);
					spec.SetSetByCallerMagnitude(FName("Duration"), (max ? abilitySystem->GetActiveGameplayEffect(max.GetValue())->GetDuration() : 0.f) + 2.f);
					abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
				}

			}

			auto controller = Cast<ABasePlayerController>(GetController());
			if (controller && IsLocallyControlled()) {
				controller->CancelCurrentInputActions();
			}
		}

		if (StunnedMontage) {
			PlayMontage(StunnedMontage);
		}
	}
	else if (StunnedMontage) {
		StopAnimMontage(StunnedMontage);
	}
}

TArray<ECustomTraceChannels> APlayerCharacter::GetNearMissSelectionChannels() {
	auto channels = Super::GetNearMissSelectionChannels();
	channels.Add(ECustomTraceChannels::IgnorePlayer);
	return channels;
}

void APlayerCharacter::OnPlayerTeleported() {
	if (IsLocallyControlled()) {
		OnLocalPlayerTeleported();		
	}

	if(HasAuthority())
	{
		ElytraComponent->SetIsFlying(false);
	}

	if (auto controller = GetPlayerController()) {
		controller->OnPawnTeleported();
	}
	OnPlayerTeleportedInternal.Broadcast();

	//Waiting for game to tick when we have teleported possibly not enough to have correct ambience.
	if (auto* const game = GetGame()) {
		game->updateAmbience();
	}

}

TArray<APlayerCharacter*> APlayerCharacter::GetTeleportCandidates() const
{
	return GetTeleportComponent()->GetTeleportCandidates();
}

TArray<APlayerCharacter*> APlayerCharacter::GetTeleportDependents() const
{
	return GetTeleportComponent()->GetTeleportDependents();
}


void APlayerCharacter::MakePlayerClickable() {
	SetNearMissSelection(ECR_Block);
	InteractableComponent->EnableInteraction();
	GetMesh()->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(SkeletalMeshChannel), ECR_Overlap);

}

void APlayerCharacter::MakePlayerNonClickable() const {
	ResetNearMissSelection();
	InteractableComponent->DisableInteraction();
	GetMesh()->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(SkeletalMeshChannel), CachedCollisionResponse);
}

void APlayerCharacter::ApplyDownedPushImmunity() {
	FGameplayEffectSpec pushResistantSpec(Cast<UWindImmunityGameplayEffect>(UWindImmunityGameplayEffect::StaticClass()->GetDefaultObject()), AbilitySystem->MakeEffectContext(), 1);
	DownedPushImmunityHandle = AbilitySystem->ApplyGameplayEffectSpecToSelf(pushResistantSpec);
}

void APlayerCharacter::RemoveDownedPushImmunity() {
	AbilitySystem->RemoveActiveGameplayEffect(DownedPushImmunityHandle);
}

void APlayerCharacter::OnLevitate(const FGameplayTag tag, const int32 tagCount)
{
	Super::OnLevitate(tag,tagCount);
	
	if (tagCount > 0)
	{
		if (auto* tracker = GetStatTracker())
		{
			tracker->ShulkerHit(true);
		}
		else
		{
			Client_OnLevitate();
		}
	}
}

void APlayerCharacter::Client_OnLevitate_Implementation()
{
	if (auto* tracker = GetStatTracker())
	{
		tracker->ShulkerHit(true);
	}
}

void APlayerCharacter::SetIsInInventory(const bool inInventory) {
	bInInventory = inInventory;
}

bool APlayerCharacter::IsInInventory() const {
	return bInInventory;
}

const game::DifficultyRecommendation& APlayerCharacter::GetDifficultyRecommendation() const {
	return GetEquipmentComponent()->GetDifficultyRecommendation();
}

bool APlayerCharacter::Server_IsReadyToPlay() const {
	return bIsReadyToPlay;
}

void APlayerCharacter::SetReadyToPlay() {
	if (!bIsReadyToPlay) {
		bIsReadyToPlay = true;
		SetReadyToPlayInternal();
	}
}

bool APlayerCharacter::SetReadyToPlayInternal_Validate() {
	return true;
}

void APlayerCharacter::SetReadyToPlayInternal_Implementation() {
	bIsReadyToPlay = true;
}

void APlayerCharacter::GameplayCue_Damage() {
	OnNormalDamageReceived.Broadcast();
}

void APlayerCharacter::GameplayCue_Damage_Weak() {
	OnWeakDamageReceived.Broadcast();
}

void APlayerCharacter::HandleInteraction(ACharacter* interactor) {
	//We're ping ponging the components here because we need to handle owning connections to RPC the server.

	//D11.KS - Just incase they attempt to interact with themselves, pretty sure this is impossible.
	if (interactor == static_cast<ACharacter*>(this)) return;

	//D11.KS - Don't even both attempting to revive alive players.
	if (Cast<ABaseCharacter>(this)->IsNotAlive())
	{
		if (auto instigatorReviveComponent = interactor->FindComponentByClass<UReviveComponent>())
		{
			instigatorReviveComponent->Revive(this);
		}
	}
}

// D11.SSN
UStatTrackerComponent* APlayerCharacter::GetStatTracker() const {
	return StatTrackerComponent;
}

// D11.SSN
void APlayerCharacter::TryInitStatTracker(APlayerController* playerController) {
	if (!StatTrackerComponent) {
		StatTrackerComponent = NewObject<UStatTrackerComponent>(this);
		if (!StatTrackerComponent->TryInit(playerController)) {
			StatTrackerComponent->DestroyComponent();
			StatTrackerComponent = nullptr;
		}
		else {
			StatTrackerComponent->OcelotArmour(OcelotArmourTracking);
			StatTrackerComponent->IceWeapon(IceWeaponTracking);
			StatTrackerComponent->EmeraldArmour(EmeraldArmourTracking);
			if(auto* gi = Cast<UDungeonsGameInstance>(GetGameInstance()))
				StatTrackerComponent->StartLevel(gi->Configuration.GetLevelName());
		}
	}
}

bool APlayerCharacter::HasPendingRewardItem() const {
	if (const auto* characterSerializeComponent = GetCharacterSerializeComponent()) {
		return characterSerializeComponent->HasPendingRewardItem();
	}
	return false;
}

int APlayerCharacter::NumPendingRewards() const
{
	if (const auto* characterSerializeComponent = GetCharacterSerializeComponent()) {
		return characterSerializeComponent->NumPendingRewards();
	}
	return 0;
}

void APlayerCharacter::ClaimPendingRewardItem() const {
	if (auto* characterSerializeComponent = GetCharacterSerializeComponent()) {
		if (const auto* pickupItemComp = FindComponentByClass<UPickupItemComponent>()) {
			const auto& item = characterSerializeComponent->PeekPendingReward();
			if (pickupItemComp->CanPickup(item.ItemData)) {
				if (const auto claimedItem = characterSerializeComponent->ClaimPendingRewardItem()) {
					pickupItemComp->Pickup(claimedItem->ItemData);
				}
			}
		}
	}
}

void APlayerCharacter::SalvagePendingRewardItem() const {
	if (auto* characterSerializeComponent = GetCharacterSerializeComponent()) {
		if (ItemStashComponent && characterSerializeComponent->HasPendingRewardItem()) {
			if (const auto item = characterSerializeComponent->ClaimPendingRewardItem()) {
				ItemStashComponent->SalvageItemData(item->ItemData);
			}
		}
	}
}

FRewardData APlayerCharacter::GetPendingRewardItem() const {
	if (const auto* characterSerializeComponent = GetCharacterSerializeComponent()) {
		if (characterSerializeComponent->HasPendingRewardItem()) {
			return characterSerializeComponent->PeekPendingReward();
		}
	}
	return {};
}

ERewardType APlayerCharacter::GetPendingRewardItemRewardType() const
{
	return GetPendingRewardItem().RewardType;
}

// D11.SSN - start timer to display full inventory warning in local co-op
void APlayerCharacter::SetInventoryFullWarning() {
	showInventoryFullWarning = true;
	if (IsTimerActive(fullTimerHandle)) {
		GetWorld()->GetTimerManager().ClearTimer(fullTimerHandle);
	}
	GetWorld()->GetTimerManager().SetTimer(fullTimerHandle, this, &APlayerCharacter::UnsetInventoryFullWarning, 1.0f, false);
}

void APlayerCharacter::UnsetInventoryFullWarning() {
	showInventoryFullWarning = false;
	fullTextDisplayTime = 0.0f;
	GetWorld()->GetTimerManager().ClearTimer(fullTimerHandle);
}

void APlayerCharacter::UpdateSafePositions() {
	if (InWorldAndAlive()) {
		if (IsOnGround() && !IsDodgeRolling()) {

			if (GetWorldTimerManager().IsTimerActive(mFrozenRespawnTimoutHandle)) {
				GetWorldTimerManager().ClearTimer(mFrozenRespawnTimoutHandle);
			}

			FNavLocation out;
			auto navSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			if (navSystem->ProjectPointToNavigation(GetActorLocation(), out, FVector{ 10, 10, 125 })) {

				// Add to the beginning of the array
				mLastSafePositions.Add(out.Location);
			}
		}
	}

}

TOptional<FString> APlayerCharacter::GetOnlineUserId() const {
	if (const auto* playerController = GetPlayerController()) {
		if (const auto identityInterface = online::getIdentityInterface()) {
			if (auto NetId = playerController->GetUniqueNetId()) {
				if (const auto uniquePlayerId = identityInterface->GetUniquePlayerId(playerController->GetLocalPlayerIndex(), NetId->GetType())) {
					return uniquePlayerId->ToString();
				}
			}
		}
	}
	return TOptional<FString>();
}

// D11.SSN
void APlayerCharacter::OnRep_TNTKillCount() {
	if (auto* tracker = GetStatTracker()) {
		tracker->TNT(TNTKillCount);
	}
}

// D11.SSN
void APlayerCharacter::OnRep_OpenedChestRarity() {
	if (auto* tracker = GetStatTracker()) {
		tracker->OpenFancyChest(LastOpenedChestRarity);
	}
}

// D11.SSN
void APlayerCharacter::OnRep_IceWandKillCount() {
	if (auto* tracker = GetStatTracker()) {
		tracker->IceWand(IceWandKillCount);
	}
}

// D11.SSN
void APlayerCharacter::OnRep_OcelotArmourTracking() {
	if (auto* tracker = GetStatTracker()) {
		tracker->OcelotArmour(OcelotArmourTracking);
	}
}

// D11.SSN
void APlayerCharacter::OnRep_IceWeaponTracking() {
	if (auto* tracker = GetStatTracker()) {
		tracker->IceWeapon(IceWeaponTracking);
	}
}

// D11.SSN
void APlayerCharacter::OnRep_EmeraldArmourTracking() {
	if (auto* tracker = GetStatTracker()) {
		tracker->EmeraldArmour(EmeraldArmourTracking);
	}
}

// D11.SSN
void APlayerCharacter::OnRep_SpinBladeHitCount() {
	if (auto* tracker = GetStatTracker()) {
		tracker->SpinWheel(SpinBladeHitCount);
	}
}

// D11.SSN
void APlayerCharacter::OnRep_TridentKillCount() {
	if (auto* tracker = GetStatTracker()) {
		tracker->Trident(TridentKillCount);
	}
}

void APlayerCharacter::OnRep_ElytraAttackHitCount()
{
	if (auto* tracker = GetStatTracker())
	{
		tracker->ElytraAttack(ElytraAttackHitCount);
	}
}

void APlayerCharacter::OnRep_VoidEffectMagnitude()
{
}

void APlayerCharacter::ClientUnlockBonusMission_Implementation(ELevelNames levelName) {
	EGameDifficulty difficulty = EGameDifficulty::Difficulty_1;

	if (const auto* game = actorquery::getGame(GetWorld())) {
		difficulty = game->settings().difficulty.chosen();
	}

	if (!MissionProgressComponent->IsMissionUnlocked(difficulty, levelName)) {
		MissionProgressComponent->UnlockBonusMission(levelName);
		OnMissionUnlocked.Broadcast(levelName);
	}
}

void APlayerCharacter::ClientUnlockMerchant_Implementation(TSubclassOf<UMerchantDef> merchantDef) {
	check(IsLocallyControlled() && "trying to unlock a merchant on a character which not locally controlled.");
	if (!IsLocallyControlled()) {
		return;
	}

	if (auto serializer = GetCharacterSerializeComponent()) {
		if (!serializer->HasUnlockedMerchant(merchantDef)) {
			serializer->UnlockMerchant(merchantDef);
			OnMerchantUnlocked.Broadcast(merchantDef);
		}
	}
}

void APlayerCharacter::OnLlamaOffering() {
	if (UStatTrackerComponent* tracker = GetStatTracker()) {
		tracker->LlamaOffering();
	}
	else {
		Client_OnLlamaOffering();
	}
}

void APlayerCharacter::Client_OnLlamaOffering_Implementation() {
	if (UStatTrackerComponent* tracker = GetStatTracker()) {
		tracker->LlamaOffering();
	}
}

void APlayerCharacter::OnIceWandGlowSquid() {
	if (UStatTrackerComponent* tracker = GetStatTracker()) {
		tracker->GlowSquid();
	}
	else {
		Client_OnIceWandGlowSquid();
	}
}

void APlayerCharacter::Client_OnIceWandGlowSquid_Implementation() {
	if (UStatTrackerComponent* tracker = GetStatTracker()) {
		tracker->GlowSquid();
	}
}

void APlayerCharacter::OnEnterBubbleColumn(bool entered) {
	if (UStatTrackerComponent* tracker = GetStatTracker()) {
		tracker->BubbleColumn(entered);
	}
	else {
		Client_OnEnterBubbleColumn(entered);
	}
}

void APlayerCharacter::Client_OnEnterBubbleColumn_Implementation(bool entered) {
	if (UStatTrackerComponent* tracker = GetStatTracker()) {
		tracker->BubbleColumn(entered);
	}
}

void APlayerCharacter::OnBubbledEnemy(bool bubbled) {
	if (UStatTrackerComponent* tracker = GetStatTracker()) {
		tracker->Bubbled(bubbled);
	}
	else {
		Client_OnBubbledEnemy(bubbled);
	}
}

void APlayerCharacter::Client_OnBubbledEnemy_Implementation(bool bubbled) {
	if (UStatTrackerComponent* tracker = GetStatTracker()) {
		tracker->Bubbled(bubbled);
	}
}

void APlayerCharacter::OnLostConduit() {
	if (UStatTrackerComponent* tracker = GetStatTracker()) {
		tracker->LostConduit();
	}
	else {
		Client_OnLostConduit();
	}
}

void APlayerCharacter::Client_OnLostConduit_Implementation() {
	if (UStatTrackerComponent* tracker = GetStatTracker()) {
		tracker->LostConduit();
	}
}

void APlayerCharacter::OnSquideasterEggActive()
{
	if (UStatTrackerComponent* tracker = GetStatTracker()) {
		tracker->SquidEasterEgg();
	}
	else
	{
		Client_OnSquidEasterEggActive();
	}
}

void APlayerCharacter::Client_OnSquidEasterEggActive_Implementation()
{
	if (UStatTrackerComponent* tracker = GetStatTracker()) {
		tracker->SquidEasterEgg();
	}
}

void APlayerCharacter::TryVoidAchievement()
{
	if (UStatTrackerComponent* tracker = GetStatTracker())
	{
		tracker->Voided(VoidEffectMagnitude);
	}
	else
	{
		Client_TryVoidAchievement();
	}
}

void APlayerCharacter::Client_TryVoidAchievement_Implementation()
{
	if (UStatTrackerComponent* tracker = GetStatTracker())
	{
		tracker->Voided(VoidEffectMagnitude);
	}
}