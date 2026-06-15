#include "Dungeons.h"
#include "ArmorProperty.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "Engine/AssetManager.h"
#include "Assets/ArmorPropertyAssetFinder.h"
#include "game/ArmorProperties/ArmorPropertyTypeDefs.h"
#include "ArmorPropertyType.h"

UArmorProperty::UArmorProperty() {
	bReplicates = true;
}

void UArmorProperty::BroadcastArmorPropertyTriggeredEvent_Implementation() const
{
	if (auto baseCharacter = Cast<ABaseCharacter>(GetOwner())) {
		if (auto playerController = Cast<ABasePlayerController>(baseCharacter->GetController())) {
			playerController->ReceiveOnArmorPropertyTriggered(TypeID);
		}
	}
}

void UArmorProperty::BeginPlay() {
	Super::BeginPlay();
}

void UArmorProperty::SetItemPower(float ItemPower)
{
	SourceItemPower = ItemPower;
}

FFormatOrderedArguments UArmorProperty::createFormattedDisplayValueString() const {
	FFormatOrderedArguments args;
	args.Emplace(FText::FromString(createFormattedValueString()));
	return args;
}

EArmorPropertyID UArmorProperty::GetTypeID() const {
	return TypeID;
}

const FPropertyColorPrio& UArmorProperty::GetPropertyColorPrio() const {
	return PropertyColorPrio;
}

ABaseCharacter* UArmorProperty::GetOwnerCharacter() const {
	return Cast<ABaseCharacter>(GetOwner());
}

TArray< UTexture2D* > UArmorProperty::s_IconTextures;
TArray< UMaterialInstance* > UArmorProperty::s_IconMaterials;
TArray< UClass* > UArmorProperty::s_ArmorPropertyClasses;

template <typename T> T* RootAndCast(const TOptional<FSoftObjectPath>& path) {
	if (path) {
		if (T* obj = Cast<T>(path->ResolveObject())) {
			obj->AddToRoot();
			return obj;
		}
	}

	return nullptr;
}

void UArmorProperty::PreloadIconTextures()
{
	s_IconTextures.SetNumZeroed((int)EEnchantmentTypeID::Last);
	s_IconMaterials.SetNumZeroed((int)EEnchantmentTypeID::Last);
	s_ArmorPropertyClasses.SetNumZeroed((int)EEnchantmentTypeID::Last);

	TArray<FSoftObjectPath> assets = IDungeonsModule::Get().GetArmorPropertyAssetFinder()->GetAssets();

	UAssetManager::GetStreamableManager().RequestAsyncLoad(assets, []() {
		const UArmorPropertyAssetFinder* assetFinder = IDungeonsModule::Get().GetArmorPropertyAssetFinder();
		for (const auto& armorproperty : game::armorproperties::type::getArmorProperties()) {
			EArmorPropertyID id = armorproperty.getId();
			int index = (int)id;

			s_ArmorPropertyClasses[index] = RootAndCast<UClass>(assetFinder->GetClassPath(id));
			s_IconTextures[index] = RootAndCast<UTexture2D>(assetFinder->GetIconPath(id));
			s_IconMaterials[index] = RootAndCast<UMaterialInstance>(assetFinder->GetMaterialPath(id));
		}
	}, FStreamableManager::AsyncLoadHighPriority, false, false, TEXT("PreloadArmorProperties"));
}

UTexture2D* UArmorProperty::GetIconTextureForArmorPropertyType(UObject* ref, EArmorPropertyID type)
{
	return s_IconTextures[(int)type];
}

UMaterialInstance* UArmorProperty::GetIconMaterialInstanceForArmorPropertyType(UObject* ref, EArmorPropertyID type)
{
	return s_IconMaterials[(int)type];
}

FText UArmorProperty::GetNameForArmorPropertyType(EArmorPropertyID type)
{
	return game::armorproperties::type::getArmorPropertyType(type).getDisplayName();
}
