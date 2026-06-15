#include "DungeonsHotbarSlotItemSlotLite.h"

#include "Components/TextBlock.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/InvalidationBox.h"
#include "game/item/ItemSlot.h"
#include "game/item/instance/AItemInstance.h"
#include "game/item/ItemFunctionLibrary.h"

#include "DungeonsImage.h"
#include "DungeonsWidgetswitcher.h"

void UDungeonsHotbarSlotItemSlotLite::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UDungeonsHotbarSlotItemSlotLite::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if(ItemSlot)
	{
		
		if (!AlwaysEnabled)
		{
			//limit these to 1 update  per max Indices tick
			if (ItemSlotIndex == CurrentTickIndex)
			{
				if (!RefreshBusy())
				{
					RefreshEnabled();
				}
			}

			++CurrentTickIndex %= MaxSlotIndexes;
		}

		UpdateAnimTimers(InDeltaTime);
	}

	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UDungeonsHotbarSlotItemSlotLite::UpdateAnimTimers(float fDelta)
{
	//Ready Flash Anim
	if (mReadyFlash > 0.0f)
	{
		const float fFlashVal = (mReadyFlash > 0.9f) ? 0.0f : mReadyFlash;

		ReadyFlashLite->SetColorAndOpacity(FLinearColor(1,1,1, fFlashVal));
		mReadyFlash -= fDelta;
		if (mReadyFlash <= 0.0f)
		{
			mReadyFlash = 0.0f;
			ReadyFlashLite->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	//Activate flash anim
	if (mActiveFlash > 0.0f)
	{
		const float fCurveVal = (ActiveFlashCurve) ? ActiveFlashCurve->GetFloatValue(mActiveFlash) : mActiveFlash; //just in case...

		ItemActivateFlashLite->SetRenderOpacity(fCurveVal);
		ActivatedFrameLite->SetRenderOpacity(fCurveVal);
		mActiveFlash -= fDelta;
		if (mActiveFlash <= 0.0f)
		{
			mActiveFlash = 0.0f;
			ItemActivateFlashLite->SetVisibility(ESlateVisibility::Collapsed);
			ActivatedFrameLite->SetVisibility(ESlateVisibility::Collapsed);
		}
	}


}

void UDungeonsHotbarSlotItemSlotLite::StartReadyFlash()
{
	ReadyFlashLite->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	mReadyFlash = 1.0f;
}

void UDungeonsHotbarSlotItemSlotLite::StartActiveFlash()
{
	ItemActivateFlashLite->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ActivatedFrameLite->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	mActiveFlash = 1.0f;
}

void UDungeonsHotbarSlotItemSlotLite::UpdateCooldownMaterial(UItemSlot* pSlot, float Duration, float Remaining, float Reduction)
{
	if (auto* pDynamicMatl = CooldownMaterial->GetDynamicMaterial())
	{
		const float fTimeSeconds = GetWorld()->GetTimeSeconds() - Duration - Remaining ;
		pDynamicMatl->SetScalarParameterValue("CooldownStart", fTimeSeconds);
		pDynamicMatl->SetScalarParameterValue("CooldownDuration", Duration);
		pDynamicMatl->SetScalarParameterValue("CooldownReduction", Reduction);
	}
}

void UDungeonsHotbarSlotItemSlotLite::SetBusy(bool Busy)
{
	if (Busy != mBusy)
	{
		BusyFrameLite->SetVisibility((Busy) ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		
		InnerFrameDecorLite->SetVisibility((Busy) ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
		InnerDecorINVBoxLite->SetVisibility((Busy) ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);

		mBusy = Busy;
	}
}

bool UDungeonsHotbarSlotItemSlotLite::RefreshBusy()
{
	if (ItemSlot)
	{
		const bool IsBusy = ItemSlot->IsBusy();
		SetBusy(IsBusy);
		return IsBusy;
	}
	
	SetBusy(false);
	return false;
}

void UDungeonsHotbarSlotItemSlotLite::SetDisabled(bool bDisabled)
{
	if (bDisabled != mDisabled)
	{
			HotbarItemSlotIconRoot->SetRenderOpacity((bDisabled) ? 0.67f : 1.0f);
			HotbarItemSlotIconRoot->SetIsEnabled(!bDisabled);			
			InnerDecorINVBoxLite->SetVisibility((bDisabled) ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);		
			mDisabled = bDisabled;
	}
}

bool UDungeonsHotbarSlotItemSlotLite::RefreshEnabled()
{
	if (Item)
	{
		const bool Deactivated = !CanActivateSlot() || UItemFunctionLibrary::GetIsPassiveForItemType(Item->GetItemId());		
		SetDisabled(Deactivated);
		return Deactivated;
	}

	SetDisabled(false);
	return false;
}

bool UDungeonsHotbarSlotItemSlotLite::CanActivateSlot() const
{
	if (ItemSlot)
	{
		return ItemSlot->CanActivateSlot();
	}

	return false;
}

