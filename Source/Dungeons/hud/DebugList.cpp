#include "Dungeons.h"
#include "Engine.h"
#include "hud/DebugList.h"


DebugList::DebugList(AHUD* hud, const FVector2D& offset)
	: hud { hud }
	, lineCount { 0 }
{
	if (offset.X < 0) {
		FVector2D viewportSize;
		GEngine->GameViewport->GetViewportSize(viewportSize);

		this->offset.Set(offset.X + viewportSize.X, offset.Y);
	} else {
		this->offset.Set(offset.X, offset.Y);
	}
}

void DebugList::Draw(const FString& string, const FColor& color/* = FColor::White*/, int indent/* = 0*/) {
	hud->DrawText(
		string,
		color,
		offset.X + indent,
		offset.Y + lineHeight * lineCount
	);
	lineCount++;
}

void DebugList::NewLine() {
	lineCount++;
}

const int DebugList::lineHeight = 12;