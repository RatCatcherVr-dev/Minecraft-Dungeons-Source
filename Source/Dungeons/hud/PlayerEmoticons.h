#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/Engine/Canvas.h"
#include "ui/map/MapPinWidget.h"
#include "game/emotes/Emotes.h"
#include "PlayerEmoticons.generated.h"

UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API AChatMapActor : public AActor {
	GENERATED_BODY()
public:
	AChatMapActor();

	void Tick(float deltaTime) override;

	void SetMapPinWidget(TSubclassOf<UMapPinWidget> mapPinClass);

	void SetFollowPlayer(APlayerCharacter* player);
private:
	UPROPERTY(VisibleAnywhere)
	class UMapPinComponent* MapPinComp;

	UPROPERTY()
	TWeakObjectPtr<APlayerCharacter> FollowPlayer;
};


UCLASS()
class DUNGEONS_API UPlayerEmoticons : public UObject {
	GENERATED_UCLASS_BODY()

	class EmoteState {
	public:
		const EEmote emote;
		const float until;
		TOptional<FVector> detachedWorldPosition;
	};

public:
	void SetEmote(UWorld* world, const FVector& location, int32 playerId, EEmote emote);
	
	float GetEmoticonWeight(const FVector2D& location, const FVector2D& viewportSize) const;

	void Draw(UCanvas* canvas, APlayerController* playerOwner, float dpi_scale) const;

private:


	static const float MASTER_SCALE;
	static const float ICON_SCALE;
	static const float BUBBLE_SCALE;
	static const float EMOTE_DURATION;
	static const float EMOTE_DETACHED_DURATION;
	static const float EMOTE_FADE_OUT_TIME;
	static const float EMOTE_FADE_IN_TIME;
	static const float EMOTE_JUICE_SCALE;
	static const float EMOTE_JUICE_JUMP;
	static const float ICON_PADDING;
	static const float TEXTURE_PADDING;
	
	static const FVector2D WORLD_CHARACTER_TALK_OFFSET;

	void SpawnMapActor(int32 playerId, const FSoftClassPath& classPath, bool followPlayer, UWorld* world, const FVector& location, float duration);
	void DrawBubble(const EmoteState* iconState, const FVector2D& viewportSize, const APlayerCharacter* actor, UCanvas* canvas, APlayerController* playerOwner, float dpi_scale) const;

	APlayerCharacter* GetPlayerFromId(UWorld*, int32 ID) const;

	FVector2D TalkBubbleSize;
	FVector2D TalkBubbleOrientedSize;
	FVector2D DetachedBubbleSize;

	UPROPERTY()
	TMap<EEmote, FCanvasIcon> EmoteCanvasIcons;	
	
	UPROPERTY()
	FCanvasIcon BubbleIcon;
	
	UPROPERTY()
	FCanvasIcon BubbleIconN;
	
	UPROPERTY()
	FCanvasIcon BubbleIconW;

	UPROPERTY()
	FCanvasIcon BubbleIconS;

	UPROPERTY()
	FCanvasIcon BubbleIconE;

	UPROPERTY()
	FCanvasIcon DetachedBubbleIcon;

	UPROPERTY()
	FCanvasIcon DetachedBubbleShadow;
	
	TMap<int, EmoteState> TalkBubbleMapping;	

	TMap<int, TWeakObjectPtr<AChatMapActor>> MapActorMapping;
};
