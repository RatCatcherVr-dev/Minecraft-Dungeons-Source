// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "DungeonsBorderSlot.h"
#include "Widgets/SNullWidget.h"
#include "Widgets/Layout/SBorder.h"
#include "DungeonsBorder.h"
#include "ObjectEditorUtils.h"

/////////////////////////////////////////////////////
// UDungeonsBorderSlot

UDungeonsBorderSlot::UDungeonsBorderSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Padding = FMargin(4, 2);

	HorizontalAlignment = HAlign_Fill;
	VerticalAlignment = VAlign_Fill;
}

void UDungeonsBorderSlot::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	Border.Reset();
}

void UDungeonsBorderSlot::BuildSlot(TSharedRef<SBorder> InBorder)
{
	Border = InBorder;

	Border.Pin()->SetPadding(Padding);
	Border.Pin()->SetHAlign(HorizontalAlignment);
	Border.Pin()->SetVAlign(VerticalAlignment);

	Border.Pin()->SetContent(Content ? Content->TakeWidget() : SNullWidget::NullWidget);
}

void UDungeonsBorderSlot::SetPadding(FMargin InPadding)
{
	CastChecked<UDungeonsBorder>(Parent)->SetPadding(InPadding);
}

void UDungeonsBorderSlot::SetHorizontalAlignment(EHorizontalAlignment InHorizontalAlignment)
{
	CastChecked<UDungeonsBorder>(Parent)->SetHorizontalAlignment(InHorizontalAlignment);
}

void UDungeonsBorderSlot::SetVerticalAlignment(EVerticalAlignment InVerticalAlignment)
{
	CastChecked<UDungeonsBorder>(Parent)->SetVerticalAlignment(InVerticalAlignment);
}

void UDungeonsBorderSlot::SynchronizeProperties()
{
	if (Border.IsValid())
	{
		SetPadding(Padding);
		SetHorizontalAlignment(HorizontalAlignment);
		SetVerticalAlignment(VerticalAlignment);
	}
}

#if WITH_EDITOR

void UDungeonsBorderSlot::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	static bool IsReentrant = false;

	if (!IsReentrant)
	{
		IsReentrant = true;

		if (PropertyChangedEvent.Property)
		{
			static const FName PaddingName("Padding");
			static const FName HorizontalAlignmentName("HorizontalAlignment");
			static const FName VerticalAlignmentName("VerticalAlignment");

			FName PropertyName = PropertyChangedEvent.Property->GetFName();

			if (UDungeonsBorder* ParentBorder = CastChecked<UDungeonsBorder>(Parent))
			{
				if (PropertyName == PaddingName)
				{
					FObjectEditorUtils::MigratePropertyValue(this, PaddingName, ParentBorder, PaddingName);
				}
				else if (PropertyName == HorizontalAlignmentName)
				{
					FObjectEditorUtils::MigratePropertyValue(this, HorizontalAlignmentName, ParentBorder, HorizontalAlignmentName);
				}
				else if (PropertyName == VerticalAlignmentName)
				{
					FObjectEditorUtils::MigratePropertyValue(this, VerticalAlignmentName, ParentBorder, VerticalAlignmentName);
				}
			}
		}

		IsReentrant = false;
	}
}

#endif
