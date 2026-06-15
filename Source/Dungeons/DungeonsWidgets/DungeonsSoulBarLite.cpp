#include "DungeonsSoulBarLite.h"

#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/InvalidationBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Spacer.h"
#include "game/item/ItemSlot.h"
#include "game/item/instance/AItemInstance.h"
#include "game/item/ItemFunctionLibrary.h"
#include "game/component/SoulComponent.h"

#include "DungeonsImage.h"
#include "DungeonsWidgetswitcher.h"

void UDungeonsSoulBarLite::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UDungeonsSoulBarLite::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	
	UpdateAnimTimers(InDeltaTime);
	UpdateUsageIndicator(InDeltaTime);

	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UDungeonsSoulBarLite::UpdateAnimTimers(float fDelta)
{
	//Face Glow Anim
	if (mFaceGlowCountDown > 0.0f)
	{
		const float fVal = FMath::Min(mFaceGlowCountDown * FACE_GLOW_FACTOR, 1.0f);
		iconPop->SetOpacity(fVal);
		mFaceGlowCountDown -= fDelta;
		if (mFaceGlowCountDown <= 0.0f)
		{
			mFaceGlowCountDown = 0.0f;
			iconPop->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	//Bar Glow Anim
	if (mBarGlowCountDown > 0.0f)
	{
		const float fVal = FMath::Min(mBarGlowCountDown * BAR_GLOW_FACTOR, 1.0f);

		barGlow->SetOpacity(fVal);
		mBarGlowCountDown -= fDelta;
		if (mBarGlowCountDown <= 0.0f)
		{
			mBarGlowCountDown = 0.0f;
			barGlow->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			const float ScaleVal = FMath::Sin(2.0f * mBarGlowCountDown) * 0.03f;
			barGlow->SetRenderScale(FVector2D(1.0f + ScaleVal, 1.0f - ScaleVal));
		}
	}


	//Missing Souls Anim
	if (mMissingGlowCountDown > 0.0f)
	{
		const float fVal = FMath::Clamp( mMissingGlowCountDown/mMissingGlowTime, 0.0f, 1.0f);

		MissingSoulsIndicator->SetRenderOpacity(fVal);
		mMissingGlowCountDown -= fDelta;
		if (mMissingGlowCountDown <= 0.0f)
		{
			mMissingGlowCountDown = 0.0f;
			MissingSoulsIndicator->SetVisibility(ESlateVisibility::Collapsed);
		}
	}


	if (mNumberCountDown > 0.0f)
	{
		mNumberCountDown -= fDelta;

		const float fCurveVal = (NumberCurve) ? NumberCurve->GetFloatValue(1.0f - mNumberCountDown) : (1.0f - mNumberCountDown); //just in case...

		if (TargetText)
		{
			TargetText->SetRenderOpacity(fCurveVal);
			TargetText->SetShadowColorAndOpacity(FLinearColor(0,0,0, fCurveVal*0.7f));
			TargetText->SetShadowOffset(FVector2D(0, fCurveVal*3.0f));
			TargetText->SetRenderTranslation(FVector2D(fCurveVal * NumberMovement, 0.0f));
		}

		if (mNumberCountDown <= 0.0f)
		{
			mNumberCountDown = 0.0f;
			TargetText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (mFullShineCountDown > 0.0f)
	{
		const float fOpacity = (mFullShineCountDown >= 0.25f) ? FMath::GetMappedRangeValueClamped(FVector2D(0.75f, 0.25f), FVector2D(0.0f, 1.0f), mFullShineCountDown):
																FMath::GetMappedRangeValueClamped(FVector2D(0.25f, 0.00f), FVector2D(1.0f, 0.0f), mFullShineCountDown);
		FullShine->SetRenderOpacity(fOpacity);

		mFullShineCountDown -= fDelta;
		if (mFullShineCountDown <= 0.0f)
		{
			mFullShineCountDown = 0.0f;
			FullShine->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UDungeonsSoulBarLite::UpdateUsageIndicator(float fDelta)
{
	if (IndicatorPercentage > CurrentPercentage)
	{
		if (mUsageGlowCountDown > 0.0f)
		{
			mUsageGlowCountDown -= fDelta;
			UsageIndicator->SetRenderScale(FVector2D(IndicatorPercentage, 1.0f));

			if (!UsageIndicator->IsVisible())
			{
				UsageIndicator->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
		else
		{

			if (FMath::IsNearlyEqual(CurrentPercentage, IndicatorPercentage, 0.01f))
			{
				StopUsageIndicator(CurrentPercentage);
			}
			else
			{
				IndicatorPercentage = FMath::Lerp(IndicatorPercentage, CurrentPercentage,0.2f);
				UsageIndicator->SetRenderScale(FVector2D(IndicatorPercentage, 1.0f));
			}
		}
	}
}

void UDungeonsSoulBarLite::StartFaceGlow()
{
	iconPop->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	mFaceGlowCountDown = 0.75f;
}

void UDungeonsSoulBarLite::StartBarGlow()
{
	barGlow->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	mBarGlowCountDown = 1.0f;
}

void UDungeonsSoulBarLite::StartNumberAnim()
{
	mNumberCountDown = 1.0f;
	TargetText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UDungeonsSoulBarLite::StartFullShine()
{
	mFullShineCountDown = 0.75f;
	FullShine->SetRenderOpacity(0.0f);
	FullShine->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UDungeonsSoulBarLite::StartMissingSoulsGlow(float fPercentNeeded)
{
	mMissingGlowCountDown = mMissingGlowTime;
	MissingSoulsIndicator->SetRenderScale(FVector2D(fPercentNeeded,1.0f));
	MissingSoulsIndicator->SetRenderOpacity(1.0f);
	MissingSoulsIndicator->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}


void UDungeonsSoulBarLite::StopUsageIndicator(float fPercent)
{
	IndicatorPercentage = fPercent;
	UsageIndicator->SetVisibility(ESlateVisibility::Collapsed);
	UsageIndicator->SetRenderScale(FVector2D(0.0f, 1.0f));
	mUsageGlowCountDown = 0.5f;
}

void UDungeonsSoulBarLite::SoulPercentageChanged(USoulComponent* SoulComponent)
{
	if (SoulComponent)
	{
		const float fSoulPercentage = SoulComponent->GetSoulPercentage();

		FSlateChildSize SlateSize(ESlateSizeRule::Fill);
		SlateSize.Value = 1.0f - fSoulPercentage;
		Cast<UHorizontalBoxSlot>(EmptyFill->Slot)->SetSize(SlateSize);
		SlateSize.Value = fSoulPercentage;
		Cast<UHorizontalBoxSlot>(MeterFill->Slot)->SetSize(SlateSize);


		if (fSoulPercentage > IndicatorPercentage)
		{
			StopUsageIndicator(fSoulPercentage);
		}

		if (fSoulPercentage >= 1.0f && CurrentPercentage < 1.0f)
		{
			StartFullShine();
		}

		CurrentPercentage = fSoulPercentage;
	}
}

