

#pragma once

#include "CoreMinimal.h"
#include "game/item/ItemType.h"
#include "SerializableItemId.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType, meta = (HasNativeMake = "Dungeons.ItemFunctionLibrary.MakeItemId", HasNativeBreak = "Dungeons.ItemFunctionLibrary.BreakItemId"))
struct DUNGEONS_API FSerializableItemId
{
	GENERATED_BODY()

	//Only exists for unreal purposes.
	FSerializableItemId();

	//Use this when you need to pass an FItemId to Blueprints or via network.
	FSerializableItemId(const FItemId& id) :Id(id), SerializedId(id.GetBackingType()) {};

	operator const FItemId&() const;
	bool operator==(const FSerializableItemId& other) const;
	bool operator!=(const FSerializableItemId& other) const;
	bool operator<(const FSerializableItemId& other) const;
	
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
	void PostSerialize(const FArchive& Ar);

	friend uint32 GetTypeHash(const FSerializableItemId&);

	FString ToString() const;
	bool IsValid() const;

	FName GetSerializedId() const { return SerializedId; }
private:
	const FItemId& ItemId() const;
	
	mutable TOptional<FItemId> Id;
	
	UPROPERTY(EditDefaultsOnly)
	FName SerializedId;
};


template<>
struct TStructOpsTypeTraits<FSerializableItemId> : public TStructOpsTypeTraitsBase2<FSerializableItemId>
{
	enum
	{
		WithNetSerializer = true,
		WithPostSerialize = true,
		WithIdenticalViaEquality = true,
	};
};