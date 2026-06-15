#include "Dungeons.h"
#include "Enchantment.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "EnchantmentUtil.h"
#include "UnrealNetwork.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/component/EnchantmentComponent.h"
#include "Async.h"
#include "Engine/AssetManager.h"
#include "Assets/EnchantmentAssetFinder.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/SoulGatheringGameplayEffect.h"
#include "game/item/instance/AItemInstance.h"
#include "util/Algo.h"

//#pragma optimize("",off)

UEnchantment::UEnchantment() {
	bReplicates = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnchantment::SetLevel(int level) {
	Level = level;
}

int UEnchantment::GetLevel() const {
	return Level;
}

const EnchantmentType& UEnchantment::GetType() const {
	return game::enchantment::type::getEnchantmentType(TypeId);
}

void UEnchantment::Start() {
	auto owner = GetCharacterOwner();
	if (owner && owner->IsAlive()) {
		OnStart();
	}
}

void UEnchantment::End() {
	auto owner = GetCharacterOwner();
	if (owner && owner->IsAlive()) {
		OnEnd();
	}
}

bool UEnchantment::ShouldTriggerOnProjectile(const ABaseProjectile* Projectile) const
{
	return (GetType().isAlwaysEnchantable()) || (Projectile ? Projectile->IsEnchanted() : false);
}

void UEnchantment::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		if (!Source) {
			Source = GetOwner();
		}

		if (auto enchantmentComponent = GetOwner()->FindComponentByClass<UEnchantmentComponent>()) {
			enchantmentComponent->TryRegisterEnchantment(this);
		}

		if (const auto soulGatherCount = GetType().getSoulGatherCount()) {
			auto owner = GetCharacterOwner();
			auto abilitySystem = owner->GetAbilitySystemComponent();
			FGameplayEffectSpec soulSpec = effects::CreateGameplayEffectSpec<USoulGatheringGameplayEffect>(abilitySystem);
			soulSpec.GetContext().AddSourceObject(this);
			soulSpec.GetContext().AddInstigator(owner, Source);
			soulSpec.SetSetByCallerMagnitude(USoulGatheringGameplayEffect::SoulGatheringMagnitude, soulGatherCount);
			SoulGatheringHandle = abilitySystem->ApplyGameplayEffectSpecToSelf(soulSpec);
		}
	}
}

void UEnchantment::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	if (GetOwnerRole() == ROLE_Authority) {

		if (SoulGatheringHandle.IsValid()) {
			GetCharacterOwner()->GetAbilitySystemComponent()->RemoveActiveGameplayEffect(SoulGatheringHandle);
		}

		if (auto enchantmentComponent = GetOwner()->FindComponentByClass<UEnchantmentComponent>()) {
			enchantmentComponent->TryUnregisterEnchantment(this);
		}
	}
}

void UEnchantment::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEnchantment, Level);
	DOREPLIFETIME(UEnchantment, EnchantmentSource);
	DOREPLIFETIME(UEnchantment, OwnerItemRarity);
}

float UEnchantment::GetSourceItemPower() const {
	if (const auto* item = Cast<AItemInstance>(GetSource())) {
		return item->GetItemPower();
	}

	return 1.f;
}

bool UEnchantment::IsSourceItemMelee() const {
	auto source = Cast<AItemInstance>(Source);
	return source ? source->GetItemType().getSlotType() == ESlotType::MeleeWeapon : true;
}
bool UEnchantment::IsSourceItemRanged() const {
	auto source = Cast<AItemInstance>(Source);
	return source ? source->GetItemType().getSlotType() == ESlotType::RangedWeapon : true;
}
bool UEnchantment::IsSourceItemArmor() const {
	auto source = Cast<AItemInstance>(Source);
	return source ? source->GetItemType().getSlotType() == ESlotType::Armor : true;
}

ABaseCharacter* UEnchantment::GetCharacterOwner() const {
	return Cast<ABaseCharacter>(GetOwner());
}

bool UEnchantment::IsOwnerMob() const {
	return GetOwner()->IsA<AMobCharacter>();
}
void UEnchantment::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEnchantment::BroadcastEnchantmentTriggeredEvent_Implementation(bool canCombo) const {
	if (auto baseCharacter = Cast<ABaseCharacter>(GetOwner())) {
		if (auto playerController = Cast<ABasePlayerController>(baseCharacter->GetController())) {
			playerController->ReceiveOnEnchantmentTriggered(TypeId, canCombo);
		}
	}
}

TArray< UTexture2D* > UEnchantment::s_IconTextures;
TArray< UMaterialInstance* > UEnchantment::s_IconMaterials;
TArray< UClass* > UEnchantment::s_EnchantmentClasses;

template <typename T> T* RootAndCast(const TOptional<FSoftObjectPath>& path) {
	if (path) {
		if (T* obj = Cast<T>(path->ResolveObject())) {
			obj->AddToRoot();
			return obj;
		}
	}

	return nullptr;
}

void UEnchantment::PreloadIconTextures()
{
	s_IconTextures.SetNumZeroed((int)EEnchantmentTypeID::Last);
	s_IconMaterials.SetNumZeroed((int)EEnchantmentTypeID::Last);
	s_EnchantmentClasses.SetNumZeroed((int)EEnchantmentTypeID::Last);

	TArray<FSoftObjectPath> assets = IDungeonsModule::Get().GetEnchantmentAssetFinder()->GetAssets();

	UAssetManager::GetStreamableManager().RequestAsyncLoad(assets, []() {
		const UEnchantmentAssetFinder* assetFinder = IDungeonsModule::Get().GetEnchantmentAssetFinder();
		for (const auto& enchantment : game::enchantment::type::getAvailableEnchantments()) {
			EEnchantmentTypeID id = enchantment.getEnchantmentTypeID();
			int index = (int)id;

			s_EnchantmentClasses[index] = RootAndCast<UClass>(assetFinder->GetClassPath(id));
			s_IconTextures[index] = RootAndCast<UTexture2D>(assetFinder->GetIconPath(id));
			s_IconMaterials[index] = RootAndCast<UMaterialInstance>(assetFinder->GetMaterialPath(id));
		}
	}, FStreamableManager::AsyncLoadHighPriority, false, false, TEXT("PreloadEnchantments"));
}

UTexture2D* UEnchantment::GetIconTextureForEnchantmentType(UObject* ref, EEnchantmentTypeID type)
{
	return s_IconTextures[(int)type];
}

UMaterialInstance* UEnchantment::GetIconMaterialInstanceForEnchantmentType(UObject* ref, EEnchantmentTypeID type)
{

	return s_IconMaterials[(int)type];
}

FText UEnchantment::GetNameForEnchantmentType(EEnchantmentTypeID type) {
	return game::enchantment::type::getEnchantmentType(type).getDisplayName();
}

FText UEnchantment::GetDescriptionForEnchantmentType(EEnchantmentTypeID type) {

	if (auto* enchantmentClass = GetEnchantmentTypeClass(type)) {
		if (const auto defaultObject = Cast<UEnchantment>(enchantmentClass->GetDefaultObject())) {
			return defaultObject->CreateDescription();
		}
	}

	const auto enchType = game::enchantment::type::getEnchantmentType(type);
	return enchType.getDescription();
}

TArray<FItemBulletPoint> UEnchantment::GetDisplayBulletPointsTextForEnchantmentType(EEnchantmentTypeID type) {
	TArray<FItemBulletPoint> DisplayBullets;

	int soulGatherCount = game::enchantment::type::getEnchantmentType(type).getSoulGatherCount();
	if (soulGatherCount > 0) {		
		DisplayBullets.Add(FItemBulletPoint::CreateSoulGatherBulletPoint(soulGatherCount));
	}

	return DisplayBullets;
}

UClass* UEnchantment::GetEnchantmentTypeClass(EEnchantmentTypeID type) {
	if (s_EnchantmentClasses[(int)type])
	{
		return s_EnchantmentClasses[(int)type];
	}

	//none for unset
	if (type == EEnchantmentTypeID::Unset)
	{
		return nullptr;
	}

	const auto enchType = game::enchantment::type::getEnchantmentType(type);

	if (const auto path = IDungeonsModule::Get().GetEnchantmentAssetFinder()->GetClassPath(type)) {
		//use same asset manager as async in case its already doing it
		UAssetManager::GetStreamableManager().RequestSyncLoad(path.GetValue());
		UObject* pResolvedObj = path->ResolveObject();
		s_EnchantmentClasses[(int)type] = Cast<UClass>(pResolvedObj);

		if (s_EnchantmentClasses[(int)type])
		{
			s_EnchantmentClasses[(int)type]->AddToRoot();
		}
		else
		{
			//fall back to this
			s_EnchantmentClasses[(int)type] = StaticLoadClass(UEnchantment::StaticClass(), nullptr, *path->ToString());
		}
	}

	return s_EnchantmentClasses[(int)type];
}

FText UEnchantment::GetLevelEffectDescriptionForEnchantmentType(EEnchantmentTypeID type, int level, float itemPower)
{	
	if (level <= 0) {
		return FText::GetEmpty();
	}

	if (auto* enchantmentClass = GetEnchantmentTypeClass(type))
	{
		if (const auto defaultObject = Cast<UEnchantment>(enchantmentClass->GetDefaultObject())) {
			const auto enchType = game::enchantment::type::getEnchantmentType(type);
			const auto multiplier = defaultObject->CreateFormattedLevelEffectMultiplier(level, itemPower);
			return FText::Format(defaultObject->GetType().getLevelEffectTemplate(level), FText::FromString(multiplier));
		}
	}
	
	return FText::GetEmpty();	
}

int32 UEnchantment::GetUpgradeCostForEnchantmentTypeLevel(EEnchantmentTypeID type, int32 level, bool isNetherite) {
	auto enchantmentType = game::enchantment::type::getEnchantmentType(type);
	/*Current enchantment costs design:
			Common	Powerful
		Lv1	1		2		+ netherite ? 1
		Lv2	2		3		+ netherite ? 1
		Lv3	3		4		+ netherite ? 1
	*/

	return level + 
		   (enchantmentType.isRarity(EEnchantmentRarity::Powerful) ? 1 : 0) + 
		   (isNetherite ? 1 : 0);
}

bool UEnchantment::GetCanBeUsedByMobsForEnchantmentType(EEnchantmentTypeID type) {
	return game::enchantment::type::getEnchantmentType(type).canBeUsedByMobs();
}

bool UEnchantment::GetHasTagForEnchantmentType(EEnchantmentTypeID type, EEnchantmentTag tag) {
	return game::enchantment::type::getEnchantmentType(type).hasTag(tag);
}

bool UEnchantment::GetIsRarityForEnchantmentType(EEnchantmentTypeID type, EEnchantmentRarity rarity) {
	return game::enchantment::type::getEnchantmentType(type).isRarity(rarity);
}

EEnchantmentRarity UEnchantment::GetEnchantmentTypeRarity(EEnchantmentTypeID type) {
	return game::enchantment::type::getEnchantmentType(type).getRarity();
}

EEnchantmentCategory UEnchantment::GetEnchantmentTypeCategory(EEnchantmentTypeID type) {
	return game::enchantment::type::getEnchantmentType(type).getEnchantmentCategory();
}

FString UEnchantment::CreateFormattedLevelEffectMultiplier(int forLevel, float forItemPower) const {
	const float levelMultiplier = LevelMultiplier(forLevel);
	const float powerMultiplier = ItemPowerMultiplier(forItemPower);
	return MultiplierFormatter(levelMultiplier * powerMultiplier);
}

FText UEnchantment::CreateDescription() const{
	return game::enchantment::type::getEnchantmentType(TypeId).getDescription();
}

UAbilitySystemComponent* UEnchantment::GetAbilitySystemComponent() const {
	if (auto character = GetCharacterOwner()) {
		return character->GetAbilitySystemComponent();
	}
	
	return nullptr;
}

//#pragma optimize("",on)
