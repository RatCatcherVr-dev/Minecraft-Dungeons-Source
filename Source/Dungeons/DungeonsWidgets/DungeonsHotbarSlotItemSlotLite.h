#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DungeonsHotbarSlotItemSlotLite.generated.h"

class UTextBlock;
class UCanvasPanel;
class UDungeonsImage;
class UDungeonsWidgetSwitcher;
class UInvalidationBox;
class UItemSlot;
class AItemInstance;

// D11.SC New C++ based Hotbar Slot widget
UCLASS()
class DUNGEONS_API UDungeonsHotbarSlotItemSlotLite : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime)override;


protected:

	void UpdateAnimTimers(float fDelta);

	UFUNCTION(BlueprintCallable)
	void StartReadyFlash();

	UFUNCTION(BlueprintCallable)
	void StartActiveFlash();

	UFUNCTION(BlueprintCallable)
	void UpdateCooldownMaterial(UItemSlot* pSlot, float Duration, float Remaining, float Reduction);
	
	UFUNCTION(BlueprintCallable)
	void SetBusy(bool Busy);

	bool RefreshBusy();

	UFUNCTION(BlueprintCallable)
	void SetDisabled(bool bDisabled);

	UFUNCTION(BlueprintCallable)
	bool RefreshEnabled();

	UFUNCTION(BlueprintCallable)
	bool CanActivateSlot() const;

	UFUNCTION(BlueprintCallable)
	bool IsDisabled() const{return mDisabled;};
	
	UPROPERTY(BlueprintReadWrite)
	UItemSlot* ItemSlot = nullptr;

	UPROPERTY(BlueprintReadWrite)
	AItemInstance* Item = nullptr;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* LitePanel;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDungeonsImage* ReadyFlashLite;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDungeonsImage* BusyFrameLite;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDungeonsImage* InnerFrameDecorLite;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDungeonsImage* CooldownMaterial;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDungeonsImage* ActivatedFrameLite;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDungeonsImage* ItemActivateFlashLite;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UInvalidationBox* InnerDecorINVBoxLite;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UDungeonsWidgetSwitcher* HotbarItemSlotIconRoot;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Config")
	bool AlwaysEnabled = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	int32 ItemSlotIndex = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Config")
	int32 MaxSlotIndexes = 1;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Config")
	UCurveFloat* ActiveFlashCurve = nullptr;
	
	int32 CurrentTickIndex = 0;

	float mReadyFlash = 0.0f;
	float mActiveFlash = 0.0f;

	bool mBusy = false;
	bool mDisabled = false;
	
};
