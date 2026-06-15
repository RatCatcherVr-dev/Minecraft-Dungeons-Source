#pragma once

#include "CoreMinimal.h"

class ABaseCharacter;
class AItemInstance;

namespace game { namespace item { namespace power { namespace format {
	DUNGEONS_API extern FString AbsoluteDamage(const ABaseCharacter& owner, const AItemInstance* instance, float multiplier);	
	DUNGEONS_API extern FString AbsoluteDamagePerSecond(const ABaseCharacter& owner, const AItemInstance* instance, float multiplier);
	DUNGEONS_API extern FString AbsoluteMaxHealthIncrease(const ABaseCharacter& owner, const AItemInstance* instance, float multiplier);
	DUNGEONS_API extern FString AbsoluteItemHealth(const ABaseCharacter& owner, const AItemInstance* instance, float multiplier);
	DUNGEONS_API extern FString AbsoluteHealthHealed(const ABaseCharacter& owner, const AItemInstance* instance, float multiplier);
}}}}