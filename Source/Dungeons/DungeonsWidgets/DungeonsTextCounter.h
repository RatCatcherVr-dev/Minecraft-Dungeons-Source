#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DungeonsTextCounter.generated.h"

class UTextBlock;
class UCanvasPanel;
class UDungeonsImage;
class UDungeonsWidgetSwitcher;
class UInvalidationBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDungeonsTextCounterWidgetStartStop);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDungeonsTextCounterWidgetStep, const int32, lastStep, const int32, currentStep);

// D11.SC New C++ based TextCounter widget
UCLASS()
class DUNGEONS_API UDungeonsTextCounterWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized()override;

	virtual void NativePreConstruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;


	UFUNCTION(BlueprintCallable)
	void SetCounterInstant(int32 NewCount);

	UFUNCTION(BlueprintCallable)
	void SetCounterSmooth(int32 NewSmoothCount);

	UFUNCTION(BlueprintCallable)
	void SetCountingSmoothly(bool smoothly);

	UFUNCTION(BlueprintCallable)
	void FinishSmoothCount();

	UFUNCTION(BlueprintCallable)
	void SetSmoothCountDelay(float delay);

	UFUNCTION(BlueprintCallable)
	void Invalidate();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool HasPendingSmoothCounting() const;
	
	

	//progression delegates

	UPROPERTY(BlueprintAssignable)
	FDungeonsTextCounterWidgetStartStop OnStartedCountingSmoothly;

	UPROPERTY(BlueprintAssignable)
	FDungeonsTextCounterWidgetStartStop OnStoppedCountingSmoothly;

	UPROPERTY(BlueprintAssignable)
	FDungeonsTextCounterWidgetStep OnSmoothCountStep;

protected:

	void SetDisplayCount(float fVal);

	void SetSmoothCount(double fVal);

	//UFUNCTION(BlueprintCallable)
	//void StartReadyFlash();

	
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* CounterText;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UInvalidationBox* CounterTextInvBox;


	int32 lastSmoothRounded = -1;
	int32 mDisplayVal = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	int32 DefaultTextSize = 32;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	int32 StartingCount = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	int32 minKDisplayCapValue = 1; // Value (in thousands) before it transitions to 1.1k etc

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	int32 maxDisplayCount = (MAX_int32 -1); //maximum number that will be displayed by this counter

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	float SMOOTH_COUNT_SPEED = 1.0f;

	double Count = 0.0f;
	double CountSmooth = 0.0f;
	float SmoothCountDelay = 0.0f;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	FSlateColor DefaultTextColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	FSlateColor DefaultShadowColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	FVector2D ShadowTranslationFactor;

	bool counting_smoothly = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	bool K_CappedDisplay = true;
	
};
