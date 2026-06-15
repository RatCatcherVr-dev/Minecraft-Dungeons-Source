#include "DungeonsTextCounter.h"

#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/InvalidationBox.h"

#include "DungeonsImage.h"
#include "DungeonsWidgetswitcher.h"

void UDungeonsTextCounterWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UDungeonsTextCounterWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UDungeonsTextCounterWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	
	if (Count != CountSmooth)
	{
		if (SmoothCountDelay > 0.0f)
		{
			SmoothCountDelay -= InDeltaTime; 
		}
		else
		{
			const double InterpolateSpeed = SMOOTH_COUNT_SPEED * (FMath::Abs(Count - CountSmooth) + 1.0f);			

			const double fNewVal = [&]{
				const double Dist = Count - CountSmooth;

				// If distance is too small, just set the desired location
				if (FMath::Square(Dist) < SMALL_NUMBER)
				{
					return Count;
				}

				const double Step = InterpolateSpeed * InDeltaTime;
				return CountSmooth + FMath::Clamp<double>(Dist, -Step, Step);
			}();
			
			const int32 iNewVal = fNewVal;

			if (iNewVal != lastSmoothRounded)
			{
				OnSmoothCountStep.Broadcast(lastSmoothRounded,iNewVal);
				lastSmoothRounded = iNewVal;
			}

			SetSmoothCount(fNewVal);
			SetCountingSmoothly(true);
		}

	}
	else
	{
		SetCountingSmoothly(false);
	}



	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UDungeonsTextCounterWidget::SetCounterInstant(int32 NewCount)
{
	Count = CountSmooth = NewCount;
	SetDisplayCount(Count);
}

void UDungeonsTextCounterWidget::SetCounterSmooth(int32 NewSmoothCount)
{
	Count = NewSmoothCount;
}

void UDungeonsTextCounterWidget::SetCountingSmoothly(bool smoothly)
{
	if (smoothly != counting_smoothly)
	{
		if(smoothly)
		{ 
			OnStartedCountingSmoothly.Broadcast();
		}
		else
		{
			OnStoppedCountingSmoothly.Broadcast();
		}

		counting_smoothly = smoothly;
	}
}

void UDungeonsTextCounterWidget::FinishSmoothCount()
{
	SetCounterInstant(Count);
	SetCountingSmoothly(false);
}

void UDungeonsTextCounterWidget::SetSmoothCountDelay(float delay)
{
	SmoothCountDelay = delay;
}

void UDungeonsTextCounterWidget::Invalidate()
{
	CounterTextInvBox->InvalidateCache();
}

bool UDungeonsTextCounterWidget::HasPendingSmoothCounting() const
{
	return Count != CountSmooth;
}

void UDungeonsTextCounterWidget::SetDisplayCount(float fVal)
{
	const int32 IntValue = FMath::Min((int32)fVal, maxDisplayCount);

	if(IntValue != mDisplayVal)
	{
		if (K_CappedDisplay && IntValue >= (minKDisplayCapValue*1000))
		{
			const float fRounded = IntValue * 0.001f;

			static const FNumberFormattingOptions NumberFormatOptions = FNumberFormattingOptions()
				.SetRoundingMode(ERoundingMode::ToZero)
				.SetMinimumIntegralDigits(1)
				.SetMaximumIntegralDigits(4)
				.SetMinimumFractionalDigits(2)
				.SetMaximumFractionalDigits(2);
		
			const FText NumString = FText::Format(FText::FromString("{0}k"), FText::AsNumber(fRounded, &NumberFormatOptions));

			CounterText->SetText(NumString);
		}
		else
		{
			static const FNumberFormattingOptions NumberFormatOptions = FNumberFormattingOptions()
				.SetMinimumIntegralDigits(1)
				.SetMaximumIntegralDigits(324);
			const FText NumString = FText::AsNumber(IntValue, &NumberFormatOptions);

			CounterText->SetText(NumString);
		}

		
		mDisplayVal = IntValue;
	}
}

void UDungeonsTextCounterWidget::SetSmoothCount(double fVal)
{
	CountSmooth = fVal;
	SetDisplayCount(fVal);
}

