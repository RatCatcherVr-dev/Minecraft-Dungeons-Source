#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/Canvas.h"
#include "game/component/ReviveComponent.h"
#include "game/component/RangedAttackComponent.h"
#include "game/item/ItemTypeDefs.h"
#include "game/actor/SuddenDeath.h"
#include "PlayerIndicators.generated.h"


UCLASS()
class DUNGEONS_API UPlayerIndicators : public UObject {

	class CanvasIconData {
	private:
		FCanvasIcon mCanvasIcon;
		FVector2D mSize;

	public:
		CanvasIconData(FCanvasIcon CanvasIcon, FVector2D Size) :
			mCanvasIcon(CanvasIcon)
			, mSize(Size)
		{}


		FCanvasIcon GetIcon() const {
			return mCanvasIcon;
		}

		FVector2D GetSize() const {
			return mSize;
		}
	};

	GENERATED_UCLASS_BODY()

	void Draw(UCanvas* canvas, APlayerController* playerOwner, float dpi_scale, AHUD* hud);

	static const float MASTER_SCALE;
	static const float MINIATURE_SKULL_SCALE;
	static const FVector2D EMOTE_PADDING;

	void SetSuddenDeathActor(ASuddenDeath* suddenDeath);

private:

	void DrawSkull(UCanvas* canvas, UWorld* world, FColor& portraitColor, FVector2D& textureDrawPosition, float scale, const float borderOpacity);
	void DrawMiniatureSkull(UCanvas* canvas, UWorld* world, FColor& portraitColor, FVector2D& textureDrawPosition, float scale, const float borderOpacity);
	void DrawSkull(UCanvas* canvas, UWorld* world, FColor& portraitColor, FVector2D& textureDrawPosition, float scale, FVector2D& skullOffset, FVector2D& skullFrameOffset, FVector2D& skullFrameSize, float skullIconScale, const float borderOpacity) const;
	void DrawSkullFrameBorder(UCanvas* canvas, const FColor& frameColor, FVector2D& skullFramePosition, const float scale) const;
	void DrawSkullFrame(UCanvas* canvas, FColor& frameColor, FVector2D& skullFramePosition, FVector2D& skullFrameSize) const;
	void DrawSkullIcon(UCanvas* canvas, FVector2D& skullPosition, float scale) const;

	void DrawAlivePlayerHealthBar(UCanvas* canvas, UTexture2D* barTexture, UHealthBarComponent* healthBarComponent, FColor& portraitColor, FVector2D& healthBarPosition, float healthBarWidth, float healthBarHeight) const;

	static void DrawHealthBarFrame(UCanvas* canvas, FCanvasIcon& frameIcon, const FColor& frameColor, FVector2D& framePosition, float scale);
	void DrawHealthBarTile(UCanvas* canvas, UTexture2D* barTexture, FColor barColor, FVector2D& barPosition, float barWidth, float barHeight, float barTextureWidth) const;
	void DrawHealthBarBackground(UCanvas* canvas, UTexture2D* barTexture, FColor& barColor, FVector2D& healthBarPosition, float healthBarWidth, float healthBarHeight) const;

	void DrawLocalPlayerName(UCanvas* Canvas, const FVector2D FramePosition, const FVector2D Alignment, const float Scale, const FColor LocalPlayerColour, const FText& LocalPlayerText);
	void DrawPlayerNameAndArrowCount(UCanvas* Canvas, const ABasePlayerState* playerState, URangedAttackComponent* rangedAttackComponent, const FVector2D FramePosition, const FVector2D Alignment, const float Scale);
	void DrawPlayerPortrait(UCanvas* canvas, AHUD* hud, UPlayerAvatarComponent* avatarComponent, FVector2D& portraitPosition, float scale) const;	

	void DrawArrowCount(UCanvas* Canvas, URangedAttackComponent* rangedAttackComponent, const FVector2D FramePosition, const FVector2D Alignment, const float Scale);

	void DrawReviveProgress(UCanvas* canvas, UTexture2D* barTexture, UReviveComponent*const activeReviveComponent, const FColor& barColor, FVector2D& healthBarPosition, const
	                        float healthBarWidth, const float healthBarHeight) const;

	// D11.SSN
	void DrawInventoryFullWarning(UCanvas* Canvas, const FVector2D& FramePosition, const FVector2D& Alignment, const float Scale, const float DisplayTime);

	FVector2D GetProjectedClamped(FVector2D& viewportSize, FVector& projectedPosition, float scale) const;
	FVector2D GetTextureDrawPosition(FVector2D& projectedClamped, bool isInViewport, float scale) const;

	static UReviveComponent* GetActiveReviveComponent(AActor* owner);

	static void ResetCanvasDrawColor(UCanvas* canvas);

	static const FColor HEALTH_BAR_HEAL_COLOR;
	static const FColor HEALTH_BAR_DAMAGE_COLOR;
	static const FColor HEALTH_BAR_BG_COLOR;

	static const FColor HEALTH_BAR_REVIVE_COLOR;

	static const FColor PLAYER_NAME_COLOR;
	static const FColor PLAYER_NAME_SHADOW_COLOR;

	static const FColor ARROW_COUNT_COLOR;
	static const FColor ARROW_COUNT_SHADOW_COLOR;

	static const FBox2D VIEWPORT_HIDE_PORTRAIT_REGION;

	static const uint8 HEALTH_BAR_ALPHA;

	FVector2D ViewportPadding;
	FVector2D Bounds;

	FVector2D PortraitFrameOffset;
	FVector2D PortraitOffset;
	FVector2D PortraitSize;

	FVector2D HealthBarFrameOffset;
	FVector2D HealthBarOffset;
	FVector2D HealthBarSize;

	FVector2D HealthBarBigFrameOffset;
	FVector2D HealthBarBigOffset;
	FVector2D HealthBarBigSize;
	FVector2D HealthBarBigFrameSize;

	FVector2D SkullFrameOffset;
	FVector2D SkullFrameSize;
	FVector2D SkullFrameBorderSize;

	FVector2D SkullOffset;
	FVector2D SkullSize;

	FVector2D MiniatureSkullFrameOffset;
	FVector2D MiniatureSkullFrameSize;
	FVector2D MiniatureSkullOffset;
	FVector2D MiniatureSkullSize;

	UPROPERTY()
	FCanvasIcon PortraitBgIcon;

	UPROPERTY()
	FCanvasIcon PortraitFrameIcon;

	UPROPERTY()
	FCanvasIcon HealthBarBgIcon;

	UPROPERTY()
	FCanvasIcon HealthBarFrameIcon;

	UPROPERTY()
	FCanvasIcon HealthBarBigFrameIcon;

	UPROPERTY()
	FCanvasIcon HealthBarBigBgIcon;

	UPROPERTY()
	FCanvasIcon PortraitIcon;

	UPROPERTY()
	UTexture2D* HealthBarTexture;

	UPROPERTY()
	UTexture2D* HealthBarBigTexture;

	UPROPERTY()
	FCanvasIcon SkullIcon;

	UPROPERTY()
	FCanvasIcon SkullFrameIcon;

	UPROPERTY()
	FCanvasIcon SkullFrameBorderIcon;

	UPROPERTY()
	UTexture2D* SkullFrameTexture;

	UPROPERTY()
	UMaterial* PortraitMaterial;


	static const FVector2D ARROW_COUNT_SHADOW_OFFSET;	
	static const FVector2D ARROW_SPRITE_OFFSET;	
	static const FVector2D ARROW_COUNT_OFFSET;

	static const FVector2D PLAYER_NAME_OFFSET;
	static const FVector2D PLAYER_NAME_SHADOW_OFFSET;

	static const FVector2D PLAYER_NAME_ARROW_ROW_ALIGNMENT;
	static const FVector2D PLAYER_NAME_ARROW_ROW_OFFSET;

	// D11.SSN
	static const FVector2D PLAYER_INVENTORY_FULL_ROW_OFFSET;

	static const float PLAYER_NAME_ARROW_COUNT_SPACING;

	
	UFont* ArrowCountFont;
	FText ArrowCountText;

	UFont* PlayerNameFont;
	UFont* LocalPlayerDisplayTextFont;

	UPROPERTY()
	TArray<UTexture2D*> LoadedAmmoTextures;
	TMap<FItemId, CanvasIconData> ItemTypeAmmoIconMap;

	UPROPERTY()
	TWeakObjectPtr<ASuddenDeath> SuddenDeathActor;

	const CanvasIconData* GetOrLoadAmmoIconSmallForItemType(const FItemId& ItemType);
};

