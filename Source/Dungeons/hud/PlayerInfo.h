#pragma once

#include "util/TileDebug.h"
#include "hud/DebugList.h"

/**
 * 
 */
class DUNGEONS_API PlayerInfo {
public:
	PlayerInfo() = default;
	
	void PrintTileDebug(DebugList&, const Util::TileDebugData&, int indentLevel = 0);

	void PrintMobDebug(DebugList& mobInfo, AActor* mob);

	void Draw(AHUD*, APlayerController*);
	
	~PlayerInfo() = default;

private:
	void _PrintBlockMaterial(const FString& header, DebugList&, const class Block&, const BlockGraphicsPack& blockGraphicsPack) const;

	void _PrintComponent(const FString&, DebugList&, const UPrimitiveComponent*) const;

	TOptional<Util::TileDebugData> TileDebugData;
};
