#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

/**
 * 
 */
class DUNGEONS_API DebugList {
public:
	DebugList(AHUD*, const FVector2D&);
	
	void Draw(const FString&, const FColor& = FColor::White, int indent = 0);

	void NewLine();

private:
	AHUD* hud;

	FVector2D offset;

	int lineCount;

	const static int lineHeight;
};
