// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "RenderingThread.h"
#include "UObject/GCObject.h"
#include "Styling/SlateBrush.h"

class DUNGEONS_API FMutableDeferredCleanupSlateBrush : public ISlateBrushSource, public FDeferredCleanupInterface, public FGCObject
{
public:
	static TSharedRef<FMutableDeferredCleanupSlateBrush> CreateBrush(
		class UTexture* InTexture,
		const FVector2D& InImageSize,
		const FLinearColor& InTint = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f),
		ESlateBrushTileType::Type InTiling = ESlateBrushTileType::NoTile,
		ESlateBrushImageType::Type InImageType = ESlateBrushImageType::FullColor);

	virtual const FSlateBrush* GetSlateBrush() const override { return InternalBrush; }

	FSlateBrush* GetMutableSlateBrush() const;

	// FGCObject
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;

private:
	FMutableDeferredCleanupSlateBrush();

	FMutableDeferredCleanupSlateBrush(FSlateBrush* Brush)
		: InternalBrush(Brush)
	{
	}

	FSlateBrush* InternalBrush;
};
