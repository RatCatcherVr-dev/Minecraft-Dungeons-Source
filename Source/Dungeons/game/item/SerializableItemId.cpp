#include "SerializableItemId.h"

FSerializableItemId::FSerializableItemId()
	/*
	When running on PS4 we will execute this prior to FItemRegistry being completed. To solve this we leave this little gem.
	Sword will hopefully always be with us, and is a "safe default". This type should not be optional, and even default
	constructed it should be functioning. Thus, we find our selves with adding some kind of default. Sword is the first item
	registered when this was created, so it seems fitting it is the default.
	*/
	:SerializedId(TEXT("Sword"))
{
}

FSerializableItemId::operator const FItemId&() const {
	return ItemId();
}

bool FSerializableItemId::operator==(const FSerializableItemId& other) const
{
	return SerializedId == other.SerializedId;
}

bool FSerializableItemId::operator!=(const FSerializableItemId& other) const
{
	return SerializedId != other.SerializedId;
}

bool FSerializableItemId::operator<(const FSerializableItemId& other) const {
	return ItemId() < other;
}

bool FSerializableItemId::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) {
	FItemRegistry::SizeType index;
	if(Ar.IsSaving()) {
		index = GetItemRegistry().IndexOf(Id.GetValue());
	}

	Ar << index;

	if(Ar.IsLoading()) {
		Id = GetItemRegistry().GetValues()[index]->getId();
		SerializedId = Id->GetBackingType();
	}

	bOutSuccess = true;
	return true;
}

void FSerializableItemId::PostSerialize(const FArchive& Ar) {
	if(Ar.IsLoading()) {
		auto backingId = GetItemRegistry().Request(SerializedId);
	ensureAlwaysMsgf(backingId, TEXT("Invalid Item id %s detected when deserializing."), *SerializedId.ToString());
		Id = backingId;
	}
}

FString FSerializableItemId::ToString() const {
	return ItemId().GetBackingType().ToString();
}

uint32 GetTypeHash(const FSerializableItemId& itemId) {
	return GetTypeHash(itemId.SerializedId);
}

bool FSerializableItemId::IsValid() const {
	Id = GetItemRegistry().Request(SerializedId);

	return Id.IsSet();
}

const FItemId& FSerializableItemId::ItemId() const {
	if (!Id) {
		Id = GetItemRegistry().Request(SerializedId);
	}

	return Id.GetValue();
}
