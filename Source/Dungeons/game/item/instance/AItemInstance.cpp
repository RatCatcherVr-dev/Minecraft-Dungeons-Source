#include "Dungeons.h"
#include "AItemInstance.h"
#include "game/item/ItemType.h"
#include "DungeonsGameInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <Net/UnrealNetwork.h>
#include "DungeonsGameMode.h"
#include <AbilitySystemComponent.h>
#include "game/component/SoulComponent.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include <AbilitySystemGlobals.h>
#include "game/Enchantments/Enchantment.h"
#include "game/item/power/ItemPowerEffect.h"
#include "game/actor/item/GearActor.h"
#include "game/component/EquipmentDisplayComponent.h"
#include "game/component/RangedAttackComponent.h"
#include <GameFramework/GameStateBase.h>
#include "ClientEventHub.h"
#include "game/abilities/effects/SoulGatheringGameplayEffect.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/util/DungeonsGearUtilLibrary.h"
#include "online/seasons/LiveOps.h"
#include "online/sessions/OnlineUtil.h"

AItemInstance::AItemInstance() {
	bReplicates = true;
	bAlwaysRelevant = true;
	bOnlyRelevantToOwner = false;
	NetUpdateFrequency = 1.0f;
	CooldownProvider = UDummyCooldownProvider::StaticClass()->GetDefaultObject<UDummyCooldownProvider>();
}

void AItemInstance::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		OnSetupWithValidOwner();
	}
}

void AItemInstance::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
}

const ItemType& AItemInstance::GetItemType() const {
	return GetItemRegistry().Get(ItemId);
}

void AItemInstance::SetItemPower(float itemPower) {
	ItemPower = itemPower;
	OnItemPowerChanged.Broadcast();	
}

float AItemInstance::GetItemPower() const {
	return ItemPower;
}

bool AItemInstance::IsAutomaticDestructible()
{
	return bAutomaticDestruction;
}

const FSerializableItemId& AItemInstance::GetItemId() const {
	return ItemId;
}

const FSerializableItemId& AItemInstance::GetAlternativeItemId() const
{
	return GetItemId();
}

int AItemInstance::GetDisplayCount() const {
	// Return -1 to use the slot's display count instead of the item instance'sK
	return -1;
}

bool AItemInstance::CanActivate() const {
	return !CooldownProvider->IsOnCooldown() && GetSoulActivationCost() <= GetSoulComponent()->GetSoulCount();
}

bool AItemInstance::IsBusy() const {
	return false;
}

void AItemInstance::OnTriedToActivate() const {
	if (auto owner = GetOwner()) {
		if (GetSoulActivationCost() > 0 && GetSoulActivationCost() > GetSoulComponent()->GetSoulCount()){
			if (auto enchantmentComponent = owner->FindComponentByClass<USoulComponent>()) {
				enchantmentComponent->TriggerSoulsNeededHint(GetSoulActivationCost());
			}
		}
	}
}

void AItemInstance::OnRep_Owner() {
	Super::OnRep_Owner();
	OnSetupWithValidOwner();
}

void AItemInstance::SetCooldownProvider(ICooldownProvider* provider)
{
	CooldownProvider = provider;
	OnCooldownProviderAssigned(CooldownProvider);
}

void AItemInstance::ForceActivate()
{
	if(HasAuthority())
	{
		ICooldownProvider* current = CooldownProvider;
		CooldownProvider = UDummyCooldownProvider::StaticClass()->GetDefaultObject<UDummyCooldownProvider>();
		TryActivate();
		SetCooldownProvider(current);
	}
}

float AItemInstance::GetSoulActivationCost() const {
	const auto& itemType = GetItemType();
	return itemType.getSoulCost();	
}

int AItemInstance::GetSoulGatherCount() const {
	auto baseAmount = GetItemType().getSoulGatherCount();

	if (auto enchantmentComponent = GetOwner()->FindComponentByClass<UEnchantmentComponent>()) {
		for (auto* enchantment : enchantmentComponent->GetEnchantmentsBySource(this)) {
			baseAmount += enchantment->GetType().getSoulGatherCount();
		}
	}

	return baseAmount;
}

void AItemInstance::OnSetupWithValidOwner() {
	ensure(GetOwner() != nullptr);
	if (auto meleeAttackComponent = GetOwner()->FindComponentByClass<UMeleeAttackComponent>()) {
		meleeAttackComponent->OnPlayerMeleeAttack.AddUObject(this, &AItemInstance::OnPlayerMeleeAttack);
	}
	if (auto rangedAttackComponent = GetOwner()->FindComponentByClass<URangedAttackComponent>()) {
		rangedAttackComponent->OnPlayerBeginRangedAttack.AddUObject(this, &AItemInstance::OnPlayerBeginRangedAttack);
	}
	if (auto ownerPlayer = Cast<APlayerCharacter>(GetOwner())) {
		if (ownerPlayer->GetController() != nullptr) {
			OnOwnerAndOwnerControllerReplicated();
		}
		else {
			ownerPlayer->OnControllerReplicated.AddUObject(this, &AItemInstance::OnOwnerControllerReplicated);
		}
	}
}

void AItemInstance::OnOwnerAndOwnerControllerReplicated() {
}

UItemPowerEffect* AItemInstance::GetPowerEffect(int32 index /*= 0*/) const
{
	//checkf(PowerEffects.IsValidIndex(index), "index out of bounds for power effect");
	return Cast<UItemPowerEffect>(PowerEffects[index]->GetDefaultObject());	
}

void AItemInstance::Activate(const FPredictionKey& predictionKey) { 
	auto characterOwner = GetCharacterOwner();
	
	if (!bHasManualCooldownActivation) {
		CooldownProvider->TriggerCooldown(CalculateCooldown(), predictionKey);
	}

	if(SharedPassiveCooldown > 0.f && characterOwner->IsLocallyControlled()) {
		characterOwner->SetSharedCooldown(0.f, SharedPassiveCooldown);
		characterOwner->CancelAllActions();	
	}
	

	if (ItemActorClass) {
		HandleItemActorSpawn();
	}

	if (HasAuthority()) {
		if (ActivationAnimation) {
			if (ItemActorClass) {
				MulticastHandleItemActorSpawn();
			}
			if(UAnimMontage* montage = Cast<UAnimMontage>(ActivationAnimation)) {
				characterOwner->MulticastPlayMontage(montage, 1, EMontagePlayReturnType::Duration, 0, predictionKey);
			} else {
				characterOwner->MulticastPlayAnimationAsDynamicMontage(ActivationAnimation, ActivationAnimationSlot, 0, 0.2f, 1.0f, 1, 0, 0, predictionKey);	
			}
		}

		auto gameMode = Cast<ADungeonsGameMode>(GetWorld()->GetAuthGameMode());
		gameMode->OnActorUsedItem(GetOwner(), this);
		
		if(!bHasManualSoulConsumption) {
			if (const auto soulCoust = GetSoulActivationCost()) {
				GetSoulComponent()->ServerAddSouls(-soulCoust);
			}
		}
	} else if(ActivationAnimation) {
		if(UAnimMontage* montage = Cast<UAnimMontage>(ActivationAnimation)) {
			characterOwner->PlayMontage(montage);
		} else {
			characterOwner->PlayAnimationAsDynamicMontage(ActivationAnimation, ActivationAnimationSlot);
		}
	}

	OnActivated();

	OnItemActivationCompleted.Broadcast(true);

	if (!bCanFail) {
		ActivationSucceeded(predictionKey);
	}

	if (const auto playerOwner = Cast<APlayerCharacter>(GetOwner())) {
		playerOwner->GetClientEventHubComponent()->UseItem(ItemId);
	}
}

void AItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);	
	DOREPLIFETIME(AItemInstance, ItemPower);
}


void AItemInstance::ActivateServer_Implementation(FPredictionKey predictionKey) {
	UAbilitySystemComponent* abilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	check(abilitySystem && "Item owned by character without abilitySystemComponent.");

	FScopedPredictionWindow window(abilitySystem, predictionKey, !bManualPredictionHandling);
	Activate(predictionKey);
}

bool AItemInstance::ActivateServer_Validate(FPredictionKey predictionKey) {
	return true;
}

bool AItemInstance::TryActivate() {
	if (CooldownProvider->IsOnCooldown()) { return false; }

	OnItemActivationPending.Broadcast(true);

	STAT(FScopeCycleCounter scopeCounter(GetItemType().activateInstanceStatId));

	UAbilitySystemComponent* abilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();
	check(abilitySystem && "Item owned by character without abilitySystemComponent.");
	FScopedPredictionWindow window(abilitySystem, bUsePrediction);

	if (Role != ROLE_Authority) {
		ActivateServer(abilitySystem->ScopedPredictionKey);
	}
	Activate(abilitySystem->ScopedPredictionKey);

	return true;
}

USoundCue* AItemInstance::GetInventoryDropSoundForItemId(UObject* WorldContextObject, const FSerializableItemId& itemId) {
	auto* instance = WorldContextObject->GetWorld()->GetGameInstance<UDungeonsGameInstance>();
	return instance->GetInventoryItemDataHolder()->GetInventoryDropSound(itemId);
}

void AItemInstance::ResetInstance() {
	
}

float AItemInstance::CalculateCooldown() const {
	float cooldownMultiplier = 1.0f;

	if (const auto owner = GetCharacterOwner()) {
		if (const auto abilitySystemComponent = owner->GetAbilitySystemComponent()) {
			bool isPotion = GetItemRegistry().Get(ItemId).hasTag(ItemTag::HealthPotion);
			cooldownMultiplier = abilitySystemComponent->GetNumericAttribute(isPotion ? UItemAttributeSet::PotionCooldownMultiplierAttribute() : UItemAttributeSet::ItemCooldownMultiplierAttribute());
		}
	}

	return GetItemType().getCoolDownSeconds() * cooldownMultiplier;
}

USoulComponent* AItemInstance::GetSoulComponent() const {
	return GetOwner()->FindComponentByClass<USoulComponent>();
}

APlayerCharacter* AItemInstance::GetPlayerOwner() const {
	return Cast<APlayerCharacter>(GetOwner());
}

ABaseCharacter* AItemInstance::GetCharacterOwner() const{
	return Cast<ABaseCharacter>(GetOwner());
}

UEquipmentDisplayComponent* AItemInstance::GetOwnerEquipmentDisplayComponent() const {
	return GetPlayerOwner()->GetEquipmentDisplayComponent();
}

void AItemInstance::Serialize(FArchive& Ar) {
	if(Ar.IsSaving()) {
		ItemIdName = ItemId.GetSerializedId();
	}

	Super::Serialize(Ar);
}

#if WITH_EDITOR
void AItemInstance::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
	const UProperty* PropertyThatChanged = PropertyChangedEvent.Property;

	if (PropertyThatChanged && PropertyThatChanged->GetFName() == GET_MEMBER_NAME_CHECKED(AItemInstance, ItemId)) {
		ItemIdName = ItemId.GetSerializedId();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AItemInstance::OnOwnerControllerReplicated() {
	if (auto ownerPlayer = Cast<APlayerCharacter>(GetOwner())) {
		OnOwnerAndOwnerControllerReplicated();
	}
}

void AItemInstance::MulticastHandleItemActorSpawn_Implementation() {
	HandleItemActorSpawn();
}

void AItemInstance::MulticastHandleItemActorRemoval_Implementation() {
	HandleItemActorRemoval();
}

void AItemInstance::ServerHandleItemActorRemoval_Implementation() {
	MulticastHandleItemActorRemoval();
}
bool AItemInstance::ServerHandleItemActorRemoval_Validate() {
	return true;
}

void AItemInstance::HandleItemActorSpawn() {
	RemoveItemActor();
	SpawnItemActor();

	GetWorld()->GetTimerManager().SetTimer(RemoveItemActorTimerHandle, this, &AItemInstance::HandleItemActorRemoval, ActivationAnimation->GetPlayLength());

	if (auto* equipmentDisplayComponent = GetOwnerEquipmentDisplayComponent()) {
		equipmentDisplayComponent->HideWeapons();
	}
}

void AItemInstance::HandleItemActorRemoval() const {
	RemoveItemActor();

	if (auto* equipmentDisplayComponent = GetOwnerEquipmentDisplayComponent()) {
		equipmentDisplayComponent->RestoreWeaponVisibility();
	}

	if (ItemActor.IsValid()) {
		ItemActor->Destroy();
	}
}

void AItemInstance::SpawnItemActor() {
	if (ItemActor.IsValid()) {
		ItemActor->Destroy();
	}

	if (const auto* equipmentDisplayComponent = GetOwnerEquipmentDisplayComponent()) {
		if (auto* playerMesh = equipmentDisplayComponent->GetPlayerMesh()) {
			ItemActor = AItemActor::SpawnItemActor(GetWorld(), ItemActorClass);
			ItemActor->Paperdoll->AttachToPlayerMesh(playerMesh);
			ItemActor->SetVisibility(true);
		}
	}
}

void AItemInstance::RemoveItemActor() const {
	if (const auto* equipmentDisplayComponent = GetOwnerEquipmentDisplayComponent()) {
		if (auto* playerMesh = equipmentDisplayComponent->GetPlayerMesh()) {
			TArray<USceneComponent*> childComponents;
			playerMesh->GetChildrenComponents(true, childComponents);

			for (auto child : childComponents) {
				if (child->ComponentTags.Contains("ActivePermanent")) {
					FDetachmentTransformRules rules(EDetachmentRule::KeepRelative, false);
					child->DetachFromComponent(rules);
					child->DestroyComponent();
				}
			}
		}
	}
}

void AItemInstance::OnPlayerMeleeAttack(const APlayerCharacter* player) {
	RemoveItemActorFromLocal();
}

void AItemInstance::OnPlayerBeginRangedAttack(const APlayerCharacter* player) {
	RemoveItemActorFromLocal();
}

void AItemInstance::RemoveItemActorFromLocal() {
	if (ItemActor.IsValid()) {
		if (HasAuthority()) {
			MulticastHandleItemActorRemoval();
		}
		else {
			ServerHandleItemActorRemoval();
			HandleItemActorRemoval();
		}
	}
}

TArray<FText> AItemInstance::CreatePowerEffectDescription(const ABaseCharacter& owner, float power) const {
	TArray<FText> effectTexts;

	if (PowerEffects.Num() > 0) {
		for (auto&& PowerEffect : PowerEffects) {
			const UItemPowerEffect* effect = PowerEffect->GetDefaultObject<UItemPowerEffect>();
			if (!effect->IsHiddenFromPlayer()) {
				auto absoluteText = effect->CreateAbsoluteText(owner, this, power);
				if (!absoluteText.IsEmpty()) {
					//use absolute if exists
					effectTexts.Add(absoluteText);
				} else {
					//fallback to relative
					effectTexts.Add(effect->CreateText(power));
				}
			}
		}
	}

	return effectTexts;
}

void AItemInstance::PopulateBulletPoints(TArray<FItemBulletPoint>& intoList, const ABaseCharacter& owner) const {}


float AItemInstance::GetStats(EItemStats stat) const {	
	return -1.0f; //No default instance class stats
}

void AItemInstance::ApplyEquippedEffects() {
	if (HasAuthority()) {
		if (EffectHandles.Num() > 0) {
			//Make sure we dont have two effects on top of each other.
			RemoveEquippedEffects();
		}
		if (auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner())) {
			for (auto&& PowerEffect : PowerEffects) {
				if (PowerEffect.GetDefaultObject()->GameplayEffect) {
					const UItemPowerEffect* effect = PowerEffect->GetDefaultObject<UItemPowerEffect>();
					EffectHandles.Add(abilitySystem->ApplyGameplayEffectToSelf(effect->GameplayEffect.GetDefaultObject(), ItemPower, abilitySystem->MakeEffectContext()));
				}
			}

			if (const auto soulGatherCount = GetItemType().getSoulGatherCount()) {
				FGameplayEffectSpec soulSpec = effects::CreateGameplayEffectSpec<USoulGatheringGameplayEffect>(abilitySystem);
				soulSpec.SetSetByCallerMagnitude(USoulGatheringGameplayEffect::SoulGatheringMagnitude, soulGatherCount);
				soulSpec.GetContext().AddSourceObject(this);
				soulSpec.GetContext().AddInstigator(GetOwner(), this);
				EffectHandles.Add(abilitySystem->ApplyGameplayEffectSpecToSelf(soulSpec));
			}
		}
	}
}

void AItemInstance::RemoveEquippedEffects() {
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());

	if (HasAuthority() && abilitySystem) {
		//Owning character can have been destroyed when the gearitem is destroyed.
		if (EffectHandles.Num() > 0) {
			for(auto&& EffectHandle : EffectHandles){
				abilitySystem->RemoveActiveGameplayEffect(EffectHandle);
				EffectHandle.Invalidate();
			}
			EffectHandles.Reset();
		}
	}
	bEffectsApplied = false;
}

void AItemInstance::ActivateEquippedEffects() {
	ApplyEquippedEffects();
}

void AItemInstance::DeactivateEquippedEffects() {
	RemoveEquippedEffects();
}

void AItemInstance::ActivationSucceeded(FPredictionKey key) {
	OnItemSuccess.Broadcast();
	DungeonsGearUtilLibrary::OnItemActivated(GetCharacterOwner(), this, key);
}