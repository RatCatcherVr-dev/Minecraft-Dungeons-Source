// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Misc/Attribute.h"
#include "Styling/SlateBrush.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "Components/Widget.h"
#include "UObject/ScriptInterface.h"
#include "Slate/SlateTextureAtlasInterface.h"
#include "DungeonsImage.generated.h"

class SDungeonsImage;
class UMaterialInstanceDynamic;
class UMaterialInterface;
class USlateBrushAsset;
class UTexture2D;
struct FStreamableHandle;

/**
 * The image widget allows you to display a Slate Brush, or texture or material in the UI.
 *
 * * No Children
 */
UCLASS(AutoExpandCategories=("Appearance|Image"))
class DUNGEONS_API UDungeonsImage : public UWidget
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
		bool SnapToPixels = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
		bool DynamicallyStreamAsset = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
		bool bRenderAsPowerOfTwo = false;

#if WITH_EDITORONLY_DATA
	/** Image to draw */
	UPROPERTY()
		USlateBrushAsset* Image_DEPRECATED;
#endif

	/** Image to draw */
	// Put Brush in a sub-category so it can be hidden by derived classes (e.g. DungeonsSoftImage)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance|Image")
		FSlateBrush Brush;

	/** A bindable delegate for the Image. */
	UPROPERTY()
		FGetSlateBrush BrushDelegate;

	/** Color and opacity */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (sRGB = "true"))
		FLinearColor ColorAndOpacity;

	/** A bindable delegate for the ColorAndOpacity. */
	UPROPERTY()
		FGetLinearColor ColorAndOpacityDelegate;


	virtual void SetVisibility(ESlateVisibility InVisibility) override;

	virtual void Serialize(FArchive& Ar);

#if WITH_EDITOR
	/**
	 * This is called when a property is about to be modified externally
	 *
	 * @param PropertyThatWillChange	Property that will be changed
	 */
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	/**
	 * This alternate version of PostEditChange is called when properties inside structs are modified.  The property that was actually modified
	 * is located at the tail of the list.  The head of the list of the UStructProperty member variable that contains the property that was modified.
	 */
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;

#endif
public:

	UPROPERTY(EditAnywhere, Category = Events, meta = (IsBindableEvent = "True"))
		FOnPointerEvent OnMouseButtonDownEvent;

public:

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetColorAndOpacity(FLinearColor InColorAndOpacity);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetOpacity(float InOpacity);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetBrushSize(FVector2D DesiredSize);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
		void SetBrushTintColor(FSlateColor TintColor);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
		virtual void SetBrush(const FSlateBrush& InBrush);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
		virtual void SetBrushFromAsset(USlateBrushAsset* Asset);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
		virtual void SetBrushFromTexture(UTexture2D* Texture, bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
		virtual void SetBrushFromAtlasInterface(TScriptInterface<ISlateTextureAtlasInterface> AtlasRegion, bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
		virtual void SetBrushFromTextureDynamic(UTexture2DDynamic* Texture, bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
		virtual void SetBrushFromMaterial(UMaterialInterface* Material);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		virtual void SetBrushFromSoftSprite(TSoftObjectPtr<UObject> SoftPaperSprite, bool bMatchSize = false);
	
	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
		virtual void SetBrushFromSoftTexture(TSoftObjectPtr<UTexture2D> SoftTexture, bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
		virtual void SetBrushFromSoftTextureBlocking(TSoftObjectPtr<UTexture2D> SoftTexture, bool bMatchSize = false);

	UFUNCTION(BlueprintCallable, Category = "Appearance")
		virtual void SetBrushFromSoftTextureBlockingHighLow(TSoftObjectPtr<UTexture2D> SoftTextureHigh, TSoftObjectPtr<UTexture2D> SoftTextureLow, bool UseLow,  bool bMatchSize = false);

	/**  */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
		UMaterialInstanceDynamic* GetDynamicMaterial();

	//~ Begin UWidget Interface
	virtual void SynchronizeProperties() override;
	//~ End UWidget Interface

	//~ Begin UVisual Interface
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//~ End UVisual Interface

	void LoadBrushImage();

	void UnloadBrushImage();

	void CompleteTextureStreaming();
	//~ Begin UObject Interface
	virtual void PostLoad() override;
	//~ End UObject Interface

	/** Mark this object as modified, also mark the slot as modified. */
	virtual bool Modify(bool bAlwaysMarkDirty = true) override;

	virtual void SetIsEnabled(bool bInIsEnabled) override;

	/** DO NOT REMOVE!. Design time safety check to prevent hard references dragging large textures into memory on Switch. */
	UFUNCTION(BlueprintCallable, Category = "Appearance")
	virtual void CheckForHardReferencedImage(bool DesignTime);

#if WITH_EDITOR
	//~ Begin UWidget Interface
	virtual const FText GetPaletteCategory() override;
	//~ End UWidget Interface
#endif

protected:
	//~ Begin UWidget Interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	//~ End UWidget Interface

	/** Translates the bound brush data and assigns it to the cached brush used by this widget. */
	const FSlateBrush* ConvertImage(TAttribute<FSlateBrush> InImageAsset) const;

	//
	void CancelTextureStreaming();

	//
	FReply HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent);


	virtual void OnWidgetRebuilt() override;
	
	UFUNCTION()
	void OnRootWidgetVisibilityChanged(ESlateVisibility NewVisibility);
protected:

	TSharedPtr<SDungeonsImage> MyImage;
	TSharedPtr<FStreamableHandle> StreamingHandle;

protected:

	PROPERTY_BINDING_IMPLEMENTATION(FSlateColor, ColorAndOpacity);

private:
	FVector2D GetDesiredTextureBrushSize(const UTexture2D& Texture);

	void OnObjectToRenderChanged();
	void OnRenderTextureModeChanged();

	void SetupUVs();

	void ClearResourceObject();
	void GetUVPointsToRender(TArray<FVector2D>& target);

	bool DoesPathExists(FSoftObjectPath path);
private:
	UPROPERTY()
	TSoftObjectPtr<UObject> ObjectToLoad;
	UUserWidget* ourUserWidget;
	bool bIsTexture /*= true*/;
	
	bool AreSizesPowerOfTwo(int SizeX, int SizeY);

};
