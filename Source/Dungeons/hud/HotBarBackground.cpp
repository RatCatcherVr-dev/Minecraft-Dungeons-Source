#include "Dungeons.h"
#include "Engine.h"
#include "HotBarBackground.h"
#include "CanvasUtility.h"
#include "DungeonsGameInstance.h"

const float UHotBarBackground::MASTER_SCALE = 0.5f;

const float UHotBarBackground::TEXTURE_PADDING = 1.0f;

float UHotBarBackground::SCALE_MULTIPLIER = 1.f;

//Whole hotbar background graphics location
const FVector2D UHotBarBackground::HOT_BAR_POSITION = { 0.5f, 1.0f };

//intended to be horizontally centered, vertically bottom aligned.
const FVector2D UHotBarBackground::HOT_BAR_ALIGNMENT = { -0.5f, -1.0f };
const FVector2D UHotBarBackground::COOP_HOT_BAR_ALIGNMENTS[4] = { {0.0f, -1.0f}, {-1.0f, -1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f} };

//Size of box behind hotbar.
FVector2D UHotBarBackground::HOT_BAR_SIZE = { 2040.f, 25.f };
const FVector2D UHotBarBackground::COOP_HOT_BAR_SIZE = { 1592.0f, 75.0f };

//Color of box behind hotbar
const FLinearColor UHotBarBackground::HOT_BAR_COLOR = FLinearColor{ 0.f,0.f,0.f,1.0f };

//Padding for box on local coop.
const float UHotBarBackground::HORIZONTAL_PADDING = 24.0f;
const float UHotBarBackground::INNER_HOTBAR_PADDING = 20.0f;

UHotBarBackground::UHotBarBackground(const FObjectInitializer& ObjectInitializer)
{
}

void UHotBarBackground::SetVisible(bool is_visible) {
	visible = is_visible;
}

void UHotBarBackground::SetOffset(FVector2D newOffset) {
	coopOffsets[0] = newOffset;
}

void UHotBarBackground::SetHotbarSize(FVector2D newSize) {
	HOT_BAR_SIZE = newSize;
}

void UHotBarBackground::SetCoopOffset(int index, FVector2D newOffset) {

	if(index < 0 || index > 4)
	{
		return;
	}

	coopOffsets[index] = newOffset;
}

void UHotBarBackground::SetScale(float scale)
{
	SCALE_MULTIPLIER = scale;
}

void UHotBarBackground::Draw(UCanvas* canvas, APlayerController* playerOwner, float dpi_scale, float dt, AHUD* hud)
{
	if (GEngine == nullptr || !visible) { return; }

	int hotbarsToDraw = 1;

	//D11.KS - Don't bother rendering the background for now.
	if(UWorld* World = GEngine->GetWorldFromContextObject(playerOwner, EGetWorldErrorMode::LogAndReturnNull))
	{
		hotbarsToDraw = World->GetGameInstance()->GetNumLocalPlayers();
	}

	FVector2D viewportSize;
	GEngine->GameViewport->GetViewportSize(viewportSize);

	if (hotbarsToDraw > 1)
	{
		const float scale = MASTER_SCALE * dpi_scale;

		for (int i = 0; i < hotbarsToDraw; i++)
		{
			const FVector2D hotbarSizeScaled = COOP_HOT_BAR_SIZE * scale * FVector2D(SCALE_MULTIPLIER, 1);
			const FVector2D offsetPosition = hotbarSizeScaled * COOP_HOT_BAR_ALIGNMENTS[i];
			const FVector2D viewportAlignment = viewportSize * COOP_HOT_BAR_ALIGNMENTS[i] * -1;
			const float horizontalAlignDirection = (COOP_HOT_BAR_ALIGNMENTS[i].X * 2) + 1;
			const float coopOffsetAligned = (HORIZONTAL_PADDING + INNER_HOTBAR_PADDING * SCALE_MULTIPLIER) * scale * horizontalAlignDirection;

			const FVector2D offsetedBackgroundPosition = viewportAlignment + offsetPosition + coopOffsets[i];
			hud->DrawRect(HOT_BAR_COLOR, offsetedBackgroundPosition.X + coopOffsetAligned, offsetedBackgroundPosition.Y, hotbarSizeScaled.X, COOP_HOT_BAR_SIZE.Y * scale);
		}
	}
	else
	{
		const float scale = MASTER_SCALE * SCALE_MULTIPLIER * dpi_scale;
		const FVector2D offsetedBackgroundPosition{
			HOT_BAR_POSITION.X * viewportSize.X + HOT_BAR_SIZE.X * HOT_BAR_ALIGNMENT.X * scale + coopOffsets[0].X * scale, 
			HOT_BAR_POSITION.Y * viewportSize.Y + HOT_BAR_SIZE.Y * HOT_BAR_ALIGNMENT.Y * MASTER_SCALE * dpi_scale + coopOffsets[0].Y * MASTER_SCALE * dpi_scale };
		hud->DrawRect(HOT_BAR_COLOR, offsetedBackgroundPosition.X, offsetedBackgroundPosition.Y, HOT_BAR_SIZE.X * scale, HOT_BAR_SIZE.Y * MASTER_SCALE * dpi_scale);
	}
}
