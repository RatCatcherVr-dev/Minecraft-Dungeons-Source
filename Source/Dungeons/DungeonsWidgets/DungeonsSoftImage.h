// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DungeonsImage.h"
#include "SoftBrush.h"
#include "DungeonsSoftImage.generated.h"

/**
 * The image widget holds a soft pointer to a Slate Brush, which is easier to garbage collect than DungeonsImage. Use this for large UI textures so they are offloaded when playing ingame.
 *
 * * No Children
 */
UCLASS(HideCategories=("Appearance|Image"), AutoExpandCategories=("Appearance|Soft Image"))
class DUNGEONS_API UDungeonsSoftImage : public UDungeonsImage
{
	GENERATED_BODY()

public:

	/** Soft object pointer to a Texture2D. Useful for offloading textures from memory when they're not used. E.g. map images when playing ingame. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance|Soft Image")
	FSoftBrush SoftBrush;

#if WITH_EDITOR
	/**
	 * This is called when a property is about to be modified externally.
	 */
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	/** Set the texture that sets the Brush. This should be the only Setter used in DungeonsSoftImage. */
	virtual void SetBrushFromSoftTexture(TSoftObjectPtr<UTexture2D> SoftTexture, bool bMatchSize = false) override;

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

private:
	void SetBrushFromSoftBrush();
};
