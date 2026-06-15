// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "Dungeons.h"
#include "SDungeonsLoadingScreenWidget.h"

#include "SScaleBox.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBorder.h"
#include "game/difficulty/DifficultyUtil.h"
#include "game/mission/LoadingScreenDefs.h"
#include "game/mission/theme/MissionTheme.h"

#include "SSafeZone.h"
#include "SThrobber.h"
#include "SDPIScaler.h"
#include "Engine/Texture2D.h"
#include "Engine/UserInterfaceSettings.h"
#include <SharedPointer.h>
#include <SlateDynamicImageBrush.h>
#include <DeferredCleanupSlateBrush.h>
#include "SAnimatingImage.h"
#include <Internationalization.h>
#include "StringTableCore.h"
#include <StringTableRegistry.h>
#include "STransformAnimationWidget.h"
#include <SCanvas.h>
#include "STransformAnimationText.h"
#include "Widgets/Layout/SConstraintCanvas.h"



/////////////////////////////////////////////////////
// SDungeonsLoadingScreen

namespace game { namespace loadingscreen {

SDungeonsLoadingScreen::~SDungeonsLoadingScreen()
{
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("################### ~SDungeonsLoadingScreen ##################\n\n\n"));

}

FVector2D getSize(UTexture2D* tex) {
	return FVector2D(tex->GetSizeX(), tex->GetSizeY());
}

template<typename T>
TSharedRef<T> createBrush(const TSoftObjectPtr<UTexture2D>& softPtr, float scale, TOptional<FVector2D> overrideSize = {}) {
	auto texture = Cast<UTexture2D>(softPtr.LoadSynchronous());
	check(texture  && "Loading-screen texture asset not found");
	return T::CreateBrush(texture, overrideSize.Get(getSize(texture)) * scale);
}

template<typename T>
TSharedRef<T> createBrush(FSoftObjectPath softPath, float scale, TOptional<FVector2D> overrideSize = {}) {
	return createBrush<T>(TSoftObjectPtr<UTexture2D>(softPath), scale, overrideSize);
}



void SDungeonsLoadingScreen::Construct(const FArguments& InArgs, const FLoadingScreenDescription& InScreenDescription)
{
	LastComputedDPIScale = 1.0f;

	FVector2D viewportSize;
	GEngine->GameViewport->GetViewportSize(viewportSize);
	viewportSize = (viewportSize.X <= 0.0f) && (viewportSize.Y <= 0.0f) ? FVector2D(2000.0f, 2000.0f) : viewportSize;	

	TSharedRef<SOverlay> Root = SNew(SOverlay);

	{
		UTexture2D* BGTexture;

		FString AssetName = InScreenDescription.LevelMetaData.LevelImage.GetAssetName() + TEXT("_") + FPlatformProperties::IniPlatformName();
		FString PathString = InScreenDescription.LevelMetaData.LevelImage.GetAssetPathString();

		FString AssetPath;
		PathString.Split(TEXT("/"), &AssetPath, nullptr, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
		FString PlatFormAsset = AssetPath + TEXT("/") + AssetName + "." + AssetName;
		
		FSoftObjectPath PlatFormAssetPath(PlatFormAsset);
		BGTexture = Cast<UTexture2D>(PlatFormAssetPath.TryLoad());

		if (!BGTexture)
		{
			const auto& BGTextureAsset = InScreenDescription.LevelMetaData.LevelImage;
			BGTexture = Cast<UTexture2D>(BGTextureAsset.TryLoad());
		}		

		check(BGTexture && "BGTexture asset not found");

		if (!BGTexture)
		{
			UE_LOG(LogDungeons, Warning, TEXT("!!!! WARNING LOAD SCREEN %s Image has been defined but does not exist in package! Please ensure the asset for it has been cooked"), *InScreenDescription.LevelMetaData.LevelImage.ToString());
			FSoftObjectPath FallBackLS("/Game/UI/Materials/LoadingScreens/loadingGame.loadingGame");
			BGTexture = Cast<UTexture2D>(FallBackLS.TryLoad());
			check(BGTexture && "BGTexture asset not found");
		}

		FVector2D Size = FVector2D(BGTexture->GetSizeX(), BGTexture->GetSizeY());
		LoadingScreenBrush = FDeferredCleanupSlateBrush::CreateBrush(BGTexture, Size);

		Root->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SScaleBox)
				.Stretch(EStretch::Type::ScaleToFill)
				[
					SNew(SImage)
					.ColorAndOpacity(FLinearColor(0, 0, 0, 1))
					.Image(LoadingScreenBrush->GetSlateBrush())
				]
			];

		Root->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SScaleBox)
				.Stretch(InScreenDescription.ImageStretch)
				[
					SNew(STransformAnimationWidget)
					.Image(LoadingScreenBrush->GetSlateBrush())
					.TransformationSequence(
					{
						{
							{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({0.0f, 0.0f}, {0.0f, 0.0f}), FInterpolationData<float>(1.0f, 1.5f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 40.0f, ECurveEaseFunction::Linear },
						},
						false
					})
				]
			];

		
	}
	
	LoadingAnimationBrush = createBrush<FMutableDeferredCleanupSlateBrush>(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Misc/loader.loader"), 1.f / 1.85f, FVector2D(240.f, 240.f));

	const EMissionTheme theme = missions::get(InScreenDescription.LevelMetaData.LevelID).theme();

	auto ThreatLevelFont = InScreenDescription.ThreatLevelFont;
	FText ThreatLevelText;
	auto difficultyColor = InScreenDescription.Style.DifficultyColor;
	auto difficultyShadowColor = InScreenDescription.Style.DifficultyShadowColor;

	if (missions::get(InScreenDescription.LevelSettings.getLevelName()).levelVariation() != ELevelVariationType::Invalid) {
		auto& ThreatLevelFlagPath = InScreenDescription.Style.ThreatLevelFlag;
		if (!ThreatLevelFlagPath.IsNull()) {

			ThreatLevelBadgeBrush = createBrush<FDeferredCleanupSlateBrush>(ThreatLevelFlagPath, 0.5f);
			if (InScreenDescription.LevelSettings.getEndlessStruggle().Value > 0) {
				DifficultyBrush = createBrush<FDeferredCleanupSlateBrush>(FSoftObjectPath("/Game/UI/Materials/Difficulty/asset_levelselect_level6.asset_levelselect_level6"), 0.6f);
				difficultyColor = { 1.0f, 1.0f, 1.0f, 1.0f };
				difficultyShadowColor = { 0.0f, 0.0f, 0.0f, 0.5f };
				//There is an extra space here to center the number.
				ThreatLevelText = FText::Format(INVTEXT("+{0}  "), FText::FromString(FString::FromInt(InScreenDescription.LevelSettings.getEndlessStruggle().Value)));
				ThreatLevelFont = InScreenDescription.ThreatLevelPlusFont;
			}
			else {
				ThreatLevelText = UDifficultyUtil::getThreatLevelDisplayName(InScreenDescription.LevelSettings.getThreatLevel());
				if (auto* difficultyIconPath = InScreenDescription.Style.Difficulty.Find(InScreenDescription.LevelSettings.getDifficulty())) {
					DifficultyBrush = createBrush<FDeferredCleanupSlateBrush>(*difficultyIconPath, 0.35f);
				}
			}
		}

		auto& badgePath = InScreenDescription.Style.Badge;
		if (!badgePath.IsNull()) {
			DifficultyBadgeBrush = createBrush<FDeferredCleanupSlateBrush>(badgePath, InScreenDescription.Style.BadgeScale);
		}

		if (auto* wingPath = InScreenDescription.Style.ExtraChallenge.wings.Find(InScreenDescription.LevelSettings.getExtraChallenge())) {
			RightWingBrush = createBrush<FDeferredCleanupSlateBrush>(*wingPath, 0.3f);
		}		
	}
	
	const auto threatShadowColor = InScreenDescription.Style.ThreatLevelShadowColor;
	const float textVerticalOffset = ThreatLevelBadgeBrush ? 70.0f : 20.0f;
	const float shadowOffset = 2.5f;
	const float flagOffset = 15.f;
	const auto difficultyShadowOffset = InScreenDescription.Style.DifficultyShadowOffset;


	//TSharedRef<SWidget> TipWidget = SNullWidget::NullWidget;
	//if (InScreenDescription.Tips.Num() > 0)
	//{
	//	int32 TipIndex = FMath::RandRange(0, InScreenDescription.Tips.Num() - 1);

	//	TipWidget = SNew(STextBlock)
	//		.WrapTextAt(InScreenDescription.TipWrapAt)
	//		//.Font(Settings->TipFont)
	//		.Text(InScreenDescription.Tips[TipIndex]);
	//}
	
	// set up user facing formatted loading screen text:
	FFormatNamedArguments LoadLevelTextArgs;
	LoadLevelTextArgs.Add("level", InScreenDescription.LevelMetaData.LevelName);
	LoadLevelTextArgs.Add("description", InScreenDescription.LevelMetaData.LevelDescription);

	FText LoadingScreenLevelMessage = FText::Format(game::loadingscreen::LoadingScreenLevel(), LoadLevelTextArgs);
	FText TravelingToText = game::loadingscreen::LoadingScreenTravelingTo();
	FText LoadingScreenDailyTrialMessage = UMissionDefs::GetMissionTrialText(InScreenDescription.LevelMetaData.LevelID);

	float loadingPlateDelay = InScreenDescription.LoadingPlateDisplayDelay;
	
	Root->AddSlot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Bottom)
		.Padding(FMargin(50.0f, 50.0f))
		[
			SNew(SSafeZone)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Bottom)
			.IsTitleSafe(false)
			[
				SNew(SDPIScaler)
				.DPIScale(this, &SDungeonsLoadingScreen::GetDPIScale)
				[
					SNew(SOverlay)

					+ SOverlay::Slot()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Left)
						.AutoWidth()
						[
							SNew(SSpacer)
							.Size(FVector2D(20.0f, 0.0f))
						]

						+ SHorizontalBox::Slot()
						.Padding(FMargin(0, 0, 20, 0))
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Left)
						.AutoWidth()
						[
							SNew(SAnimatingImage)
							.Image(LoadingAnimationBrush->GetMutableSlateBrush())
							.AnimationData({ true, 8, 8, 59, 0.05f, 240.f, 240.f, 2048, 2048 })
							.TransformationSequence(
								{
									{
										{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 0.f}, {1.f, 1.f, 1.f, 0.f}), FInterpolationData<FVector2D>({0.0f, 0.0f}, {0.0f, 0.0f}), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay + 0.1f, ECurveEaseFunction::Linear },
										{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 0.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({0.0f, 0.0f}, {0.0f, 0.0f}), FInterpolationData<float>(1.0f, 1.2f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.5f, ECurveEaseFunction::Linear },
									},
									false
								})
						]

						+ SHorizontalBox::Slot()
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Left)
						.AutoWidth()
						[
							SNew(SSpacer)
							.Size(FVector2D(20.0f, 0.0f))
						]

						+ SHorizontalBox::Slot()
						//.Padding(FMargin(20.0f, 0.0f))
						.AutoWidth()
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Left)
						[
							SNew(SVerticalBox)

							+ SVerticalBox::Slot()
							.AutoHeight()
							.VAlign(VAlign_Top)
							.HAlign(HAlign_Left)
							[
								SNew(STransformAnimationTextBlock)
								.Text(TravelingToText)
								.Font(InScreenDescription.TravelingToFont)
								.ColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
								.ShadowColorAndOpacity(FColor(0, 0, 0, 150))
								.ShadowOffset(FVector2D(5.f, 5.f))
								.TransformationSequence(
								{
									{
										{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 0.f}, {1.f, 1.f, 1.f, 0.f}), FInterpolationData<FVector2D>({0.0f, -10.0f}, {0.0f, -10.0f}), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay + 0.1f, ECurveEaseFunction::Linear },
										{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 0.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({0.0f, -10.0f}, {0.0f,  0.0f}), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.5f, ECurveEaseFunction::Linear },
									},
									false
								})
							]

							+ SVerticalBox::Slot()
							.AutoHeight()
							.VAlign(VAlign_Top)
							.HAlign(HAlign_Left)
							[
								SNew(STransformAnimationTextBlock)
								.Text(LoadingScreenLevelMessage)
								.Font(InScreenDescription.LevelNameFont)
								.ColorAndOpacity(FColor(255, 207, 77, 255))
								.ShadowColorAndOpacity(FColor(250, 102, 17, 255))
								.ShadowOffset(FVector2D(2.5f, 2.5f))
								.TransformationSequence(
									{
										{
											{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 0.f}, {1.f, 1.f, 1.f, 0.f}), FInterpolationData<FVector2D>({0.0f, 5.0f}, {0.0f, 5.0f}), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay + 0.1f, ECurveEaseFunction::Linear },
											{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 0.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({0.0f, 5.0f}, {0.0f, -5.0f}), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.5f, ECurveEaseFunction::Linear },
										},
										false
									})
							]
						]
					]
				]
			]
		];
	
	Root->AddSlot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Top)
		.Padding(FMargin(50.0f, 50.0f))
		[
			SNew(SSafeZone)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Bottom)
			.IsTitleSafe(false)
			[
				SNew(SDPIScaler)
				.DPIScale(this, &SDungeonsLoadingScreen::GetDPIScale)
				[

					SNew(SOverlay)

					+ SOverlay::Slot()
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						.AutoWidth()
						.HAlign(HAlign_Left)
						.VAlign(VAlign_Center)
						.Padding(FMargin(10.0f, 0.0f))
						[
							SNew(SVerticalBox)

							+ SVerticalBox::Slot()
							.AutoHeight()
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							[
								SNew(SOverlay)

								//Trial wings shadow
								+ SOverlay::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STransformAnimationWidget)
									.Image(RightWingBrush ? RightWingBrush->GetSlateBrush() : nullptr)
									.TransformationSequence(
										{
											{
												{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.5f}, {0.f, 0.f, 0.f, 0.5f}), FInterpolationData<FVector2D>({40.0f, 10.0f}, {40.0f, 10.0f}), FInterpolationData<float>(0.0f, 0.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.5f}, {0.f, 0.f, 0.f, 0.5f}), FInterpolationData<FVector2D>({44.0f, 10.0f}, {44.0f, 10.0f}), FInterpolationData<float>(0.0f, 1.3f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.1f, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.5f}, {0.f, 0.f, 0.f, 0.5f}), FInterpolationData<FVector2D>({40.0f, 10.0f}, {40.0f, 10.0f}), FInterpolationData<float>(1.3f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 1.f, ECurveEaseFunction::Linear },
											},
											false
										})
									.RenderTransform(FSlateRenderTransform(FScale2D(-1.0f, 1.0f)))
									.RenderTransformPivot(FVector2D(0.5f, 0.5f))
									.Visibility(RightWingBrush ? EVisibility::Visible : EVisibility::Collapsed)
								]

								+ SOverlay::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STransformAnimationWidget)
									.Image(RightWingBrush ? RightWingBrush->GetSlateBrush() : nullptr)
									.TransformationSequence(
										{
											{
												{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.5f}, {0.f, 0.f, 0.f, 0.5f}), FInterpolationData<FVector2D>({40.0f, 10.0f}, {40.0f, 10.0f}), FInterpolationData<float>(0.0f, 0.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.5f}, {0.f, 0.f, 0.f, 0.5f}), FInterpolationData<FVector2D>({44.0f, 10.0f}, {44.0f, 10.0f}), FInterpolationData<float>(0.0f, 1.3f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.1f, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.5f}, {0.f, 0.f, 0.f, 0.5f}), FInterpolationData<FVector2D>({40.0f, 10.0f}, {40.0f, 10.0f}), FInterpolationData<float>(1.3f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 1.f, ECurveEaseFunction::Linear },
											},
											false
										})
									.RenderTransform(FSlateRenderTransform(FScale2D(1.0f, 1.0f)))
									.RenderTransformPivot(FVector2D(0.5f, 0.5f))
									.Visibility(RightWingBrush ? EVisibility::Visible : EVisibility::Collapsed)
								]

								//Trial wings
								+ SOverlay::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STransformAnimationWidget)
									.Image(RightWingBrush ? RightWingBrush->GetSlateBrush() : nullptr)
									.TransformationSequence(
										{
											{												
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({33.0f, 0.0f}, {33.0f, 0.0f}), FInterpolationData<float>(0.0f, 0.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({36.0f, 0.0f}, {36.0f, 0.0f}), FInterpolationData<float>(0.0f, 1.3f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.1f, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({33.0f, 0.0f}, {33.0f, 0.0f}), FInterpolationData<float>(1.3f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 1.f, ECurveEaseFunction::Linear },
											},
											false
										})
									.RenderTransform(FSlateRenderTransform(FScale2D(-1.0f, 1.0f)))
									.RenderTransformPivot(FVector2D(0.5f, 0.5f))
									.Visibility(RightWingBrush ? EVisibility::Visible : EVisibility::Collapsed)
								]

								+ SOverlay::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STransformAnimationWidget)
									.Image(RightWingBrush ? RightWingBrush->GetSlateBrush() : nullptr)
									.TransformationSequence(
										{
											{
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({33.0f, 0.0f}, {33.0f, 0.0f}), FInterpolationData<float>(0.0f, 0.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({36.0f, 0.0f}, {36.0f, 0.0f}), FInterpolationData<float>(0.0f, 1.3f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.1f, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({33.0f, 0.0f}, {33.0f, 0.0f}), FInterpolationData<float>(1.3f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 1.f, ECurveEaseFunction::Linear },											
											},
											false
										})
									.RenderTransform(FSlateRenderTransform(FScale2D(1.0f, 1.0f)))
									.RenderTransformPivot(FVector2D(0.5f, 0.5f))
									.Visibility(RightWingBrush ? EVisibility::Visible : EVisibility::Collapsed)
								]

								// Threat level badge shadow
								+ SOverlay::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STransformAnimationWidget)
									.Image(ThreatLevelBadgeBrush ? ThreatLevelBadgeBrush->GetSlateBrush() : nullptr)
									.TransformationSequence(
										{
											{
												{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.0f}, {0.f, 0.f, 0.f, 0.0f}), FInterpolationData<FVector2D>({0.0f, 0.0f}, {0.0f, 0.0f}), FInterpolationData<float>(0.0f, 0.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay + 1.0f, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.0f}, {0.f, 0.f, 0.f, 0.5f}), FInterpolationData<FVector2D>({0.0f, flagOffset+40.0f}, {0.0f, flagOffset+50.0f}), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.2f, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.5f}, {0.f, 0.f, 0.f, 0.5f}), FInterpolationData<FVector2D>({0.0f, flagOffset+50.0f}, {0.0f, flagOffset+30.0f}), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.5f, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.5f}, {0.f, 0.f, 0.f, 0.5f}), FInterpolationData<FVector2D>({0.0f, flagOffset+30.0f}, {0.0f, flagOffset+35.0f}), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.5f, ECurveEaseFunction::Linear },
											},
											false
										})
									.Visibility(ThreatLevelBadgeBrush ? EVisibility::Visible : EVisibility::Collapsed)
								]

								// Threat level badge
								+ SOverlay::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STransformAnimationWidget)
									.Image(ThreatLevelBadgeBrush ? ThreatLevelBadgeBrush->GetSlateBrush() : nullptr)
									.Visibility(ThreatLevelBadgeBrush ? EVisibility::Visible : EVisibility::Collapsed)
									.TransformationSequence(
										{
											{
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 0.f}, {1.f, 1.f, 1.f, 0.f}), FInterpolationData<FVector2D>({0.0f, 0.0f}, {0.0f, 0.0f}), FInterpolationData<float>(0.0f, 0.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay + 1.0f, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 0.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({0.0f, flagOffset+30}, {0.0f, flagOffset+40.0f}), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.2f, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({0.0f, flagOffset+40.0f}, {0.0f, flagOffset+20.0f}), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.5f, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({0.0f, flagOffset+20.0f}, {0.0f, flagOffset+25.0f}), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.5f, ECurveEaseFunction::Linear },
											},
											false
										})
								]								
																
								// Threat level ( and Endless Struggle )
								+ SOverlay::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STransformAnimationTextBlock)
									.Text(ThreatLevelText)
									.Visibility(!ThreatLevelText.IsEmpty() ? EVisibility::Visible : EVisibility::Collapsed)
									.Font(ThreatLevelFont)
									.ColorAndOpacity(FColor::Black)
									.ShadowColorAndOpacity(threatShadowColor)
									.ShadowOffset(FVector2D(0.0f, shadowOffset))
									.TransformationSequence(
										{
											{
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 0.f}, {1.f, 1.f, 1.f, 0.f}), FInterpolationData<FVector2D>({0.0f, 0.0f}, {0.0f, 0.0f}), FInterpolationData<float>(0.0f, 0.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay + 1.0f, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 0.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({0.0f, 70.0f}, {0.0f, 80.0f}), FInterpolationData<float>(0.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.2f, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({0.0f, 80.0f}, {0.0f, 60.0f}), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.5f, ECurveEaseFunction::Linear },
												{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({0.0f, 60.0f}, {0.0f, 65.0f}), FInterpolationData<float>(1.0f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.5f, ECurveEaseFunction::Linear },
											},
											false
										})
								]

								// Difficulty Badge Shadow 
								+ SOverlay::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STransformAnimationWidget)
									.Image(DifficultyBadgeBrush ? DifficultyBadgeBrush->GetSlateBrush() : nullptr)
									.TransformationSequence(
									{
										{
											{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.f}, {0.f, 0.f, 0.f, 0.5f}), FInterpolationData<FVector2D>({0.0f, 10.0f}, {0.0f, 10.0f}), FInterpolationData<float>(0.0f, 0.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay, ECurveEaseFunction::QuadIn },
											{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.5f}, {0.f, 0.f, 0.f, 0.5f}), FInterpolationData<FVector2D>({0.0f, 10.0f}, {0.0f, 10.0f}), FInterpolationData<float>(0.0f, 1.2f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.1f, ECurveEaseFunction::QuadIn },
											{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.5f}, {0.f, 0.f, 0.f, 0.5f}), FInterpolationData<FVector2D>({0.0f, 10.0f}, {0.0f, 10.0f}), FInterpolationData<float>(1.2f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 1.f, ECurveEaseFunction::QuadIn }
										},
										false
									})
									.Visibility(DifficultyBadgeBrush ? EVisibility::Visible : EVisibility::Collapsed)
								]

								// Difficulty Badge
								+ SOverlay::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STransformAnimationWidget)
									.Image(DifficultyBadgeBrush ? DifficultyBadgeBrush->GetSlateBrush() : nullptr)
									.TransformationSequence(
									{
										{
											{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({0.0f, 0.0f}, {0.0f, 0.0f}), FInterpolationData<float>(0.0f, 0.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay, ECurveEaseFunction::QuadIn },
											{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({0.0f, 0.0f}, {0.0f, 0.0f}), FInterpolationData<float>(0.0f, 1.2f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.1f, ECurveEaseFunction::QuadIn },
											{ this->AsShared(), FInterpolationData<FLinearColor>({1.f, 1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}), FInterpolationData<FVector2D>({0.0f, 0.0f}, {0.0f, 0.0f}), FInterpolationData<float>(1.2f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 1.f, ECurveEaseFunction::QuadIn }
										},
										false
									})
									.Visibility(DifficultyBadgeBrush ? EVisibility::Visible : EVisibility::Collapsed)
								]
								//Difficulty shadow
								+ SOverlay::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STransformAnimationWidget)
									.Image(DifficultyBrush ? DifficultyBrush->GetSlateBrush() : nullptr)
								.TransformationSequence(
									{
										{
											{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.f}, difficultyShadowColor), FInterpolationData<FVector2D>(difficultyShadowOffset, difficultyShadowOffset), FInterpolationData<float>(0.0f, 0.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay, ECurveEaseFunction::QuadIn },
											{ this->AsShared(), FInterpolationData<FLinearColor>(difficultyShadowColor, difficultyShadowColor), FInterpolationData<FVector2D>(difficultyShadowOffset, difficultyShadowOffset), FInterpolationData<float>(0.0f, 1.2f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.1f, ECurveEaseFunction::QuadIn },
											{ this->AsShared(), FInterpolationData<FLinearColor>(difficultyShadowColor, difficultyShadowColor), FInterpolationData<FVector2D>(difficultyShadowOffset, difficultyShadowOffset), FInterpolationData<float>(1.2f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 1.f, ECurveEaseFunction::QuadIn }
										},
										false
									})
								.Visibility(DifficultyBrush && !difficultyShadowOffset.IsNearlyZero() ? EVisibility::Visible : EVisibility::Collapsed)
								]

								//Difficulty
								+ SOverlay::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Center)
								[
									SNew(STransformAnimationWidget)
									.Image(DifficultyBrush ? DifficultyBrush->GetSlateBrush() : nullptr)
									.TransformationSequence(
									{
										{
											{ this->AsShared(), FInterpolationData<FLinearColor>({0.f, 0.f, 0.f, 0.f}, difficultyColor), FInterpolationData<FVector2D>({0.0f, 0.0f}, {0.0f, 0.0f}), FInterpolationData<float>(0.0f, 0.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), loadingPlateDelay, ECurveEaseFunction::QuadIn },
											{ this->AsShared(), FInterpolationData<FLinearColor>(difficultyColor, difficultyColor), FInterpolationData<FVector2D>({0.0f, 0.0f}, {0.0f, 0.0f}), FInterpolationData<float>(0.0f, 1.2f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 0.1f, ECurveEaseFunction::QuadIn },
											{ this->AsShared(), FInterpolationData<FLinearColor>(difficultyColor, difficultyColor), FInterpolationData<FVector2D>({0.0f, 0.0f}, {0.0f, 0.0f}), FInterpolationData<float>(1.2f, 1.0f), FInterpolationData<FBox2D>(FBox2D(-viewportSize, viewportSize), FBox2D(-viewportSize, viewportSize)), 1.f, ECurveEaseFunction::QuadIn }
										},
										false
									})
									.Visibility(DifficultyBrush ? EVisibility::Visible : EVisibility::Collapsed)
								]
							]

							+ SVerticalBox::Slot()
							.Padding(FMargin(0, textVerticalOffset))
							.AutoHeight()
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
								.ShadowColorAndOpacity(FColor(0, 0, 0, 150))
								.ShadowOffset(FVector2D(2.5f, 2.5))
								.Text(LoadingScreenDailyTrialMessage)
								.Font(InScreenDescription.DifficultyFont)
							]							
						]
					]
				]
			]
		];

	ChildSlot
	[
		Root
	];
	
}

void SDungeonsLoadingScreen::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	const FVector2D& LocalSize = AllottedGeometry.GetLocalSize();
	FIntPoint Size((int32)LocalSize.X, (int32)LocalSize.Y);
	const float NewScale = GetDefault<UUserInterfaceSettings>()->GetDPIScaleBasedOnSize(Size);

	if (NewScale != LastComputedDPIScale)
	{
		LastComputedDPIScale = NewScale;
		SlatePrepass(1.0f);
	}
}

float SDungeonsLoadingScreen::GetDPIScale() const
{
	return LastComputedDPIScale;
}

}}

