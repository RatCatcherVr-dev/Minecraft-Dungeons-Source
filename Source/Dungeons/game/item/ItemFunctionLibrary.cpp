#include "ItemFunctionLibrary.h"
#include "game/item/ItemTypeDefs.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Assets/ItemAssetFinder.h"
#include "Dungeons.h"

TArray< UTexture2D* > UItemFunctionLibrary::s_PreloadGearIconTextures;

FSerializableItemId UItemFunctionLibrary::MakeItemId(FSerializableItemId inId)
{
	return inId;
}

FName UItemFunctionLibrary::BreakItemId(const FSerializableItemId& inId)
{
	return static_cast<FItemId>(inId).GetBackingType();
}

bool UItemFunctionLibrary::EqualEqual_ItemTypeID(const FSerializableItemId& A, const FSerializableItemId& B) {
	return static_cast<FItemId>(A) == static_cast<FItemId>(B);
}

bool UItemFunctionLibrary::NotEqual_ItemTypeID(const FSerializableItemId& A, const FSerializableItemId& B) {
	return static_cast<FItemId>(A) != static_cast<FItemId>(B);
}

bool UItemFunctionLibrary::GetIsUniqueForItemType(const FSerializableItemId& type) {
	return GetItemRegistry().Get(type).isUnique();
}

UTexture2D* UItemFunctionLibrary::GetIconTextureForItemType(const FSerializableItemId& type) {
	return GetTextureHelper(IDungeonsModule::Get().GetItemAssetFinder()->IconPath(type));
}

UTexture2D* UItemFunctionLibrary::GetIconTextureForItemTypeHigh(const FSerializableItemId& type) {
	return GetTextureHelper(IDungeonsModule::Get().GetItemAssetFinder()->IconPathHigh(type));
}


UTexture2D* UItemFunctionLibrary::StreamIconTextureForItemIdHigh(const FSerializableItemId& type, bool blockingLoad) {
	if (blockingLoad)
	{
		return UItemFunctionLibrary::GetIconTextureForItemTypeHigh(type);
	}

	if (auto SoftPath = IDungeonsModule::Get().GetItemAssetFinder()->IconPathHigh(type))
	{
		TSharedPtr<FStreamableHandle> AssetStreamHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftPath.GetValue(),
			[]() {}
			, FStreamableManager::AsyncLoadHighPriority,
			true,
			false,
			TEXT("StreamIconTextureForItemTypeHigh"));

		if (AssetStreamHandle->HasLoadCompleted())
		{
			UTexture2D* LoadedTexture = Cast< UTexture2D >(AssetStreamHandle->GetLoadedAsset());

			UAssetManager::GetStreamableManager().Unload(SoftPath.GetValue()); //release all handles for this asset

			return LoadedTexture;
		}
	}

	return nullptr;
}

UTexture2D* UItemFunctionLibrary::GetAmmoIconSmallTextureForItemType(const FSerializableItemId& type) {
	return GetTextureHelper(IDungeonsModule::Get().GetItemAssetFinder()->AmmoIconPathSmall(type));
}

UTexture2D* UItemFunctionLibrary::GetGearIconTextureForItemType(const FSerializableItemId& type) {
	if (auto gearIcon = GetPreloadGearIconTextureInternal(type)) {
		return gearIcon;
	}

	if (const auto* parent = GetItemRegistry().Get(type).getParent()) {
		return GetPreloadGearIconTextureInternal(parent->getId());
	}

	return nullptr;
}

bool UItemFunctionLibrary::GetIsSoulUseForItemType(const FSerializableItemId& type) {
	return GetItemRegistry().Get(type).isSoulUseItem();
}

bool UItemFunctionLibrary::GetIsSoulGatherForItemType(const FSerializableItemId& type) {
	return GetItemRegistry().Get(type).isSoulGatherItem();
}

ESlotType UItemFunctionLibrary::GetSlotTypeForItemType(const FSerializableItemId& type) {
	return GetItemRegistry().Get(type).getSlotType();
}

bool UItemFunctionLibrary::GetIsGearForItemType(const FSerializableItemId& type) {
	return GetItemRegistry().Get(type).isGear();
}

FText UItemFunctionLibrary::GetNameForItemType(const FSerializableItemId& type) {
	return GetItemRegistry().Get(type).getNameText();
}

FText UItemFunctionLibrary::GetDescriptionForItemType(const FSerializableItemId& type) {
	return GetItemRegistry().Get(type).getDescriptionText();
}

float UItemFunctionLibrary::GetDefaultDurationForItemType(const FSerializableItemId& type) {
	return GetItemRegistry().Get(type).getDurationSeconds();
}

float UItemFunctionLibrary::GetDefaultCooldownForItemType(const FSerializableItemId& type) {
	return GetItemRegistry().Get(type).getCoolDownSeconds();
}

bool UItemFunctionLibrary::GetHasCooldownForItemType(const FSerializableItemId& type) {
	return GetItemRegistry().Get(type).hasCooldown();
}

ItemTag UItemFunctionLibrary::GetTagForItemType(const FSerializableItemId& type) {
	return GetItemRegistry().Get(type).getTag();
}

bool UItemFunctionLibrary::DoesSlotTypeAcceptItemType(ESlotType comparedSlotType, const FSerializableItemId& comparedItemType) {
	return comparedSlotType == ESlotType::Any ? true : GetItemRegistry().Get(comparedItemType).accepts(comparedSlotType);
}

bool UItemFunctionLibrary::GetIsInventoryOnlyForItemType(const FSerializableItemId& type) {
	return GetItemRegistry().Get(type).isInventoryOnly();
}

bool UItemFunctionLibrary::GetIsPassiveForItemType(const FSerializableItemId& type) {
	return GetItemRegistry().Get(type).isPassive();
}

FString UItemFunctionLibrary::Conv_ItemTypeIDToString(const FSerializableItemId& type) {
	return BreakItemId(type).ToString();
}

void UItemFunctionLibrary::PreloadGearIconTextures()
{
	const auto& itemTypes = GetItemRegistry().GetValues();
	s_PreloadGearIconTextures.SetNumZeroed(itemTypes.Num());

	for (int i(0); i < itemTypes.Num(); ++i)
	{
		auto& thisItemType = *itemTypes[i];

		FString IconPath = IDungeonsModule::Get().GetItemAssetFinder()->GearIconPath(thisItemType.getId()).Get(FSoftObjectPath()).ToString();

		if (IconPath.Len() > 0)
		{
			FSoftObjectPath SoftPath = IconPath;

			UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftPath,
				[i, SoftPath]() {

				if (s_PreloadGearIconTextures.IsValidIndex(i))
				{
					s_PreloadGearIconTextures[i] = Cast<UTexture2D>(SoftPath.ResolveObject());
					if (s_PreloadGearIconTextures[i])
					{
						s_PreloadGearIconTextures[i]->AddToRoot();
					}
				}
				
			}
				, FStreamableManager::AsyncLoadHighPriority,
				false,
				false,
				TEXT("PreloadGearIconTextures"));
		}

	}
}

void UItemFunctionLibrary::ClearPreloadedGearIconTextures()
{
	for (auto LoadedTexture : s_PreloadGearIconTextures)
	{
		if (LoadedTexture)
		{
			LoadedTexture->RemoveFromRoot();
		}
	}
	s_PreloadGearIconTextures.Reset();
}

UTexture2D* UItemFunctionLibrary::GetPreloadGearIconTextureInternal(const FSerializableItemId& type)
{
	const auto index = GetItemRegistry().IndexOf(type);
	if (s_PreloadGearIconTextures.IsValidIndex(index))
	{
		//we have tried to preload, check if its ongoing
		if (s_PreloadGearIconTextures[index])
		{
			return s_PreloadGearIconTextures[index];
		}
		else
		{
			//not finished streaming, force the issue
			if (const auto IconPath = IDungeonsModule::Get().GetItemAssetFinder()->GearIconPath(type))
			{
				UAssetManager::GetStreamableManager().RequestSyncLoad(IconPath.GetValue());
				UTexture2D* pNewTexture = Cast<UTexture2D>(IconPath->ResolveObject());

				if (pNewTexture)
				{
					pNewTexture->AddToRoot();
					s_PreloadGearIconTextures[index] = pNewTexture;
					return pNewTexture;
				}
			}
		}
	}

	//fall back to static load
	return GetTextureHelper(IDungeonsModule::Get().GetItemAssetFinder()->GearIconPath(type));
}

UTexture2D* UItemFunctionLibrary::GetTextureHelper(const TOptional<FSoftObjectPath> path) {
	if (path) {
		return Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, *path->ToString()));
	}

	return nullptr;
}

bool UItemFunctionLibrary::IsItemIdValid(const FSerializableItemId& type) {
	return type.IsValid();
}
