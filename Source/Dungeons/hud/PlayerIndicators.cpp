#include "Dungeons.h"
#include "Engine.h"
#include "Engine/Font.h"
#include "CanvasUtility.h"
#include "CanvasItem.h"
#include "hud/PlayerIndicators.h"
#include "Runtime/SlateCore/Public/Fonts/SlateFontInfo.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/avatar/PlayerAvatarComponent.h"
#include "game/component/HealthBarComponent.h"
#include "game/actor/character/player/BasePlayerState.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/item/instance/AItemInstance.h"
#include "ObjectiveIndicators.h"
#include "game/item/ItemFunctionLibrary.h"

//Scale of the entire portrait indicator
const float UPlayerIndicators::MASTER_SCALE = 0.5f;
const float UPlayerIndicators::MINIATURE_SKULL_SCALE = 0.5f;

const float UPlayerIndicators::PLAYER_NAME_ARROW_COUNT_SPACING = 5.0f;

const FVector2D UPlayerIndicators::EMOTE_PADDING = { 90.0f, 100.0f };


const FColor UPlayerIndicators::HEALTH_BAR_HEAL_COLOR = FColor(55, 193, 136);
const FColor UPlayerIndicators::HEALTH_BAR_DAMAGE_COLOR = FColor(255, 255, 255);
const FColor UPlayerIndicators::HEALTH_BAR_BG_COLOR = FColor(20, 18, 23);
const FColor UPlayerIndicators::HEALTH_BAR_REVIVE_COLOR = FColor(255, 255, 255);

const uint8 UPlayerIndicators::HEALTH_BAR_ALPHA = 255;

const FColor UPlayerIndicators::ARROW_COUNT_COLOR = FColor(255, 255, 255);
const FColor UPlayerIndicators::ARROW_COUNT_SHADOW_COLOR = FColor(43, 41, 50);
const FVector2D UPlayerIndicators::ARROW_COUNT_SHADOW_OFFSET = { 2.0f, 2.0f };
const FVector2D UPlayerIndicators::ARROW_SPRITE_OFFSET = { 0.0f, 6.0f };
const FVector2D UPlayerIndicators::ARROW_COUNT_OFFSET = { 0.0f, -1.0f };

const FColor UPlayerIndicators::PLAYER_NAME_COLOR = FColor(255, 255, 255);
const FColor UPlayerIndicators::PLAYER_NAME_SHADOW_COLOR = FColor(43, 41, 50);
const FVector2D UPlayerIndicators::PLAYER_NAME_OFFSET = { 0.0f, -1.0f };
const FVector2D UPlayerIndicators::PLAYER_NAME_SHADOW_OFFSET = { 2.0f, 2.0f };

const FVector2D UPlayerIndicators::PLAYER_NAME_ARROW_ROW_ALIGNMENT = { -0.5f, -1.0f };
const FVector2D UPlayerIndicators::PLAYER_NAME_ARROW_ROW_OFFSET = { 0.0f, -11.0f };

const FVector2D UPlayerIndicators::PLAYER_INVENTORY_FULL_ROW_OFFSET = { 0.0f, -50.0f };

//Box region where portraits will be hidden, unit is factor of entire viewport.
const FBox2D UPlayerIndicators::VIEWPORT_HIDE_PORTRAIT_REGION = {
	FVector2D(0.22f, 0.5f), //top left
	FVector2D(0.78f, 1.0f)  //bottom right
};

UPlayerIndicators::UPlayerIndicators(const FObjectInitializer& ObjectInitializer) {
	const ConstructorHelpers::FObjectFinder<UTexture2D> PortraitBg(TEXT("/Game/UI/Materials/Portrait/friend_portrait_bg"));
	const ConstructorHelpers::FObjectFinder<UTexture2D> PortraitFrame(TEXT("/Game/UI/Materials/Portrait/friend_portrait_frame"));
	const ConstructorHelpers::FObjectFinder<UTexture2D> PortraitUnknown(TEXT("/Game/UI/Materials/Portrait/friend_portrait_unknown"));
	const ConstructorHelpers::FObjectFinder<UTexture2D> HealthBarFrame(TEXT("/Game/UI/Materials/Portrait/friend_health_bar_frame"));
	const ConstructorHelpers::FObjectFinder<UTexture2D> HealthBarBg(TEXT("/Game/UI/Materials/Portrait/friend_health_bar_bg"));
	const ConstructorHelpers::FObjectFinder<UTexture2D> HealthBar(TEXT("/Game/UI/Materials/Portrait/friend_health_bar"));

	const ConstructorHelpers::FObjectFinder<UTexture2D> HealthBarBigBg(TEXT("/Game/UI/Materials/Portrait/friend_onscreen_health_bg"));
	const ConstructorHelpers::FObjectFinder<UTexture2D> HealthBarBigFrame(TEXT("/Game/UI/Materials/Portrait/friend_onscreen_health_frame"));
	const ConstructorHelpers::FObjectFinder<UTexture2D> HealthBarBig(TEXT("/Game/UI/Materials/Portrait/friend_onscreen_health_bar"));

	const ConstructorHelpers::FObjectFinder<UTexture2D> Skull(TEXT("/Game/UI/Materials/Portrait/skull"));
	const ConstructorHelpers::FObjectFinder<UTexture2D> SkullFrame(TEXT("/Game/UI/Materials/Portrait/skull_frame"));
	const ConstructorHelpers::FObjectFinder<UTexture2D> SkullFrameBorder(TEXT("/Game/UI/Materials/Portrait/skull_frame_border"));

	if (
		!PortraitFrame.Succeeded()
		|| !PortraitUnknown.Succeeded()
		|| !HealthBarFrame.Succeeded()
		|| !HealthBar.Succeeded()
		|| !HealthBarBigFrame.Succeeded()
		|| !HealthBarBig.Succeeded()
		|| !HealthBarBigBg.Succeeded()
		|| !Skull.Succeeded()
		|| !SkullFrame.Succeeded()
		|| !PortraitBg.Succeeded()
		|| !HealthBarBg.Succeeded()
		|| !SkullFrameBorder.Succeeded()
		) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load player portrait assets"));
		return;
	}

	const ConstructorHelpers::FObjectFinder<UFont> ArrowCountFontFinder(TEXT("/Game/Fonts/Slate/SlateArrowCount"));
	if (!ArrowCountFontFinder.Succeeded()) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load player indicator minecraft font"));
		return;
	}

	ArrowCountFont = ArrowCountFontFinder.Object;

	const ConstructorHelpers::FObjectFinder<UFont> PlayerNameFontFinder(TEXT("/Game/Fonts/Slate/PlayerName"));
	if (!PlayerNameFontFinder.Succeeded()) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load player indicator minecraft font"));
		return;
	}

	PlayerNameFont = PlayerNameFontFinder.Object;

	const ConstructorHelpers::FObjectFinder<UFont> LocalPlayerDisplayTextFontFinder(TEXT("/Game/Fonts/Slate/LocalPlayerDisplayTextFont"));
	if (!LocalPlayerDisplayTextFontFinder.Succeeded()) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load player indicator minecraft font"));
		return;
	}

	LocalPlayerDisplayTextFont = LocalPlayerDisplayTextFontFinder.Object;
	

	//Assign local textures
	HealthBarTexture = HealthBar.Object;
	HealthBarBigTexture = HealthBarBig.Object;
	SkullFrameTexture = SkullFrame.Object;

	//Create all icons from full texture.
	FVector2D portraitFrameSize;
	FVector2D healthBarFrameSize;
	FVector2D healthBarBigBgSize;

	PortraitBgIcon = CanvasUtility::MakeIconFullTexture(PortraitBg.Object, portraitFrameSize, 1.0f);
	PortraitFrameIcon = CanvasUtility::MakeIconFullTexture(PortraitFrame.Object, portraitFrameSize, 1.0f);
	HealthBarBgIcon = CanvasUtility::MakeIconFullTexture(HealthBarBg.Object, healthBarFrameSize, 1.0f);
	HealthBarFrameIcon = CanvasUtility::MakeIconFullTexture(HealthBarFrame.Object, healthBarFrameSize, 1.0f);
	HealthBarBigFrameIcon = CanvasUtility::MakeIconFullTexture(HealthBarBigFrame.Object, HealthBarBigFrameSize, 1.0f);
	HealthBarBigBgIcon = CanvasUtility::MakeIconFullTexture(HealthBarBigBg.Object, healthBarBigBgSize, 1.0f);
	SkullIcon = CanvasUtility::MakeIconFullTexture(Skull.Object, SkullSize, 0.0f);
	SkullFrameIcon = CanvasUtility::MakeIconFullTexture(SkullFrame.Object, SkullFrameSize, 0.0f);
	SkullFrameBorderIcon = CanvasUtility::MakeIconFullTexture(SkullFrameBorder.Object, SkullFrameBorderSize, 0.0f);

	//Bounds values for the whole player indicator
	Bounds = FVector2D(portraitFrameSize.X, portraitFrameSize.Y);

	//Padding around the screen
	ViewportPadding = FVector2D(0.0f, 0.0f);

	//Assume render offset for health bar is the difference between content and its border.
	//Hard-code these values for a different bar-thickness in the art
	const auto CalculatedPortraitBarBorderThickness = (portraitFrameSize.X - PortraitUnknown.Object->GetSizeX()) * 0.5f;
	const auto CalculatedHealthBarBorderThickness = (healthBarFrameSize.X - HealthBar.Object->GetSizeX()) * 0.5f;
	const auto CalculatedSkullBorderThickness = (SkullFrameSize.X - SkullSize.X) * 0.5f;

	//Offset to the top-left corner of each icon
	PortraitSize = FVector2D(PortraitUnknown.Object->GetSizeX(), PortraitUnknown.Object->GetSizeY());
	HealthBarSize = FVector2D(HealthBarTexture->GetSizeX(), HealthBarTexture->GetSizeY());
	HealthBarBigSize = FVector2D(HealthBarBigTexture->GetSizeX(), HealthBarBigTexture->GetSizeY());
	MiniatureSkullSize = SkullSize * MINIATURE_SKULL_SCALE;
	MiniatureSkullFrameSize = SkullFrameSize * MINIATURE_SKULL_SCALE;


	PortraitFrameOffset = -portraitFrameSize * 0.5f; // FVector2D(0.0f, 0.0f);
	PortraitOffset = PortraitFrameOffset + FVector2D(CalculatedPortraitBarBorderThickness, CalculatedPortraitBarBorderThickness);

	HealthBarFrameOffset = PortraitFrameOffset + FVector2D((portraitFrameSize.X - healthBarFrameSize.X)*0.5, portraitFrameSize.Y - healthBarFrameSize.Y);
	HealthBarOffset = FVector2D(HealthBarFrameOffset.X + CalculatedHealthBarBorderThickness, HealthBarFrameOffset.Y + CalculatedHealthBarBorderThickness);

	HealthBarBigFrameOffset = -HealthBarBigFrameSize * 0.5;
	HealthBarBigOffset = -HealthBarBigSize * 0.5;

	SkullFrameOffset = -SkullFrameSize * 0.5f;
	SkullOffset = -SkullSize * 0.5f;

	MiniatureSkullFrameOffset = -MiniatureSkullFrameSize * 0.5f;
	MiniatureSkullFrameOffset.Y = MiniatureSkullFrameOffset.Y + Bounds.Y*0.5f - MiniatureSkullFrameSize.Y*0.5f; //Bottom align small skull
	MiniatureSkullOffset = MiniatureSkullFrameOffset + CalculatedSkullBorderThickness * MINIATURE_SKULL_SCALE;

	ArrowCountText = FText::AsNumber(0);
}


void UPlayerIndicators::SetSuddenDeathActor(ASuddenDeath* suddenDeath) {
	SuddenDeathActor = suddenDeath;
}

void UPlayerIndicators::Draw(UCanvas* canvas, APlayerController* playerOwner, const float dpi_scale, AHUD* hud) {
	if (!playerOwner->GetPawn()) {
		return;
	}

	FVector2D viewportSize;
	GEngine->GameViewport->GetViewportSize(viewportSize);

	const UGameInstance* gi = playerOwner->GetGameInstance();

	const bool isLocalCoop = gi->GetNumLocalPlayers() > 1;
	const auto owner = playerOwner->GetPawn();
	const auto player = Cast<APlayerCharacter>(playerOwner->GetPawn());

	const auto world = playerOwner->GetWorld();
	
	for (const APlayerCharacter* actor : TActorRange<APlayerCharacter>(world)) {
		if (actor == owner && !isLocalCoop) {
			continue;
		}

		if (actor->GetWorldState() != ECharacterWorldState::InWorld) {
			continue;
		}

		if (player->GetCurrentDungeonInstanceId() != actor->GetCurrentDungeonInstanceId()) {
			continue;
		}

		if (!actor || !actor->GetDungeonsBasePlayerState()) {
			//Vrak: Removed warning since this is valid waiting state for seamless travel.
			//UE_LOG(LogTemp, Warning, TEXT("player or player->PlayerState was not"));
			continue;
		}

		auto avatarComponent = actor->FindComponentByClass<UPlayerAvatarComponent>();
		if (avatarComponent == nullptr) {
			continue;
		}

		const auto healthBarComponent = actor->GetHealthBarComponent();
		if (healthBarComponent != nullptr) {
			const auto barLocation = actorquery::getActorHealthBarPosition(playerOwner, actor);
			auto projectedPosition = CanvasUtility::ProjectLocationToCanvas(barLocation, 0.0f, canvas);

			const auto isInViewport = CanvasUtility::IsInToViewport(projectedPosition, viewportSize);

			const auto scale = MASTER_SCALE * dpi_scale;
			auto projectedClamped = GetProjectedClamped(viewportSize, projectedPosition, scale);
			const auto isInHidePortraitRegion = !isInViewport && VIEWPORT_HIDE_PORTRAIT_REGION.IsInside(projectedClamped / viewportSize);;

			const auto aliveState = actor->GetAliveState();
			const auto isDead = aliveState == EAliveState::Dead;

			if (isDead) {
				//No health bar or skull when dead.
				continue;
			}

			const auto isDown = aliveState == EAliveState::Down;
			
			auto activeReviveComponent = GetActiveReviveComponent(owner);
 
			if(isLocalCoop && isDown) {
				const TArray<ULocalPlayer*> localPlayers = gi->GetLocalPlayers();

				for(const ULocalPlayer* localPlayer : localPlayers)	{
					if(playerOwner->GetLocalPlayer() == localPlayer) {
						continue;
					}

					if(const auto localPawn = localPlayer->GetPlayerController(world)->GetPawn()) {
						if(UReviveComponent* localReviveComponent = GetActiveReviveComponent(localPawn)) {
							if(localReviveComponent->IsTargetingPlayer(actor)) {
								activeReviveComponent = localReviveComponent;
								break;
							}
						}

					}
				}
			}


			const auto isBeingRevived = isDown && activeReviveComponent && activeReviveComponent->IsTargetingPlayer(actor);
			const FAvatarData data = avatarComponent->GetPlayerAvatarData();
			auto playerColor = data.PlayerColor;
			auto portraitBackgroundColor = data.FrameBackgroundColor;

			auto textureDrawPosition = GetTextureDrawPosition(projectedClamped, isInViewport, scale);

			const auto currentHealthBarSize = isInViewport ? HealthBarBigSize : HealthBarSize;
			const auto currentHealthBarOffset = isInViewport ? HealthBarBigOffset : HealthBarOffset;
			const auto barTexture = isInViewport ? HealthBarBigTexture : HealthBarTexture;
			const float downFrameOpacity = isDown && SuddenDeathActor.IsValid() ? (SuddenDeathActor->IsSuddenDeathCountingDown() ? 1.0f : 0.0f) : 0.0f;

			if (isInViewport || isLocalCoop) {

				if (isDown) {
					DrawSkull(canvas, world, playerColor, textureDrawPosition, scale, isBeingRevived ? 0.0f : downFrameOpacity);
					textureDrawPosition.Y += (SkullFrameSize.Y + HealthBarBigFrameSize.Y + 4.0f) * 0.5f * scale;

					if (isBeingRevived) {
						auto framePosition = textureDrawPosition + HealthBarBigFrameOffset * scale;
						DrawHealthBarFrame(canvas, HealthBarBigBgIcon, HEALTH_BAR_BG_COLOR, framePosition, scale);
						DrawHealthBarFrame(canvas, HealthBarBigFrameIcon, playerColor, framePosition, scale);
					}

				} 
				else
				{
					if (actor->IsLocallyControlled() && isLocalCoop) {
						// D11.SSN - draw local player inventory full warning
						if (actor->GetShowInventoryFullWarning()) {
							DrawInventoryFullWarning(canvas, textureDrawPosition, PLAYER_NAME_ARROW_ROW_ALIGNMENT, scale, actor->fullTextDisplayTime);
						}
						//Simple local player name						
						DrawLocalPlayerName(canvas, textureDrawPosition, PLAYER_NAME_ARROW_ROW_ALIGNMENT, scale, playerColor, actor->GetLocalPlayerDisplayText());
					} else {

						//full online player name
						//Health bar
						{
							auto framePosition = textureDrawPosition;
							framePosition += HealthBarBigFrameOffset * scale;
							DrawHealthBarFrame(canvas, HealthBarBigBgIcon, HEALTH_BAR_BG_COLOR, framePosition, scale);
							DrawHealthBarFrame(canvas, HealthBarBigFrameIcon, playerColor, framePosition, scale);
						}

						//Name and arrows
						auto rangedComponent = actor->FindComponentByClass<URangedAttackComponent>();
						if (rangedComponent) {
							if (auto playerState = Cast<ABasePlayerState>(actor->GetDungeonsBasePlayerState())) {							
								DrawPlayerNameAndArrowCount(canvas, playerState, rangedComponent, textureDrawPosition + PLAYER_NAME_ARROW_ROW_OFFSET * scale, PLAYER_NAME_ARROW_ROW_ALIGNMENT, scale);							
							}
						}
					}
				}


			}
			else if (isInHidePortraitRegion) {

				if (isDown && !isBeingRevived) {
					DrawMiniatureSkull(canvas, world, playerColor, textureDrawPosition, scale, downFrameOpacity);
				}
				else {
					auto framePosition = textureDrawPosition + HealthBarFrameOffset * scale;
					DrawHealthBarFrame(canvas, HealthBarBgIcon, HEALTH_BAR_BG_COLOR, framePosition, scale);
					DrawHealthBarFrame(canvas, HealthBarFrameIcon, playerColor, framePosition, scale);
				}

			}
			else {

				if (isDown) {
					DrawSkull(canvas, world, playerColor, textureDrawPosition, scale, isBeingRevived ? 0.0f : downFrameOpacity);
					textureDrawPosition.Y += (SkullFrameSize.Y + currentHealthBarSize.Y) * 0.5f * scale;

				}
				else {
					auto framePosition = textureDrawPosition + PortraitFrameOffset * scale;
					DrawHealthBarFrame(canvas, PortraitBgIcon, HEALTH_BAR_BG_COLOR, framePosition, scale);
					DrawHealthBarFrame(canvas, PortraitFrameIcon, playerColor, framePosition, scale);

					auto portraitPosition = textureDrawPosition + PortraitOffset * scale;
					DrawPlayerPortrait(canvas, hud, avatarComponent, portraitPosition, scale);
				}

			}

			if (!isDown || isBeingRevived) {
				auto healthBarPosition = textureDrawPosition + currentHealthBarOffset * scale;
				const auto healthBarWidth = currentHealthBarSize.X * scale;
				const auto healthBarHeight = currentHealthBarSize.Y * scale;

				if (!isLocalCoop || isBeingRevived)
				{
					DrawHealthBarBackground(canvas, barTexture, portraitBackgroundColor, healthBarPosition, healthBarWidth, healthBarHeight);

					if (isBeingRevived) {
						DrawReviveProgress(canvas, barTexture, activeReviveComponent, HEALTH_BAR_REVIVE_COLOR, healthBarPosition, healthBarWidth, healthBarHeight);
					}
					else {
						DrawAlivePlayerHealthBar(canvas, barTexture, healthBarComponent, playerColor, healthBarPosition, healthBarWidth, healthBarHeight);
					}
				}
			}
		}

		ResetCanvasDrawColor(canvas);
	}
}

void UPlayerIndicators::DrawSkull(UCanvas* canvas, UWorld* world, FColor& portraitColor, FVector2D& textureDrawPosition, const float scale, const float borderOpacity)
{
	auto skullFrameSize = scale * SkullFrameSize;
	DrawSkull(canvas, world, portraitColor, textureDrawPosition, scale, SkullOffset, SkullFrameOffset, skullFrameSize, scale, borderOpacity);
}

void UPlayerIndicators::DrawMiniatureSkull(UCanvas* canvas, UWorld* world, FColor& portraitColor, FVector2D& textureDrawPosition, const float scale, const float borderOpacity)
{
	auto skullFrameSize = scale * MiniatureSkullFrameSize;
	DrawSkull(canvas, world, portraitColor, textureDrawPosition, scale, MiniatureSkullOffset, MiniatureSkullFrameOffset, skullFrameSize, scale * MINIATURE_SKULL_SCALE, borderOpacity);
}

void UPlayerIndicators::DrawSkull(UCanvas* canvas, UWorld* world, FColor& portraitColor, FVector2D& textureDrawPosition, const float scale, FVector2D& skullOffset, FVector2D& skullFrameOffset, FVector2D& skullFrameSize, const float skullIconScale, const float borderOpacity) const
{
	if(borderOpacity > 0.0f){
		for (int i = 0; i < 2; i++){
			const float prg = objectiveindicators::outlineTimeLoopProgress(GetWorld(), 1.25f, 0.7f, (float)i * 0.5f );
			const float borderScale = scale * objectiveindicators::outlineScale(prg) * 1.5f;
			const float frameBorderOpacity = borderOpacity * objectiveindicators::outlineOpacity(prg);
			const FColor frameColor = FColor(255, 255, 255, 255*frameBorderOpacity);
			auto skullFrameBorderPosition = textureDrawPosition - SkullFrameBorderSize * 0.5f * borderScale;
			DrawSkullFrameBorder(canvas, frameColor, skullFrameBorderPosition, borderScale);
		}
	}
	auto skullFramePosition = textureDrawPosition + skullFrameOffset * scale;
	DrawSkullFrame(canvas, portraitColor, skullFramePosition, skullFrameSize);

	auto skullPosition = textureDrawPosition + skullOffset * scale;
	DrawSkullIcon(canvas, skullPosition, skullIconScale);
}

void UPlayerIndicators::DrawSkullFrameBorder(UCanvas* canvas, const FColor& frameColor, FVector2D& skullFramePosition, const float scale) const
{
	canvas->SetDrawColor(frameColor);
	canvas->DrawIcon(SkullFrameBorderIcon, skullFramePosition.X, skullFramePosition.Y, scale);
	ResetCanvasDrawColor(canvas);

}
void UPlayerIndicators::DrawSkullFrame(UCanvas* canvas, FColor& frameColor, FVector2D& skullFramePosition, FVector2D& skullFrameSize) const
{
	canvas->SetDrawColor(frameColor);
	canvas->DrawTile(SkullFrameTexture, skullFramePosition.X, skullFramePosition.Y, skullFrameSize.X, skullFrameSize.Y, 0.0f, 0.0f, 1.0f, 1.0f);
	ResetCanvasDrawColor(canvas);
}

void UPlayerIndicators::DrawSkullIcon(UCanvas* canvas, FVector2D& skullPosition, const float scale) const
{
	canvas->SetDrawColor(FColor::Black);
	canvas->DrawIcon(SkullIcon, skullPosition.X, skullPosition.Y, scale);
	ResetCanvasDrawColor(canvas);
}

void UPlayerIndicators::DrawAlivePlayerHealthBar(UCanvas* canvas, UTexture2D* barTexture, UHealthBarComponent* healthBarComponent, FColor& portraitColor, FVector2D& healthBarPosition, const float healthBarWidth, const float healthBarHeight) const
{
	const auto healthPercentageCurrent = healthBarComponent->GetHealthPercentage();
	const auto healthPercentageSmooth = healthBarComponent->GetSmoothHealthPercentage();

	if (healthPercentageSmooth > healthPercentageCurrent) {
		// Draw damage
		const auto damageAmount = healthPercentageSmooth - healthPercentageCurrent;
		auto damagePosition = healthBarPosition;
		damagePosition.X += healthBarWidth * healthPercentageCurrent;
		DrawHealthBarTile(canvas, barTexture, HEALTH_BAR_DAMAGE_COLOR, damagePosition, healthBarWidth*damageAmount, healthBarHeight, damageAmount);
	}

	// Draw current health bar			
	DrawHealthBarTile(canvas, barTexture, portraitColor, healthBarPosition, healthBarWidth*healthPercentageCurrent, healthBarHeight, healthPercentageCurrent);

	if (healthPercentageSmooth < healthPercentageCurrent) {
		// Draw heal
		const auto healAmount = healthPercentageCurrent - healthPercentageSmooth;
		auto healPosition = healthBarPosition;
		healPosition.X += healthBarWidth * (healthPercentageCurrent - healAmount);
		DrawHealthBarTile(canvas, barTexture, HEALTH_BAR_HEAL_COLOR, healPosition, healthBarWidth*healAmount, healthBarHeight, healAmount);
	}
}

void UPlayerIndicators::DrawHealthBarFrame(UCanvas* canvas, FCanvasIcon& frameIcon, const FColor& frameColor, FVector2D& framePosition, const float scale)
{
	canvas->SetDrawColor(frameColor);
	canvas->DrawIcon(frameIcon, framePosition.X, framePosition.Y, scale);
	ResetCanvasDrawColor(canvas);
}

void UPlayerIndicators::DrawHealthBarTile(UCanvas* canvas, UTexture2D* barTexture, FColor barColor, FVector2D& barPosition, const float barWidth, const float barHeight, const float barTextureWidth) const
{
	canvas->SetDrawColor(barColor);
	canvas->DrawTile(barTexture, barPosition.X, barPosition.Y, barWidth, barHeight, 0.0f, 0.0f, barTextureWidth, 1.0f);
	ResetCanvasDrawColor(canvas);
}

void UPlayerIndicators::DrawLocalPlayerName( UCanvas* Canvas, const FVector2D FramePosition, const FVector2D Alignment, const float Scale, const FColor LocalPlayerColour, const FText& LocalPlayerText)
{
	FVector2D PlayerNameSize = FVector2D(0.0f, 0.0f);
	
	//Player name item
	auto PlayerNameItem = FCanvasTextItem(FVector2D(0, 0), LocalPlayerText, LocalPlayerDisplayTextFont->GetLegacySlateFontInfo(), LocalPlayerColour);

	PlayerNameItem.EnableShadow(PLAYER_NAME_SHADOW_COLOR, PLAYER_NAME_SHADOW_OFFSET);
	PlayerNameItem.Scale.Set(Scale, Scale);
	{
		float outW = 0.0f;
		float outH = 0.0f;
		Canvas->TextSize(LocalPlayerDisplayTextFont, LocalPlayerText.ToString(), outW, outH, Scale, Scale);
		PlayerNameSize = FVector2D(outW, outH);
	}

	FVector2D DrawPosition = FVector2D(FramePosition.X + (PlayerNameSize.X * Alignment.X), FramePosition.Y);
	Canvas->DrawItem(PlayerNameItem, DrawPosition);
}

void UPlayerIndicators::DrawPlayerNameAndArrowCount(UCanvas* Canvas, const ABasePlayerState* playerState, URangedAttackComponent* rangedAttackComponent, const FVector2D FramePosition, const FVector2D Alignment, const float Scale)
{	
	FVector2D PlayerNameSize = FVector2D(0.0f, 0.0f);
	FVector2D ArrowIconSize = FVector2D(0.0f, 0.0f);
	FVector2D ArrowCounterSize = FVector2D(0.0f, 0.0f);

	//Player name item
	auto PlayerNameItem = FCanvasTextItem(FVector2D(0, 0), FText::FromString(playerState->GetPlayerDisplayName()), PlayerNameFont->GetLegacySlateFontInfo(), PLAYER_NAME_COLOR);
	PlayerNameItem.EnableShadow(PLAYER_NAME_SHADOW_COLOR, PLAYER_NAME_SHADOW_OFFSET);
	PlayerNameItem.Scale.Set(Scale, Scale);

	{
		float outW = 0.0f;
		float outH = 0.0f;
		Canvas->TextSize(PlayerNameFont, playerState->GetPlayerDisplayName(), outW, outH, Scale, Scale);
		PlayerNameSize = FVector2D(outW, outH);	
	}

	//Arrow ammo icon
	UItemSlot* AmmoSlot = rangedAttackComponent->GetCurrentAmmoSlot();
	auto AmmoItem = AmmoSlot ? AmmoSlot->GetItem() : nullptr;

	auto AmmoItemType = AmmoItem ? static_cast<FItemId>(AmmoItem->GetItemId()) : game::item::type::Arrow.getId();


	auto ArrowIconData = GetOrLoadAmmoIconSmallForItemType(AmmoItemType);
	if (ArrowIconData) {		
		ArrowIconSize = ArrowIconData->GetSize();		
	}

	ArrowCountText = FText::AsNumber(FMath::Max(0, AmmoSlot ? AmmoSlot->GetDisplayCount() : 0));
	auto ArrowCountItem = FCanvasTextItem(FVector2D(0, 0), ArrowCountText, ArrowCountFont->GetLegacySlateFontInfo(), ARROW_COUNT_COLOR);
	ArrowCountItem.EnableShadow(ARROW_COUNT_SHADOW_COLOR, ARROW_COUNT_SHADOW_OFFSET);
	ArrowCountItem.Scale.Set(Scale, Scale);

	{
		float outW = 0.0f;
		float outH = 0.0f;
		Canvas->TextSize(ArrowCountFont, ArrowCountText.ToString(), outW, outH, Scale, Scale);
		ArrowCounterSize = FVector2D(outW, outH);
	}

	
	float width = PlayerNameSize.X + ArrowIconSize.X * Scale + ArrowCounterSize.X + PLAYER_NAME_ARROW_COUNT_SPACING;

	FVector2D DrawPosition = FVector2D(FramePosition.X + width * Alignment.X, FramePosition.Y);
	Canvas->DrawItem(PlayerNameItem, DrawPosition + FVector2D(0.0f, PlayerNameSize.Y * Alignment.Y) + PLAYER_NAME_OFFSET * Scale);

	DrawPosition.X += PlayerNameSize.X + PLAYER_NAME_ARROW_COUNT_SPACING;

	if (ArrowIconData) {
		FVector2D IconDrawPosition = FVector2D(DrawPosition.X, DrawPosition.Y + ArrowIconSize.Y * Alignment.Y * Scale) + ARROW_SPRITE_OFFSET * Scale;
		Canvas->DrawIcon(ArrowIconData->GetIcon(), IconDrawPosition.X, IconDrawPosition.Y , Scale);
		DrawPosition.X += ArrowIconSize.X * Scale;
	}
	Canvas->DrawItem(ArrowCountItem, DrawPosition + FVector2D(0.0f, ArrowCounterSize.Y * Alignment.Y) + ARROW_COUNT_OFFSET * Scale);
}


void UPlayerIndicators::DrawHealthBarBackground(UCanvas* canvas, UTexture2D* barTexture, FColor& barColor, FVector2D& healthBarPosition, const float healthBarWidth, const float healthBarHeight) const
{	
	canvas->SetDrawColor({barColor.R, barColor.G, barColor.B, HEALTH_BAR_ALPHA });
	canvas->DrawTile(barTexture, healthBarPosition.X, healthBarPosition.Y, healthBarWidth, healthBarHeight, 0.0f, 0.0f, 1.0f, 1.0f);
}

void UPlayerIndicators::DrawPlayerPortrait(UCanvas* canvas, AHUD* hud, UPlayerAvatarComponent* avatarComponent, FVector2D& portraitPosition, const float scale) const
{
	const FAvatarData avatarData = avatarComponent->GetPlayerAvatarData();
	const auto material = avatarData.PortraitMaterial;
	if (material != nullptr) {
		ResetCanvasDrawColor(canvas);
		hud->DrawMaterial(material, portraitPosition.X, portraitPosition.Y, PortraitSize.X, PortraitSize.Y, 0.0f, 0.0f, 1.0f, 1.0f, scale, false, 0.0f, FVector2D(0.0f, 0.0f));
	}
}

void UPlayerIndicators::DrawReviveProgress(UCanvas* canvas, UTexture2D* barTexture, UReviveComponent* const activeReviveComponent, const FColor& barColor, FVector2D& healthBarPosition, const float healthBarWidth, const float healthBarHeight) const
{
	const auto reviveProgress = activeReviveComponent->GetProgress();
	const auto FinalColor = barColor.WithAlpha(FMath::Clamp(0.5f + 0.5f* FMath::Max(0.0f, FMath::Cos(reviveProgress * 30.0f * PI)), 0.0f, 1.0f) * 255);
	DrawHealthBarTile(canvas, barTexture, FinalColor, healthBarPosition, healthBarWidth*reviveProgress, healthBarHeight, 0.0f);
}

FVector2D UPlayerIndicators::GetProjectedClamped(FVector2D& viewportSize, FVector& projectedPosition, const float scale) const
{
	const auto projectedViewportPosition = CanvasUtility::ClampToViewport(projectedPosition, viewportSize);
	return CanvasUtility::ClampToViewportEdge(projectedViewportPosition, viewportSize, ViewportPadding + Bounds * 0.5 * scale);
}

FVector2D UPlayerIndicators::GetTextureDrawPosition(FVector2D& projectedClamped, const bool isInViewport, const float scale) const
{
	// Origin of art is top left
	auto textureDrawPosition = projectedClamped;

	/*if (!isInViewport)
	{
		textureDrawPosition.X -= Bounds.X * 0.5 * scale;
		textureDrawPosition.Y -= Bounds.Y * 0.5 * scale;
	}*/

	// Snap to whole pixels
	textureDrawPosition.X = roundf(textureDrawPosition.X);
	textureDrawPosition.Y = roundf(textureDrawPosition.Y);

	return textureDrawPosition;
}

void UPlayerIndicators::ResetCanvasDrawColor(UCanvas* canvas)
{
	canvas->SetDrawColor(FColor::White);
}

UReviveComponent* UPlayerIndicators::GetActiveReviveComponent(AActor* owner)
{
	const auto reviveComponent = owner->FindComponentByClass<UReviveComponent>();
	if (reviveComponent && reviveComponent->IsCurrentlyRevivingLocal())
	{
		return reviveComponent;
	}
	return nullptr;
}

const UPlayerIndicators::CanvasIconData* UPlayerIndicators::GetOrLoadAmmoIconSmallForItemType(const FItemId& ItemType) {
	if (ItemTypeAmmoIconMap.Contains(ItemType)) {
		return &ItemTypeAmmoIconMap[ItemType];
	} else {
		auto tex = UItemFunctionLibrary::GetAmmoIconSmallTextureForItemType(ItemType);
		if (tex) {
			FVector2D size;
			FCanvasIcon icon = CanvasUtility::MakeIconFullTexture(tex, size, 1.0f);
			CanvasIconData data{ icon, size };

			ItemTypeAmmoIconMap.Add(ItemType, data);
			LoadedAmmoTextures.Add(tex);
			return &ItemTypeAmmoIconMap[ItemType];
		}
	}
	return nullptr;
}

// D11.SSN
#define LOCTEXT_NAMESPACE "Player_Indicators"
void UPlayerIndicators::DrawInventoryFullWarning(UCanvas* Canvas, const FVector2D& FramePosition, const FVector2D& Alignment, const float Scale, const float DisplayTime) {
	FVector2D fullTextSize = FVector2D(0.0f, 0.0f);

	// animate position & colour to create fade-in & rise-up effects.
	FVector2D animatedPosition = FramePosition + PLAYER_INVENTORY_FULL_ROW_OFFSET * std::min(DisplayTime * 5.0f, 1.0f) * Scale;
	FColor colour = FColor::White.WithAlpha(255 * std::min(DisplayTime * 5.0f, 1.0f));

	auto fullTextItem = FCanvasTextItem(FVector2D(0, 0), FText(LOCTEXT("Inventory_Full_Indicator", "FULL!")), LocalPlayerDisplayTextFont->GetLegacySlateFontInfo(), colour);

	fullTextItem.EnableShadow(PLAYER_NAME_SHADOW_COLOR, PLAYER_NAME_SHADOW_OFFSET);
	fullTextItem.Scale.Set(Scale, Scale);
	{
		float outW = 0.0f;
		float outH = 0.0f;
		Canvas->TextSize(LocalPlayerDisplayTextFont, FText(LOCTEXT("Inventory_Full_Indicator", "FULL!")).ToString(), outW, outH, Scale, Scale);
		fullTextSize = FVector2D(outW, outH);
	}

	FVector2D DrawPosition = FVector2D(animatedPosition.X + (fullTextSize.X * Alignment.X), animatedPosition.Y);
	Canvas->DrawItem(fullTextItem, DrawPosition);
}
#undef LOCTEXT_NAMESPACE