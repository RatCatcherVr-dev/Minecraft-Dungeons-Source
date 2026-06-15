#pragma once
#include "game/actor/character/mob/MobCharacter.h"

#include <WeakObjectPtrTemplates.h>

class InputController
{
public:
	static const uint8 SilhouetteMask;
	static const uint8 OutlineMask;

	enum class EOutlineState : uint8 {
		None = 0x0,
		Interactable = 0x01,
		Target = 0x02,
		RangedTarget = 0x03, // D11.DB

		LocalP1Outline = 0x04, //D11.KS
		LocalP2Outline = 0x05, //D11.KS
		LocalP3Outline = 0x06, //D11.KS
		LocalP4Outline = 0x07, //D11.KS

		LocalP1TargetOutline = 0x08, //D11.KS
		LocalP2TargetOutline = 0x09, //D11.KS
		LocalP3TargetOutline = 0x0A, //D11.KS
		LocalP4TargetOutline = 0x0B, //D11.KS

		LocalP1RangedTargetOutline = 0x0C, //D11.KS
		LocalP2RangedTargetOutline = 0x0D, //D11.KS
		LocalP3RangedTargetOutline = 0x0E, //D11.KS
		LocalP4RangedTargetOutline = 0x0F, //D11.KS
	};

	enum class ESilhouetteState : uint8 {
		None = 0x0,
		OccludedFriendy = 0x10,
		OccludedUnfriendly = 0x20,
		OccludedCommonItem = 0x10,
		OccludedRareItem = 0x30,
		OccludedUniqueItem = 0x40,
		OccludedLocalP1 = 0x50,
		OccludedLocalP2 = 0x60,
		OccludedLocalP3 = 0x70,
		OccludedLocalP4 = 0x80,

	};

	FORCEINLINE static uint8 CombinedState(EOutlineState outline, ESilhouetteState silhouette) { return static_cast<uint8>(outline) | static_cast<uint8>(silhouette); }
};
