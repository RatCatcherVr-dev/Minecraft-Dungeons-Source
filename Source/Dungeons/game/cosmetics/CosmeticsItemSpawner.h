#pragma once

#include "game/actor/cosmetics/CosmeticItemInfo.h"

namespace game { namespace cosmetics {	
	ACosmeticItemInfo* spawnCosmeticItemInfo(UWorld* world, AActor* owner, TSubclassOf<ACosmeticItemInfo> cosmeticItemClass);
}}
