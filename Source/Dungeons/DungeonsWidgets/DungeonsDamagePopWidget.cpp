#include "DungeonsDamagePopWidget.h"

#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "game/component/HealthComponent.h"

void UDungeonsDamagePopWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SetAlignmentInViewport(FVector2D(0.5f,0.5f));
}

void UDungeonsDamagePopWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (mLifeTime > 0.0f)
	{
		mLifeTime -= InDeltaTime;
		const float fLifeProgress = FMath::Clamp((mLifeTime / mDuration), 0.0f, 1.0f );

		{
			//Modify Screen Position
			//velocity
			const FVector2D VelocityDiff = mScreenPerspective * mScreenVelocity * (mVelocityFactor * InDeltaTime);
			mScreenLocation += VelocityDiff;

			UpdateOnScreenLocation();

			//Friction
			const float Friction = FMath::Max(1.0f - (mFrictionFactor * mFriction * InDeltaTime), 0.0f);
			mScreenVelocity *= Friction;

			//Gravity
			const float fGravity = mGravity * mGravityFactor * FMath::Pow(fLifeProgress,2.0f) * InDeltaTime;
			mScreenVelocity.Y += fGravity;

			//Scaling
			if (mScaleTime > 0.0f)
			{
				mScaleTime -= InDeltaTime;
				const float Scale = FMath::Max(1.0f + (mScaleTime * 0.625f), 0.0f);

				Text->SetRenderScale(FVector2D(Scale, Scale));
			}
		}

		{
			//Update Colour / fade
			if (fLifeProgress <= 0.25f)
			{
				Text->SetRenderOpacity(fLifeProgress * 4.0f);
			}
		}

		if (mLifeTime <= 0.0f)
		{
			mLifeTime = 0.0f;
			SetVisibility(ESlateVisibility::Collapsed);
			SetIsEnabled(false);
			OnDamagePopCompleted.Broadcast(this);
		}
	}

}

void UDungeonsDamagePopWidget::UpdateOnScreenLocation()
{
	if (auto* pPanelSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		pPanelSlot->SetPosition(mScreenLocation);
	}
}

void UDungeonsDamagePopWidget::SetupDamageVal(float fTextValue)
{
	mIsText = false;
	int32 IntValue = UHealthComponent::GetHealthAsDisplayAmount(fTextValue);

	if (mCachedVal != IntValue)
	{
		mCachedVal = IntValue;
		static const FNumberFormattingOptions NumberFormatOptions = FNumberFormattingOptions()
			.SetMinimumIntegralDigits(1)
			.SetMaximumIntegralDigits(324);
		const FText NumString = FText::AsNumber(IntValue, &NumberFormatOptions);
		Text->SetText(NumString);
	}
}

void UDungeonsDamagePopWidget::SetupTextVal(const FText& fText)
{
	mIsText = false;
	mCachedVal = -1;
	Text->SetText(fText);
}

void UDungeonsDamagePopWidget::LaunchPop()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetIsEnabled(true);
	UpdateOnScreenLocation();
}

void UDungeonsDamagePopWidget::SetupPopVals(float fDuration, float fScale, float fGravity, float fFriction, const FLinearColor& TextColour, const FLinearColor& ShadowColour, const FVector2D& InitialScreenPosition, const FVector2D& InitialScreenVelocity)
{
	mScreenLocation = InitialScreenPosition;
	mScreenVelocity = InitialScreenVelocity;

	mLifeTime = fDuration;
	mDuration = fDuration;

	mGravity = fGravity;
	mFriction = fFriction;

	Text->SetShadowColorAndOpacity(ShadowColour);
	Text->SetColorAndOpacity(FSlateColor(TextColour));
	Text->SetRenderScale(FVector2D(1.0f,1.0f));
	Text->SetRenderOpacity(1.0f);

	const float fUpVelocity = mScreenVelocity.Size() * mYLaunchFactor;
	mScreenVelocity.Y += fUpVelocity;

	mScaleTime = 0.4f;

	const float fFontSize = 20.0f;
	const FVector2D ShadowFactor(0.2f,0.2f);
	FSlateFontInfo TextInfo = Text->Font;
	TextInfo.Size = (int32)(fFontSize * fScale);

	Text->SetFont(TextInfo);
	Text->SetShadowOffset(ShadowFactor * (fFontSize * fScale));
}
