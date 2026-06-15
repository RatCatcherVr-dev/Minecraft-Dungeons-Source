#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DungeonsScreenPositionWidgetInterface.h"
#include "DungeonsDamagePopWidget.generated.h"

class UTextBlock;
class UDungeonsDamagePopWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDamagePopCompletedCallback, const UDungeonsDamagePopWidget*, PopWidget);

// D11.SC New C++ based Damage pop widget
UCLASS()
class DUNGEONS_API UDungeonsDamagePopWidget : public UUserWidget, public IDungeonsScreenPositionWidgetInterface
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized()override;
	//virtual void NativePreConstruct();
	//virtual void NativeConstruct();
	//virtual void NativeDestruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;

	UFUNCTION(BlueprintCallable)
	void UpdateOnScreenLocation();

	void SetOnScreenLocation_Implementation(const FVector2D& Pos) override { mScreenLocation = Pos; };
	void ModifyOnScreenLocation_Implementation(const FVector2D& Diff )override { mScreenLocation += Diff;};

	UFUNCTION(BlueprintCallable)
	void SetupDamageVal(float fTextValue);

	UFUNCTION(BlueprintCallable)
	void SetupTextVal(const FText& fText);

	UFUNCTION(BlueprintCallable)
	void LaunchPop();

	UPROPERTY(BlueprintAssignable)
	FDamagePopCompletedCallback OnDamagePopCompleted;

protected:

	UFUNCTION(BlueprintCallable)
	void SetupPopVals(float fDuration, float fScale, float fGravity, float fFriction, const FLinearColor& TextColour, const FLinearColor& ShadowColour, const FVector2D& InitialScreenPosition, const FVector2D& InitialScreenVelocity);


	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Text;

	FVector2D mScreenLocation;
	FVector2D mScreenVelocity;
	FVector2D mScreenPerspective = FVector2D(1.0f,0.7f);

	int32 mCachedVal = -1;

	float mLifeTime = 1.0f;
	float mDuration = 1.0f;
	float mScaleTime = 0.0f;
	float mGravity = 1.0f;
	float mGravityFactor = 1.75f;
	float mFriction = 1.0f;
	float mFrictionFactor = 2.2f;
	float mVelocityFactor = 500.0f;
	float mYLaunchFactor = -0.45f;

	bool mIsText = false;
	
};
