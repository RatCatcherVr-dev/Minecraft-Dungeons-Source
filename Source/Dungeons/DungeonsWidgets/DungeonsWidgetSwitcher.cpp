

#include "DungeonsWidgetSwitcher.h"
#include "Components/WidgetSwitcher.h"
#include "SlateFwd.h"
#include "Components/WidgetSwitcherSlot.h"

#define LOCTEXT_NAMESPACE "UMG"

/////////////////////////////////////////////////////
// UWidgetSwitcher

UDungeonsWidgetSwitcher::UDungeonsWidgetSwitcher(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;

	SWidgetSwitcher::FArguments Defaults;
	Visibility = UWidget::ConvertRuntimeToSerializedVisibility(Defaults._Visibility.Get());
}

void UDungeonsWidgetSwitcher::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	MyWidgetSwitcher.Reset();
}

int32 UDungeonsWidgetSwitcher::GetNumWidgets() const
{
	if (MyWidgetSwitcher.IsValid())
	{
		return MyWidgetSwitcher->GetNumWidgets();
	}

	return Slots.Num();
}

int32 UDungeonsWidgetSwitcher::GetActiveWidgetIndex() const
{
	if (MyWidgetSwitcher.IsValid())
	{
		return MyWidgetSwitcher->GetActiveWidgetIndex();
	}

	return ActiveWidgetIndex;
}

void UDungeonsWidgetSwitcher::SetActiveWidgetIndex(int32 Index)
{
	ActiveWidgetIndex = Index;
	if (MyWidgetSwitcher.IsValid()) {
		// Ensure the index is clamped to a valid range.
		int32 SafeIndex = FMath::Clamp(ActiveWidgetIndex, 0, FMath::Max(0, Slots.Num() - 1));
		MyWidgetSwitcher->SetActiveWidgetIndex(SafeIndex);

		auto currentWidgetIndex = MyWidgetSwitcher->GetActiveWidgetIndex();
		for (int i = 0; i < GetNumWidgets(); ++i) {
			if (i != currentWidgetIndex) {
				UWidget* widget = GetWidgetAtIndex(i);
				widget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		auto targetVis = WidgetsVisibility.IsValidIndex(currentWidgetIndex) ? WidgetsVisibility[currentWidgetIndex] : ESlateVisibility::SelfHitTestInvisible;
		if (auto activeWidget = GetWidgetAtIndex(currentWidgetIndex)) {
			activeWidget->SetVisibility(targetVis);
		}
	}
}


void UDungeonsWidgetSwitcher::SetActiveWidget(UWidget* Widget){
	auto index = GetChildIndex(Widget);
	if(index != INDEX_NONE){
		SetActiveWidgetIndex(index);
	}
}

UWidget* UDungeonsWidgetSwitcher::GetWidgetAtIndex(int32 Index) const
{
	if (Slots.IsValidIndex(Index))
	{
		return Slots[Index]->Content;
	}

	return nullptr;
}

UWidget* UDungeonsWidgetSwitcher::GetActiveWidget()const
{
	return GetWidgetAtIndex(GetActiveWidgetIndex());
}

UClass* UDungeonsWidgetSwitcher::GetSlotClass() const
{
	return UWidgetSwitcherSlot::StaticClass();
}

void UDungeonsWidgetSwitcher::OnSlotAdded(UPanelSlot* InSlot)
{
	// Add the child to the live canvas if it already exists
	if (MyWidgetSwitcher.IsValid())
	{
		CastChecked<UWidgetSwitcherSlot>(InSlot)->BuildSlot(MyWidgetSwitcher.ToSharedRef());
	}
}

void UDungeonsWidgetSwitcher::OnSlotRemoved(UPanelSlot* InSlot)
{
	// Remove the widget from the live slot if it exists.
	if (MyWidgetSwitcher.IsValid())
	{
		TSharedPtr<SWidget> Widget = InSlot->Content->GetCachedWidget();
		if (Widget.IsValid())
		{
			MyWidgetSwitcher->RemoveSlot(Widget.ToSharedRef());
		}
	}
}

TSharedRef<SWidget> UDungeonsWidgetSwitcher::RebuildWidget()
{
	MyWidgetSwitcher = SNew(SWidgetSwitcher);

	for (UPanelSlot* PanelSlot : Slots)
	{
		if (UWidgetSwitcherSlot* TypedSlot = Cast<UWidgetSwitcherSlot>(PanelSlot))
		{
			TypedSlot->Parent = this;
			TypedSlot->BuildSlot(MyWidgetSwitcher.ToSharedRef());
		}
	}

	return MyWidgetSwitcher.ToSharedRef();
}

void UDungeonsWidgetSwitcher::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	for (int i = 0; i < MyWidgetSwitcher->GetNumWidgets(); ++i)
	{
		WidgetsVisibility.Add(GetWidgetAtIndex(i)->GetVisibility());
	}

	SetActiveWidgetIndex(ActiveWidgetIndex);

}

#if WITH_EDITOR

const FText UDungeonsWidgetSwitcher::GetPaletteCategory()
{
	return LOCTEXT("Panel", "Panel");
}

void UDungeonsWidgetSwitcher::OnDescendantSelectedByDesigner(UWidget* DescendantWidget)
{
	// Temporarily sets the active child to the selected child to make
	// dragging and dropping easier in the editor.
	UWidget* SelectedChild = UWidget::FindChildContainingDescendant(this, DescendantWidget);
	if (SelectedChild)
	{
		int32 OverrideIndex = GetChildIndex(SelectedChild);
		if (OverrideIndex != -1 && MyWidgetSwitcher.IsValid())
		{
			MyWidgetSwitcher->SetActiveWidgetIndex(OverrideIndex);
#if WITH_EDITOR
			SetActiveWidgetIndex(OverrideIndex);
#endif
		}
	}
}

void UDungeonsWidgetSwitcher::OnDescendantDeselectedByDesigner(UWidget* DescendantWidget)
{
	SetActiveWidgetIndex(ActiveWidgetIndex);
}

void UDungeonsWidgetSwitcher::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	ActiveWidgetIndex = FMath::Clamp(ActiveWidgetIndex, 0, FMath::Max(0, Slots.Num() - 1));

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
