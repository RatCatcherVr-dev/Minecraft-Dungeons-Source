#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DungeonsSoulBarLite.generated.h"

class UTextBlock;
class UCanvasPanel;
class UDungeonsImage;
class UDungeonsWidgetSwitcher;
class UInvalidationBox;
class UItemSlot;
class AItemInstance;
class USoulComponent;
class USpacer;

// D11.SC New C++ based Soul Bar widget
UCLASS()
class DUNGEONS_API UDungeonsSoulBarLite : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;
	
protected:

	void UpdateAnimTimers(float fDelta);

	void UpdateUsageIndicator(float fDelta);

	UFUNCTION(BlueprintCallable)
	void StartFaceGlow();

	UFUNCTION(BlueprintCallable)
	void StartBarGlow();

	UFUNCTION(BlueprintCallable)
	void StartNumberAnim();

	UFUNCTION(BlueprintCallable)
	void StartFullShine();

	UFUNCTION(BlueprintCallable)
	void StartMissingSoulsGlow(float fPercentNeeded);

	
	UFUNCTION(BlueprintCallable)
	void StopUsageIndicator(float fPercent);

	UFUNCTION(BlueprintCallable)
	void SoulPercentageChanged(USoulComponent* SoulComponent);
	

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDungeonsImage* iconPop;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDungeonsImage* barGlow;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDungeonsImage* MissingSoulsIndicator;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDungeonsImage* UsageIndicator;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDungeonsImage* FullShine;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	USpacer* EmptyFill;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* MeterFill;


	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Config")
	UCurveFloat* NumberCurve = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
	UTextBlock* TargetText = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
	float FACE_GLOW_FACTOR = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	float NumberMovement = 0.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	float BAR_GLOW_FACTOR = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
	float CurrentPercentage = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
	float IndicatorPercentage = 0.0f;


	float mFullShineCountDown = 0.0f;
	float mUsageGlowCountDown = 0.0f;
	float mNumberCountDown = 0.0f;
	float mFaceGlowCountDown = 0.0f;
	float mBarGlowCountDown = 0.0f;
	float mMissingGlowCountDown = 0.0f;
	float mMissingGlowTime = 1.5f;

};
