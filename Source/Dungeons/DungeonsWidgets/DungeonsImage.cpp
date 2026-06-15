

#include "DungeonsImage.h"
#include "Components/Image.h"
#include "Slate/SlateBrushAsset.h"
#include "Materials/MaterialInterface.h"
#include "Engine/Texture2D.h"
#include "Engine/Texture2DDynamic.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SDungeonsImage.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Blueprint/WidgetTree.h"
#if WITH_EDITOR
#include "Editor.h"
#endif
#include "ui/WidgetHelper.h"

#define COUNT_RESOURCE_HANDLES 0

#define LOCTEXT_NAMESPACE "UMG"

static TAutoConsoleVariable<int> CVarEnableSerialiseLargeTextureWarning(TEXT("Dungeons.image.LargeTextureWarning"), 0, TEXT("Enable or disable the runtime display warnings for larger texture hard reference soaking up memory"), ECVF_Default);


/////////////////////////////////////////////////////
// UImage

UDungeonsImage::UDungeonsImage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ColorAndOpacity(FLinearColor::White)
{
}

void UDungeonsImage::PostLoad()
{
	Super::PostLoad();
	
	{
		//default unload image object post load (should be reloaded on visibility change)
		if (Brush.GetResourceObject())
		{
			UnloadBrushImage();
		}

		OnObjectToRenderChanged();
	}
}

bool UDungeonsImage::Modify(bool bAlwaysMarkDirty /*= true*/)
{
	return Super::Modify(bAlwaysMarkDirty);
}

void UDungeonsImage::SetIsEnabled(bool bInIsEnabled)
{
	return Super::SetIsEnabled(bInIsEnabled);
}

void UDungeonsImage::CheckForHardReferencedImage(bool DesignTime)
{
#if WITH_EDITOR

	if (!DesignTime)
		return; //do nothing

	if (!Brush.GetResourceObject())
		return;

	int32 EstimatedResourceSize = Brush.GetResourceObject()->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal);
	constexpr float BytesPerMiB = 1024.0f * 1024.0f;

FString WarningMsg = FString::Format(TEXT(
"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
"!! WARNING !!\n"
"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
"{0}\n"
"Has Hard references to image\n"
"{1}\n"
"This will drag this image into memory on switch!\n"
"Please use soft references to prevent this\n"
"Potential Memory Loss : {2} bytes ( {3} MB )\n"
"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
), {GetName(), Brush.GetResourceObject()->GetName(), FText::AsNumber(EstimatedResourceSize).ToString(), FText::AsNumber( EstimatedResourceSize / BytesPerMiB ).ToString() } );

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, WarningMsg);
#endif

}

void UDungeonsImage::ReleaseSlateResources(bool bReleaseChildren)
{

#if COUNT_RESOURCE_HANDLES
	{
		FSoftObjectPath SoftPath;

		if (!ObjectToLoad.IsNull())
		{
			SoftPath = ObjectToLoad.ToSoftObjectPath();
		}
		else if (Brush.GetResourceObject())
		{
			SoftPath = Brush.GetResourceObject();
		}

		TArray<TSharedRef<FStreamableHandle>> HandleList;

		if (UAssetManager::GetStreamableManager().GetActiveHandles(SoftPath, HandleList, false))
		{
			FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### UDungeonsImage::ReleaseSlateResources : GetActiveHandles %d ##################\n"), HandleList.Num());
		}
	}
#endif
	
	UnloadBrushImage();
	Super::ReleaseSlateResources(bReleaseChildren);	
	MyImage.Reset();

	
}


void UDungeonsImage::LoadBrushImage()
{
	if (!DynamicallyStreamAsset)
	{
		return;
	}

	if (!ObjectToLoad.IsNull())
	{
		// We need to do this because some widgets may never changed the objectToLoad and trying to
		// load on PostLoad() crashes the game		

		if (bIsTexture)
		{
			if (StreamingHandle.IsValid())
			{
				//already streaming
				TArray<FSoftObjectPath> AssetList;
				StreamingHandle->GetRequestedAssets(AssetList);
				if (AssetList.Num())
				{
					if (AssetList[0] == ObjectToLoad.ToSoftObjectPath()) //already loading this asset? just let it get on with it
					{
						return;
					}
				}

				//got here, currently streaming a different asset, not great, lets cancel and kick off a new one
				CancelTextureStreaming();
			}
			
			TWeakObjectPtr<UDungeonsImage> WeakThis(this); // using weak ptr in case 'this' has gone out of scope by the time this lambda is called
			
			FSoftObjectPath SoftPath = ObjectToLoad.ToSoftObjectPath();

			StreamingHandle =
				UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftPath,
					[WeakThis, SoftPath]() {
				if (WeakThis.IsValid())
				{
					if (WeakThis->ObjectToLoad.ToSoftObjectPath() == SoftPath)
					{
						UObject* imageLoaded = SoftPath.ResolveObject();
						WeakThis->Brush.SetResourceObject(imageLoaded);
						WeakThis->OnObjectToRenderChanged();
						WeakThis->ObjectToLoad.Reset();
					}
				}
			}, FStreamableManager::AsyncLoadHighPriority);
		}
		else
		{
			CancelTextureStreaming();
			UAssetManager::GetStreamableManager().RequestSyncLoad(ObjectToLoad.ToSoftObjectPath(), true);
			Brush.SetResourceObject(ObjectToLoad.Get());
			OnObjectToRenderChanged();
			ObjectToLoad.Reset();
		}

	}
}

void UDungeonsImage::UnloadBrushImage()
{
	if (DynamicallyStreamAsset)
	{
		if (StreamingHandle.IsValid())
		{
			StreamingHandle->CancelHandle();
			StreamingHandle.Reset();
		}

		if (Brush.GetResourceObject() && bIsTexture)
		{
			ObjectToLoad = Brush.GetResourceObject();
			UAssetManager::GetStreamableManager().Unload(ObjectToLoad.ToSoftObjectPath());
			Brush.SetResourceObject(nullptr);
		}
	}
}

void UDungeonsImage::CompleteTextureStreaming()
{
	if (StreamingHandle.IsValid())
	{
		StreamingHandle->WaitUntilComplete();
		StreamingHandle.Reset();
	}
}

TSharedRef<SWidget> UDungeonsImage::RebuildWidget()
{
	MyImage = SNew(SDungeonsImage);
	return MyImage.ToSharedRef();
}

void UDungeonsImage::OnWidgetRebuilt()
{
	ourUserWidget = UWidgetHelper::GetUserWidgetBeforeRoot(this);
	if (ourUserWidget)
	{
		if (UWidgetHelper::IsVisible(ourUserWidget->GetVisibility()))
		{
			LoadBrushImage();
		}

		// @pcosta: this code does not compile on UE4.22.3, I disabled it now for you to re-enable at your convenience  // jryden
				// If we use the delegate we can have a memory boost since all the non visible widgets wil be unloaded
		//#if ENGINE_MINOR_VERSION > 21
				//ourUserWidget->OnWidgetVisibilityChanged.AddDynamic(this, &UDungeonsImage::OnRootWidgetVisibilityChanged);
		//#else
		LoadBrushImage();
		//#endif
	}
	else
	{
		LoadBrushImage();
	}
	
	OnObjectToRenderChanged();
}

void UDungeonsImage::OnRootWidgetVisibilityChanged(ESlateVisibility NewVisibility)
{
	if (UWidgetHelper::IsVisible(NewVisibility))
	{
		LoadBrushImage();
	}
	else
	{
		UnloadBrushImage();
	}
}

FVector2D UDungeonsImage::GetDesiredTextureBrushSize(const UTexture2D& Texture) {
	if (bRenderAsPowerOfTwo) {
		return FVector2D(Texture.GetImportedSize().X, Texture.GetImportedSize().Y);
	} else {
		return FVector2D(Texture.GetSizeX(), Texture.GetSizeY());
	}
}

void UDungeonsImage::OnObjectToRenderChanged()
{
	SetupUVs();
}

void UDungeonsImage::OnRenderTextureModeChanged()
{
#if WITH_EDITOR
	UObject* ResourceObject = Brush.GetResourceObject();

	// We only want to change things if our ResourceObject is a texture
	if (ResourceObject && ResourceObject->IsA(UTexture2D::StaticClass()))
	{
		UTexture2D* Texture = Cast<UTexture2D>(ResourceObject);
		const auto IsRenderPowerOfTwo = bRenderAsPowerOfTwo ? ETexturePowerOfTwoSetting::PadToPowerOfTwo : ETexturePowerOfTwoSetting::None;
		if (Texture->PowerOfTwoMode != IsRenderPowerOfTwo)
		{
			Texture->PowerOfTwoMode = IsRenderPowerOfTwo;
			Texture->UpdateResource();

			//Save the texture
			UPackage *Package = Texture->GetOutermost();
			const FString& FileExtension = FPackageName::GetAssetPackageExtension();

			FString AssetPath = FPackageName::LongPackageNameToFilename(Package->GetName(), FileExtension);

			Texture->MarkPackageDirty();

			bool bSuccess = UPackage::SavePackage(Package, Texture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *AssetPath, GError, nullptr, false, true, ESaveFlags::SAVE_NoError);

			//Without this things will get weird
			FVector2D OriginalImageSize = FVector2D(Texture->GetImportedSize().X, Texture->GetImportedSize().Y);
			Brush.ImageSize = OriginalImageSize;
		}
	}

	SetupUVs();

#endif
}

void UDungeonsImage::SetupUVs()
{
	TArray<FVector2D> points;
	points.Reserve(4);
	GetUVPointsToRender(points);
	if (points.Num() > 0)
	{
		FBox2D UVPoints = FBox2D(points);
		if (UVPoints.bIsValid)
		{
			Brush.SetUVRegion(UVPoints);
		}
	}
}


void UDungeonsImage::GetUVPointsToRender(TArray<FVector2D>& target)
{

	UObject* Resource = Brush.GetResourceObject();
	bool bIsTilling = Brush.GetTiling() != ESlateBrushTileType::NoTile;

	UTexture2D* texture = Cast<UTexture2D>(Resource);
	if (texture)
	{
		int SizeX = texture->GetSizeX();
		int SizeY = texture->GetSizeY();
		FIntPoint ImportedSize = texture->GetImportedSize();

		if (!bIsTilling && texture->CompressionSettings != TextureCompressionSettings::TC_EditorIcon && (SizeX != ImportedSize.X || SizeY != ImportedSize.Y) && AreSizesPowerOfTwo(SizeX, SizeY) && !AreSizesPowerOfTwo(ImportedSize.X, ImportedSize.Y))
		{
			//this image's size is power of two but its imported isnt, means its been padded to pow2 or square pow2

			uint32 PowerOfTwoTextureSizeX = 0;
			uint32 PowerOfTwoTextureSizeY = 0;

			if (SizeX == SizeY) //Square power two
			{
				if (ImportedSize.X > ImportedSize.Y)
				{
					PowerOfTwoTextureSizeX = FMath::RoundUpToPowerOfTwo(ImportedSize.X);
					PowerOfTwoTextureSizeY = FMath::RoundUpToPowerOfTwo(ImportedSize.X);
				}
				else
				{
					PowerOfTwoTextureSizeX = FMath::RoundUpToPowerOfTwo(ImportedSize.Y);
					PowerOfTwoTextureSizeY = FMath::RoundUpToPowerOfTwo(ImportedSize.Y);
				}
			}
			else
			{
				PowerOfTwoTextureSizeX = FMath::RoundUpToPowerOfTwo(ImportedSize.X);
				PowerOfTwoTextureSizeY = FMath::RoundUpToPowerOfTwo(ImportedSize.Y);
			}

			float XPercentage = (float)ImportedSize.X / (float)PowerOfTwoTextureSizeX;
			float YPercentage = (float)ImportedSize.Y / (float)PowerOfTwoTextureSizeY;
			target.Add(FVector2D(0, 0));
			target.Add(FVector2D(0, YPercentage));
			target.Add(FVector2D(XPercentage, 0));
			target.Add(FVector2D(XPercentage, YPercentage));
		}
		else
		{
			target.Add(FVector2D(0, 0));
			target.Add(FVector2D(0, 1.0f));
			target.Add(FVector2D(1.0f, 0));
			target.Add(FVector2D(1.0f, 1.0f));
		}
	}
	;
}

bool UDungeonsImage::DoesPathExists(FSoftObjectPath path)
{
	// This checks if the given path exists. This is what the engine normally do to load an object
	// If you know a better solution to know if a path exists talk with Pedro Costa
	// PS: FPaths::PathExists doesn't work on consoles

	UObject* InOuter = nullptr;
	FString Name = path.ToString();
	ResolveName(InOuter, Name, true, true, LOAD_None & (LOAD_EditorOnly | LOAD_Quiet | LOAD_NoWarn | LOAD_DeferDependencyLoads), nullptr);
	if (InOuter)
	{
		UObject* Result = StaticFindObjectFast(UObject::StaticClass(), InOuter, *Name);
		return Result != nullptr;
	}
	return false;
}

bool UDungeonsImage::AreSizesPowerOfTwo(int SizeX, int SizeY)
{
	return (SizeX & (SizeX - 1)) == 0 && (SizeY & (SizeY - 1)) == 0;
}

void UDungeonsImage::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	TAttribute<FSlateColor> ColorAndOpacityBinding = PROPERTY_BINDING(FSlateColor, ColorAndOpacity);
	TAttribute<const FSlateBrush*> ImageBinding = OPTIONAL_BINDING_CONVERT(FSlateBrush, Brush, const FSlateBrush*, ConvertImage);

	if (MyImage.IsValid())
	{
		MyImage->SetImage(ImageBinding);
		MyImage->SetColorAndOpacity(ColorAndOpacityBinding);
		MyImage->SetOnMouseButtonDown(BIND_UOBJECT_DELEGATE(FPointerEventHandler, HandleMouseButtonDown));
		MyImage->SetSnapToPixels(SnapToPixels);
	}
	

	OnObjectToRenderChanged();
}

void UDungeonsImage::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	if (DynamicallyStreamAsset)
	{
		if (UWidgetHelper::IsVisible(InVisibility))
		{
			LoadBrushImage();
		}
		else
		{
			
			UnloadBrushImage();
		}
	}
}

void UDungeonsImage::Serialize(FArchive& Ar)
{

#if WITH_EDITOR

	if (Ar.IsLoading())
	{
		Super::Serialize(Ar);

		int32 OnScreenLargeTextureWarning = CVarEnableSerialiseLargeTextureWarning.GetValueOnGameThread();

		//Large texture hard reference warning
		if (Brush.GetResourceObject())
		{
			int32 EstimatedResourceSize = Brush.GetResourceObject()->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal);

			if (EstimatedResourceSize >= (1048576)) //anything 1 Meg and over would be worth a soft reference
			{
				constexpr float BytesPerMiB = 1024.0f * 1024.0f;

				FString WarningMsg = FString::Format(TEXT(
					"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
					"||  {0}  || Has Hard references to image ||  {1}  ||  Estimated Memory Loss : {2} bytes ( {3} MB )\n"
					"This will drag this image into memory on switch!, please consider using soft references in the PreConstruct blueprint event to avoid this\n"
					"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"

				), { GetName(), Brush.GetResourceObject()->GetName(), FText::AsNumber(EstimatedResourceSize).ToString(), FText::AsNumber(EstimatedResourceSize / BytesPerMiB).ToString() });

				if (OnScreenLargeTextureWarning)
				{
					if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, WarningMsg);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("%s"), *WarningMsg);
				}

			}
		}

		//if we have made it into the load with an ObjectToLoad, make sure its fully loaded before continuing
		if (!ObjectToLoad.IsNull() && !Brush.GetResourceObject())
		{
			UAssetManager::GetStreamableManager().RequestSyncLoad(ObjectToLoad.ToSoftObjectPath(), true);
			Brush.SetResourceObject(ObjectToLoad.Get());
			ObjectToLoad.Reset();
		}
	}
	else if (Ar.IsSaving())
	{
		//make sure ObjectToLoad is loaded before serialization saving
		if (!ObjectToLoad.IsNull() && !Brush.GetResourceObject())
		{			
			UAssetManager::GetStreamableManager().RequestSyncLoad(ObjectToLoad.ToSoftObjectPath(), true);
			Brush.SetResourceObject(ObjectToLoad.Get());
			ObjectToLoad.Reset();
		}

		Super::Serialize(Ar);
	}
	else
	{
		Super::Serialize(Ar);
	}

#else

	Super::Serialize(Ar);

#endif //WITH_EDITOR

	OnObjectToRenderChanged();
}

#if WITH_EDITOR
void UDungeonsImage::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	UProperty* PropertyThatChanged = PropertyChangedEvent.Property;
	if (PropertyThatChanged)
	{
		const FName PropertyName = PropertyThatChanged->GetFName();
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UDungeonsImage, bRenderAsPowerOfTwo))
		{
			//Enable this if you want to preview how the widget will render

			OnRenderTextureModeChanged();
		}
		else if (PropertyName == GET_MEMBER_NAME_CHECKED(UDungeonsImage, Brush))
		{
			if (DynamicallyStreamAsset)
			{
				CancelTextureStreaming();

				//we have modified the brush
				if (Brush.GetResourceObject())
				{
					bIsTexture = Brush.GetResourceObject()->IsA(UTexture2D::StaticClass());
				}
				else
				{
					bIsTexture = false;
				}
			}
		}
	}

	OnObjectToRenderChanged();

}

void UDungeonsImage::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	
	auto LocalProperty = PropertyChangedEvent.PropertyChain.GetHead()->GetValue();
	auto ModifiedProperty = PropertyChangedEvent.PropertyChain.GetTail()->GetValue();

	const FName LocalPropertyName = LocalProperty->GetFName();
	if (LocalPropertyName == GET_MEMBER_NAME_CHECKED(UDungeonsImage, Brush))
	{
		const FName ModifiedPropertyName = ModifiedProperty->GetFName();
		if (ModifiedPropertyName == TEXT("ResourceObject")) //unreal being a pita for private objects here
		{
			if (Brush.GetResourceObject())
			{
				int32 EstimatedResourceSize = Brush.GetResourceObject()->GetResourceSizeBytes(EResourceSizeMode::EstimatedTotal);

				if (EstimatedResourceSize >= (1048576)) //anything 1 Meg and over would be worth a soft reference
				{
					constexpr float BytesPerMiB = 1024.0f * 1024.0f;

					FString WarningMsg = FString::Format(TEXT(
						"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
						"||  {0}  || Has Hard references to image ||  {1}  ||  Estimated Memory Loss : {2} bytes ( {3} MB )\n"
						"This will drag this image into memory on switch!, please consider using soft references in the PreConstruct blueprint event to avoid this\n"
						"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
						
					), { GetName(), Brush.GetResourceObject()->GetName(), FText::AsNumber(EstimatedResourceSize).ToString(), FText::AsNumber(EstimatedResourceSize / BytesPerMiB).ToString() });

					if (GEngine)
						GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Red, WarningMsg);

				}
			}
		}
	}

	UWidget::PostEditChangeChainProperty(PropertyChangedEvent);
}

#endif

void UDungeonsImage::SetColorAndOpacity(FLinearColor InColorAndOpacity)
{
	ColorAndOpacity = InColorAndOpacity;
	if (MyImage.IsValid())
	{
		MyImage->SetColorAndOpacity(ColorAndOpacity);
	}
}

void UDungeonsImage::SetOpacity(float InOpacity)
{
	ColorAndOpacity.A = InOpacity;
	if (MyImage.IsValid())
	{
		MyImage->SetColorAndOpacity(ColorAndOpacity);
	}
}

const FSlateBrush* UDungeonsImage::ConvertImage(TAttribute<FSlateBrush> InImageAsset) const
{
	UDungeonsImage* MutableThis = const_cast<UDungeonsImage*>(this);
	MutableThis->Brush = InImageAsset.Get();

	return &Brush;
}

void UDungeonsImage::SetBrush(const FSlateBrush& InBrush)
{
	CancelTextureStreaming();
	ObjectToLoad.Reset();

	if (Brush != InBrush)
	{
		Brush = InBrush;

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
			MyImage->Invalidate(EInvalidateWidget::LayoutAndVolatility);
		}

		if (Brush.GetResourceObject())
		{
			bIsTexture = Brush.GetResourceObject()->IsA(UTexture2D::StaticClass());			
		}
	}

	OnObjectToRenderChanged();
}

void UDungeonsImage::SetBrushSize(FVector2D DesiredSize)
{
	if (Brush.ImageSize != DesiredSize)
	{
		Brush.ImageSize = DesiredSize;

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
			MyImage->Invalidate(EInvalidateWidget::LayoutAndVolatility);
		}
	}
}

void UDungeonsImage::SetBrushTintColor(FSlateColor TintColor)
{
	if (Brush.TintColor != TintColor)
	{
		Brush.TintColor = TintColor;

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
			MyImage->Invalidate(EInvalidateWidget::PaintAndVolatility);
		}
	}
}

void UDungeonsImage::SetBrushFromAsset(USlateBrushAsset* Asset)
{
	if (!Asset || Brush != Asset->Brush)
	{
		CancelTextureStreaming();
		Brush = Asset ? Asset->Brush : FSlateBrush();
		bIsTexture = Brush.GetResourceObject()->IsA(UTexture2D::StaticClass());
		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
			MyImage->Invalidate(EInvalidateWidget::LayoutAndVolatility);
		}
		ObjectToLoad.Reset();
	}
}

void UDungeonsImage::SetBrushFromTexture(UTexture2D* Texture, bool bMatchSize)
{
	CancelTextureStreaming();
	ObjectToLoad.Reset();
	if (Brush.GetResourceObject() != Texture)
	{
		Brush.SetResourceObject(Texture);
		bIsTexture = true;

		if (Texture) // Since this texture is used as UI, don't allow it affected by budget.
		{
			Texture->bIgnoreStreamingMipBias = true;
		}

		if (bMatchSize)
		{
			if (Texture)
			{
				Brush.ImageSize = GetDesiredTextureBrushSize(*Texture);
			}
			else
			{
				Brush.ImageSize = FVector2D(0, 0);
			}
		}

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
			MyImage->Invalidate(EInvalidateWidget::LayoutAndVolatility);
		}

		OnObjectToRenderChanged();
	}
}

void UDungeonsImage::SetBrushFromAtlasInterface(TScriptInterface<ISlateTextureAtlasInterface> AtlasRegion, bool bMatchSize)
{
	if (AtlasRegion.GetObject() && Brush.GetResourceObject() != AtlasRegion.GetObject())
	{
		CancelTextureStreaming();
		ObjectToLoad.Reset();
		Brush.SetResourceObject(AtlasRegion.GetObject());
		bIsTexture = Brush.GetResourceObject()->IsA(UTexture2D::StaticClass());
		if (bMatchSize)
		{
			if (AtlasRegion)
			{
				FSlateAtlasData AtlasData = AtlasRegion->GetSlateAtlasData();
				Brush.ImageSize = AtlasData.GetSourceDimensions();
			}
			else
			{
				Brush.ImageSize = FVector2D(0, 0);
			}
		}

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
			MyImage->Invalidate(EInvalidateWidget::LayoutAndVolatility);
		}
	}
}

void UDungeonsImage::SetBrushFromTextureDynamic(UTexture2DDynamic* Texture, bool bMatchSize)
{
	if (Brush.GetResourceObject() != Texture)
	{
		CancelTextureStreaming();
		ObjectToLoad.Reset();
		Brush.SetResourceObject(Texture);
		bIsTexture = Brush.GetResourceObject()->IsA(UTexture2D::StaticClass());

		if (bMatchSize && Texture)
		{
			Brush.ImageSize.X = Texture->SizeX;
			Brush.ImageSize.Y = Texture->SizeY;
		}

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
			MyImage->Invalidate(EInvalidateWidget::LayoutAndVolatility);
		}
	}
}

void UDungeonsImage::SetBrushFromMaterial(UMaterialInterface* Material)
{
	if (Brush.GetResourceObject() != Material)
	{
		CancelTextureStreaming();
		ObjectToLoad.Reset();
		Brush.SetResourceObject(Material);
		bIsTexture = false;

		if (MyImage.IsValid())
		{
			MyImage->SetImage(&Brush);
			MyImage->Invalidate(EInvalidateWidget::LayoutAndVolatility);
		}
	}
}

void UDungeonsImage::CancelTextureStreaming()
{
	if (StreamingHandle.IsValid())
	{
		StreamingHandle->CancelHandle();
		StreamingHandle.Reset();
	}
}

void UDungeonsImage::SetBrushFromSoftSprite(TSoftObjectPtr<UObject> SoftPaperSprite, bool bMatchSize)
{
	if (auto* atlasRect = SoftPaperSprite.Get()) {
		SetBrushFromAtlasInterface(atlasRect, bMatchSize);
	} else {
		CancelTextureStreaming();

		TWeakObjectPtr<UDungeonsImage> WeakThis(this);

		StreamingHandle =
			UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftPaperSprite.ToSoftObjectPath(),
				[WeakThis, SoftPaperSprite, bMatchSize]() {
			if (WeakThis.IsValid()) {
				WeakThis->SetBrushFromAtlasInterface(SoftPaperSprite.Get(), bMatchSize);
			}
		}, FStreamableManager::AsyncLoadHighPriority);
	}
}

void UDungeonsImage::SetBrushFromSoftTexture(TSoftObjectPtr<UTexture2D> SoftTexture, bool bMatchSize)
{
	if (auto* tex = SoftTexture.Get()) {
		SetBrushFromTexture(tex, bMatchSize);
	} else {
		CancelTextureStreaming();

		TWeakObjectPtr<UDungeonsImage> WeakThis(this); // using weak ptr in case 'this' has gone out of scope by the time this lambda is called

		StreamingHandle =
			UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftTexture.ToSoftObjectPath(),
				[WeakThis, SoftTexture, bMatchSize]() {
			if (WeakThis.IsValid())
			{
				WeakThis->bIsTexture = true;
				WeakThis->SetBrushFromTexture(SoftTexture.Get(), bMatchSize);
				WeakThis->ObjectToLoad.Reset();
			}
		}, FStreamableManager::AsyncLoadHighPriority);
	}
}

void UDungeonsImage::SetBrushFromSoftTextureBlocking(TSoftObjectPtr<UTexture2D> SoftTexture, bool bMatchSize /*= false*/)
{
	if (auto* tex = SoftTexture.Get()) {
		SetBrushFromTexture(tex, bMatchSize);
	} else {
		CancelTextureStreaming();
		UAssetManager::GetStreamableManager().RequestSyncLoad(SoftTexture.ToSoftObjectPath());

		bIsTexture = true;
		SetBrushFromTexture(SoftTexture.Get(), bMatchSize);
		ObjectToLoad.Reset();
	}

}

void UDungeonsImage::SetBrushFromSoftTextureBlockingHighLow(TSoftObjectPtr<UTexture2D> SoftTextureHigh, TSoftObjectPtr<UTexture2D> SoftTextureLow, bool UseLow, bool bMatchSize /*= false*/)
{
	auto& toUse = (UseLow) ? SoftTextureLow : SoftTextureHigh;
	if (auto* tex = toUse.Get()) {
		SetBrushFromTexture(tex, bMatchSize);
	} else {
		SetBrushFromSoftTextureBlocking(toUse, bMatchSize);
	}
}

UMaterialInstanceDynamic* UDungeonsImage::GetDynamicMaterial()
{
	UMaterialInterface* Material = NULL;

	UObject* Resource = nullptr;

	//if we are unloaded, lets load before trying to get a material
	if (!ObjectToLoad.IsNull())
	{
		LoadBrushImage();
		CompleteTextureStreaming();
	}
	
	Resource = Brush.GetResourceObject();
	Material = Cast<UMaterialInterface>(Resource);

	if (Material)
	{
		bIsTexture = false;

		UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);

		if (!DynamicMaterial)
		{
			DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
			Brush.SetResourceObject(DynamicMaterial);
			ObjectToLoad.Reset();
			bIsTexture = false;
			if (MyImage.IsValid())
			{
				MyImage->SetImage(&Brush);
				MyImage->Invalidate(EInvalidateWidget::LayoutAndVolatility);
			}
		}

		return DynamicMaterial;
	}

	return NULL;
}

FReply UDungeonsImage::HandleMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (OnMouseButtonDownEvent.IsBound())
	{
		return OnMouseButtonDownEvent.Execute(Geometry, MouseEvent).NativeReply;
	}

	return FReply::Unhandled();
}

#if WITH_EDITOR

const FText UDungeonsImage::GetPaletteCategory()
{
	return LOCTEXT("Common", "Common");
}

#endif


/////////////////////////////////////////////////////


#undef LOCTEXT_NAMESPACE
