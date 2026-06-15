#include "DungeonsSoftImage.h"

#include "Engine/Texture2D.h"

#if WITH_EDITOR
// Called when a property is changed in the editor
void UDungeonsSoftImage::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property)
	{
		const auto PropertyName = PropertyChangedEvent.GetPropertyName();
		if (PropertyName == GET_MEMBER_NAME_CHECKED(FSoftBrush, texture) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(FSoftBrush, matchSize))
		{
			SetBrushFromSoftBrush();
		}
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UDungeonsSoftImage::SetBrushFromSoftTexture(TSoftObjectPtr<UTexture2D> SoftTexture, bool bMatchSize)
{
	SoftBrush.matchSize = bMatchSize;
	SoftBrush.texture = SoftTexture;
	SetBrushFromSoftBrush();
}

// Called when starting or compiling
void UDungeonsSoftImage::SynchronizeProperties()
{
	SetBrushFromSoftBrush();
	Super::SynchronizeProperties();
}

void UDungeonsSoftImage::SetBrushFromSoftBrush()
{
	Super::SetBrushFromSoftTexture(SoftBrush.texture, SoftBrush.matchSize);
}
