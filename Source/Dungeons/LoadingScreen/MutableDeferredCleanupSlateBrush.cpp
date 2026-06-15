// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "LoadingScreen/MutableDeferredCleanupSlateBrush.h"
#include "Engine/Texture.h"

TSharedRef<FMutableDeferredCleanupSlateBrush> FMutableDeferredCleanupSlateBrush::CreateBrush(
	UTexture* InTexture,
	const FVector2D& InImageSize,
	const FLinearColor& InTint,
	ESlateBrushTileType::Type InTiling,
	ESlateBrushImageType::Type InImageType)
{
	FSlateBrush* Brush = new FSlateBrush();
	Brush->SetResourceObject(InTexture);
	Brush->ImageSize = InImageSize;
	Brush->TintColor = InTint;
	Brush->Tiling = InTiling;
	Brush->ImageType = InImageType;

	return MakeShareable(new FMutableDeferredCleanupSlateBrush(Brush), [](FMutableDeferredCleanupSlateBrush* ObjectToDelete) { BeginCleanup(ObjectToDelete); });
}

void FMutableDeferredCleanupSlateBrush::AddReferencedObjects(FReferenceCollector& Collector)
{
	InternalBrush->AddReferencedObjects(Collector);
}

FString FMutableDeferredCleanupSlateBrush::GetReferencerName() const
{
	return "FMutableDeferredCleanupSlateBrush";
}

FSlateBrush* FMutableDeferredCleanupSlateBrush::GetMutableSlateBrush() const {
	return InternalBrush;
}
