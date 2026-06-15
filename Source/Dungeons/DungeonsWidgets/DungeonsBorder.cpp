

#include "DungeonsBorder.h"
#include "Slate/SlateBrushAsset.h"
#include "Materials/MaterialInterface.h"
#include "Engine/Texture2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Layout/SBorder.h"
#include "ObjectEditorUtils.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "DungeonsBorderSlot.h"

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
// UBorder

UDungeonsBorder::UDungeonsBorder(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = false;

	ContentColorAndOpacity = FLinearColor::White;
	BrushColor = FLinearColor::White;

	Padding = FMargin(4, 2);

	HorizontalAlignment = HAlign_Fill;
	VerticalAlignment = VAlign_Fill;

	DesiredSizeScale = FVector2D(1, 1);

	bShowEffectWhenDisabled = true;
}

void UDungeonsBorder::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	UnloadBrushImage();
	MyBorder.Reset();
}

TSharedRef<SWidget> UDungeonsBorder::RebuildWidget()
{
	MyBorder = SNew(SBorder);

	if (GetChildrenCount() > 0)
	{
		Cast<UDungeonsBorderSlot>(GetContentSlot())->BuildSlot(MyBorder.ToSharedRef());
	}

	return MyBorder.ToSharedRef();
}

void UDungeonsBorder::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	TAttribute<FLinearColor> ContentColorAndOpacityBinding = PROPERTY_BINDING(FLinearColor, ContentColorAndOpacity);
	TAttribute<FSlateColor> BrushColorBinding = OPTIONAL_BINDING_CONVERT(FLinearColor, BrushColor, FSlateColor, ConvertLinearColorToSlateColor);
	TAttribute<const FSlateBrush*> ImageBinding = OPTIONAL_BINDING_CONVERT(FSlateBrush, Background, const FSlateBrush*, ConvertImage);

	MyBorder->SetPadding(Padding);
	MyBorder->SetBorderBackgroundColor(BrushColorBinding);
	MyBorder->SetColorAndOpacity(ContentColorAndOpacityBinding);

	MyBorder->SetBorderImage(ImageBinding);

	MyBorder->SetDesiredSizeScale(DesiredSizeScale);
	MyBorder->SetShowEffectWhenDisabled(bShowEffectWhenDisabled != 0);

	MyBorder->SetOnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonDown));
	MyBorder->SetOnMouseButtonUp(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonUp));
	MyBorder->SetOnMouseMove(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseMove));
	MyBorder->SetOnMouseDoubleClick(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseDoubleClick));
}

UClass* UDungeonsBorder::GetSlotClass() const
{
	return UDungeonsBorderSlot::StaticClass();
}

void UDungeonsBorder::OnSlotAdded(UPanelSlot* InSlot)
{
	// Copy the content properties into the new slot so that it matches what has been setup
	// so far by the user.
	UDungeonsBorderSlot* BorderSlot = CastChecked<UDungeonsBorderSlot>(InSlot);
	BorderSlot->Padding = Padding;
	BorderSlot->HorizontalAlignment = HorizontalAlignment;
	BorderSlot->VerticalAlignment = VerticalAlignment;

	// Add the child to the live slot if it already exists
	if (MyBorder.IsValid())
	{
		// Construct the underlying slot.
		BorderSlot->BuildSlot(MyBorder.ToSharedRef());
	}
}

void UDungeonsBorder::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (MyBorder.IsValid())
	{
		MyBorder->SetContent(SNullWidget::NullWidget);
	}
}

void UDungeonsBorder::OnWidgetRebuilt()
{
	LoadBrushImage();
}

void UDungeonsBorder::LoadBrushImage()
{
	if (!ObjectToLoad.IsNull())
	{
		if (bIsTexture)
		{
			TWeakObjectPtr<UDungeonsBorder> WeakThis(this); // using weak ptr in case 'this' has gone out of scope by the time this lambda is called

			StreamingHandle =
				UAssetManager::GetStreamableManager().RequestAsyncLoad(ObjectToLoad.ToSoftObjectPath(),
					[WeakThis]() {
				if (WeakThis.IsValid())
				{
					UObject* imageLoaded = WeakThis->ObjectToLoad.Get();
					if (imageLoaded)
					{
						WeakThis->CurrentLoadedObject = imageLoaded;
						WeakThis->bLoaded = true;
						WeakThis->Background.SetResourceObject(imageLoaded);
					}
				}
			}, FStreamableManager::AsyncLoadHighPriority);
		}
		else
		{
			UAssetManager::GetStreamableManager().RequestSyncLoad(ObjectToLoad.ToSoftObjectPath(), true);
			CurrentLoadedObject = ObjectToLoad.Get();
			Background.SetResourceObject(CurrentLoadedObject);
		}

		bLoaded = true;
	}
}

void UDungeonsBorder::UnloadBrushImage()
{
	if (bLoaded)
	{
		bLoaded = false;
		CurrentLoadedObject = nullptr;
		Background.SetResourceObject(nullptr);
		UAssetManager::GetStreamableManager().Unload(ObjectToLoad.ToSoftObjectPath());
	}
}

void UDungeonsBorder::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	if (IsImageVisible(InVisibility))
	{
		LoadBrushImage();
	}
	else
	{
		UnloadBrushImage();
	}
}

bool UDungeonsBorder::IsImageVisible(ESlateVisibility InVisibility)
{
	return InVisibility == ESlateVisibility::Visible || InVisibility == ESlateVisibility::SelfHitTestInvisible || InVisibility == ESlateVisibility::HitTestInvisible;
}

void UDungeonsBorder::SetContentColorAndOpacity(FLinearColor Color)
{
	ContentColorAndOpacity = Color;
	if (MyBorder.IsValid())
	{
		MyBorder->SetColorAndOpacity(Color);
	}
}

void UDungeonsBorder::SetPadding(FMargin InPadding)
{
	Padding = InPadding;
	if (MyBorder.IsValid())
	{
		MyBorder->SetPadding(InPadding);
	}
}

void UDungeonsBorder::SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	HorizontalAlignment = InHorizontalAlignment;
	if (MyBorder.IsValid())
	{
		MyBorder->SetHAlign(InHorizontalAlignment);
	}
}

void UDungeonsBorder::SetVerticalAlignment(EVerticalAlignment InVerticalAlignment)
{
	VerticalAlignment = InVerticalAlignment;
	if (MyBorder.IsValid())
	{
		MyBorder->SetVAlign(InVerticalAlignment);
	}
}

void UDungeonsBorder::SetBrushColor(FLinearColor Color)
{
	BrushColor = Color;
	if (MyBorder.IsValid())
	{
		MyBorder->SetBorderBackgroundColor(Color);
	}
}

FReply UDungeonsBorder::HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (OnMouseButtonDownEvent.IsBound())
	{
		return OnMouseButtonDownEvent.Execute(Geometry, MouseEvent).NativeReply;
	}

	return FReply::Unhandled();
}

FReply UDungeonsBorder::HandleMouseButtonUp(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (OnMouseButtonUpEvent.IsBound())
	{
		return OnMouseButtonUpEvent.Execute(Geometry, MouseEvent).NativeReply;
	}

	return FReply::Unhandled();
}

FReply UDungeonsBorder::HandleMouseMove(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (OnMouseMoveEvent.IsBound())
	{
		return OnMouseMoveEvent.Execute(Geometry, MouseEvent).NativeReply;
	}

	return FReply::Unhandled();
}

FReply UDungeonsBorder::HandleMouseDoubleClick(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (OnMouseDoubleClickEvent.IsBound())
	{
		return OnMouseDoubleClickEvent.Execute(Geometry, MouseEvent).NativeReply;
	}

	return FReply::Unhandled();
}

void UDungeonsBorder::SetBrush(const FSlateBrush& Brush)
{
	OnNewResourceObject();
	Background = Brush;
	ObjectToLoad = Brush.GetResourceObject();

	if (MyBorder.IsValid())
	{
		MyBorder->SetBorderImage(&Background);
	}
	LoadBrushImage();
}

void UDungeonsBorder::SetBrushFromAsset(USlateBrushAsset* Asset)
{
	Background = Asset ? Asset->Brush : FSlateBrush();

	if (MyBorder.IsValid())
	{
		MyBorder->SetBorderImage(&Background);
	}
	ObjectToLoad = Background.GetResourceObject();
	LoadBrushImage();
}

void UDungeonsBorder::SetBrushFromTexture(UTexture2D* Texture)
{
	OnNewResourceObject();
	ObjectToLoad = Texture;
	LoadBrushImage();

	if (MyBorder.IsValid())
	{
		MyBorder->SetBorderImage(&Background);
	}
}

void UDungeonsBorder::SetBrushFromMaterial(UMaterialInterface* Material)
{
	if (!Material)
	{
		UE_LOG(LogSlate, Log, TEXT("UBorder::SetBrushFromMaterial.  Incoming material is null"));
	}

	OnNewResourceObject();
	ObjectToLoad = Material;
	LoadBrushImage();

	//TODO UMG Check if the material can be used with the UI

	if (MyBorder.IsValid())
	{
		MyBorder->SetBorderImage(&Background);
	}
}

UMaterialInstanceDynamic* UDungeonsBorder::GetDynamicMaterial()
{
	UMaterialInterface* Material = nullptr;

	UObject* Resource = Background.GetResourceObject();
	Material = Cast<UMaterialInterface>(Resource);

	if (Material)
	{
		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);

		if (!DynamicMaterial)
		{
			DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
			Background.SetResourceObject(DynamicMaterial);
			CurrentLoadedObject = DynamicMaterial;
			if (MyBorder.IsValid())
			{
				MyBorder->SetBorderImage(&Background);
			}
		}

		return DynamicMaterial;
	}

	//TODO UMG can we do something for textures?  General purpose dynamic material for them?
	return nullptr;
}

void UDungeonsBorder::SetDesiredSizeScale(FVector2D InScale)
{
	DesiredSizeScale = InScale;
	if (MyBorder.IsValid())
	{
		MyBorder->SetDesiredSizeScale(InScale);
	}
}

const FSlateBrush* UDungeonsBorder::ConvertImage(TAttribute<FSlateBrush> InImageAsset) const
{
	UDungeonsBorder* MutableThis = const_cast<UDungeonsBorder*>(this);
	MutableThis->Background = InImageAsset.Get();

	return &Background;
}

void UDungeonsBorder::OnNewResourceObject()
{
	UnloadBrushImage();
}

void UDungeonsBorder::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITORONLY_DATA
	if (GetLinkerUE4Version() < VER_UE4_DEPRECATE_UMG_STYLE_ASSETS && Brush_DEPRECATED != nullptr)
	{
		Background = Brush_DEPRECATED->Brush;
		Brush_DEPRECATED = nullptr;
	}
#endif

	if (GetChildrenCount() > 0)
	{
		//TODO UMG Pre-Release Upgrade, now have slots of their own.  Convert existing slot to new slot.
		if (UPanelSlot* PanelSlot = GetContentSlot())
		{
			UDungeonsBorderSlot* BorderSlot = Cast<UDungeonsBorderSlot>(PanelSlot);
			if (BorderSlot == NULL)
			{
				BorderSlot = NewObject<UDungeonsBorderSlot>(this);
				BorderSlot->Content = GetContentSlot()->Content;
				BorderSlot->Content->Slot = BorderSlot;
				Slots[0] = BorderSlot;
			}
		}
	}

	if (ObjectToLoad.IsNull() && Background.GetResourceObject())
	{
		bIsTexture = Background.GetResourceObject()->IsA(UTexture2D::StaticClass());
		if (!bIsTexture)
		{
			return;
		}

		ObjectToLoad = Background.GetResourceObject();
		Background.SetResourceObject(nullptr);
	}
}

#if WITH_EDITOR

void UDungeonsBorder::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	static bool IsReentrant = false;

	if (!IsReentrant)
	{
		IsReentrant = true;

		if (PropertyChangedEvent.Property)
		{
			static const FName PaddingName("Padding");
			static const FName HorizontalAlignmentName("HorizontalAlignment");
			static const FName VerticalAlignmentName("VerticalAlignment");

			FName PropertyName = PropertyChangedEvent.Property->GetFName();

			if (UDungeonsBorderSlot* BorderSlot = Cast<UDungeonsBorderSlot>(GetContentSlot()))
			{
				if (PropertyName == PaddingName)
				{
					FObjectEditorUtils::MigratePropertyValue(this, PaddingName, BorderSlot, PaddingName);
				}
				else if (PropertyName == HorizontalAlignmentName)
				{
					FObjectEditorUtils::MigratePropertyValue(this, HorizontalAlignmentName, BorderSlot, HorizontalAlignmentName);
				}
				else if (PropertyName == VerticalAlignmentName)
				{
					FObjectEditorUtils::MigratePropertyValue(this, VerticalAlignmentName, BorderSlot, VerticalAlignmentName);
				}
			}
		}

		IsReentrant = false;
	}
}

const FText UDungeonsBorder::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE

