#include "Dungeons.h"
#include "ObjectiveCameraPanningDefs.h"
#include "ObjectiveCameraPanning.h"
#include "LoadingScreen/ScreenFader.h"
#include "game/actor/DungeonsPlayerCameraManager.h"
#include "game/actor/character/player/BasePlayerController.h"


namespace game { namespace objective {

PanningSequence movingPanningSequence(UWorld& world, APlayerCharacter& player, const std::vector<TransformData>& transformData, float duration) {
	PanningSequence sequence;

	if (transformData.size() <= 0) {
		return std::move(sequence);
	}

	std::string name = "firstaction";
	for (auto& transform : transformData) {
		sequence.addAction(world, std::make_unique<TransformAction>(0.0f, duration, name, transform, MoveInterpolationInfo(MoveInterpolationMethod::EaseOut, 2.0f)));
		name = "camerapanning";
	}

	auto cameraOffset = GetCameraOffset(&player);
	sequence.addAction(world, std::make_unique<TransformAction>(duration, duration, "lastaction", TransformData(TransformTarget(transformData.back().mEndPosition.GetPosition()), TransformTarget( &player, cameraOffset)), MoveInterpolationInfo(MoveInterpolationMethod::EaseOut, 2.0f)));

	return std::move(sequence);
}

}}
