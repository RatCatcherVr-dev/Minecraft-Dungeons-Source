/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"
#include "world/entity/player/Player.h"

#define PLAYER_MAX_LEVEL 24791

bool Player::isCreative() const {
// 	return (mPlayerGameType == GameType::Creative) || (mPlayerGameType == GameType::CreativeViewer);
	return false;
}

ItemInstance* Player::getSelectedItem() const {
// 	return mInventoryProxy->getSelectedItem();
	return nullptr;
}
