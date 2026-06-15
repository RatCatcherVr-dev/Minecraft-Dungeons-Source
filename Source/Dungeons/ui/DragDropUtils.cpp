#include "Dungeons.h"
#include <UMGDragDropOp.h>
#include <SlateApplication.h>
#include "DragDropUtils.h"


void UDragDropUtils::SetDragDropCursorOverride(EMouseCursor::Type cursor)
{
	TSharedPtr<FDragDropOperation> SlateDragOp = FSlateApplication::Get().GetDragDroppingContent();
	if (SlateDragOp.IsValid() && SlateDragOp->IsOfType<FUMGDragDropOp>())
	{
		TSharedPtr<FUMGDragDropOp> UMGDragDropOp = StaticCastSharedPtr<FUMGDragDropOp>(SlateDragOp);
		UMGDragDropOp->SetCursorOverride(cursor);
	}
}

