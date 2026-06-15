#pragma once

#include "GameFramework/HUD.h"
#include "hud/PlayerIndicators.h"
#include "hud/PlayerEmoticons.h"
#include "hud/ObjectiveIndicators.h"
#include "hud/HealthBars.h"
#include "hud/HotBarBackground.h"
#include "hud/PlayerInfo.h"
#include "hud/NetworkInfo.h"
#include "game/emotes/Emotes.h"
#include "DungeonHUD.generated.h"


UCLASS()
class DUNGEONS_API ADungeonHUD : public AHUD
{	
	GENERATED_BODY()

public:
	ADungeonHUD(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	
	bool IsVisible;

	UFUNCTION()
	void SetEmote(int32 playerId, EEmote emote);
		
	void DrawHUD() override;

	UFUNCTION(BlueprintCallable)
	void SetHotbarBackgroundVisibility(bool visible);

	UFUNCTION(BlueprintCallable)
	void SetHotbarBackgroundOffset(FVector2D vector);

	UFUNCTION(BlueprintCallable)
	void SetCoopHotbarBackgroundOffset(int index, FVector2D vector);

	UFUNCTION(BlueprintCallable)
	void SetHotbarSize(FVector2D size);

	UFUNCTION(BlueprintCallable)
	void SetHotbarBackgroundScaling(float scale);

	UFUNCTION(BlueprintCallable)
	void SetIsVisible(bool visible);
	
	float GetUMG_DPI_Scale() const;

private:
	UPROPERTY()
	UPlayerIndicators* Indicators;
	
	UPROPERTY()
	UPlayerEmoticons* Emoticons;

	UPROPERTY()
	UObjectiveIndicators* Objectives;

	UPROPERTY()
	UHealthBars* HealthBars;

	UPROPERTY()
	UHotBarBackground* Hotbar;
	
	PlayerInfo PlayerInfo;

	NetworkInfo NetworkInfo;
	
	bool BoundPlayer;	

};