#include "Dungeons.h"
#include "WraithFormations.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/BtEvalState.h"
#include <random>
#include "util/Random.h"

namespace bt { namespace formation {

	static const std::vector<std::vector<std::string>> icePatterns { {
		"XXX",
		"XXX",
		"XXX"		
	}, {
		".xXx.",
		"xXXXx",
		"XXXXX",
		"xXXXx",		
		".xXx."		
	}, {
		"..xXx..",
		".XXXXX.",
		"xXXXXXx",
		"XXXXXXX",
		"xXXXXXx",
		".XXXXX.",
		"..xXx.."
	}, {
		"...x..x.xxx.x..",
		".xXXx.xx.xXXx..",
		".xXx.xx.x.XXXXx",
		".x.XXXxXxXXXxx.",
		"xXxXxxXXxxXXXXx",
		".xxXXXxXxXxxXx.",
		"xxx.XxXxXxXXXx.",
		"xxXxxXxXxXxXXXx",
		".x.XXxXxXxXXx.x",
		"xx.XXXxXxXxxXx.",
		".xxXxXxxXxXXXxx",
		"x.XxXxxXxx.XXXx",
		".x.XXxXxxXxXxx.",
		"xXxXxxXxXxx.xXx",
		".x.XXxXx.x.Xx..",
		"...x.x.xx..xx.."
	} };

	FVector snap(float blockSize, const FVector& freeLocation) {
		return FVector {
			(blockSize / 2.f) + FMath::FloorToFloat(freeLocation.X / blockSize) * blockSize,
			(blockSize / 2.f) + FMath::FloorToFloat(freeLocation.Y / blockSize) * blockSize,
			(blockSize / 2.f) + FMath::FloorToFloat(freeLocation.Z / blockSize) * blockSize
		};
	}

	std::vector<std::pair<FVector, float>> generateIceFormation(uint32_t seed, const FVector& location, int patternIndex, float unitSize, float spreadSpeed) {
		Random rnd { seed };

		std::vector<std::pair<FVector, float>> formation {};

		const auto gridLocation = snap(100.f, location);

		const auto& icePattern = icePatterns[patternIndex];

		for (signed char lineIndex = 0; lineIndex < icePattern.size(); lineIndex++) {
			const auto& line = icePattern[lineIndex];

			for (signed char columnIndex = 0; columnIndex < line.size(); columnIndex++) {
				const auto& cell = line[columnIndex];

				if (cell == 'X' || (cell == 'x' && rnd.nextBoolean())) {
					const signed char lineLocal = lineIndex - icePattern.size() / 2;
					const signed char columnLocal = columnIndex - line.size() / 2;
					const auto cellLocation = FVector {
						static_cast<float>(lineLocal),
						static_cast<float>(columnLocal),
						0.f
					} * unitSize + gridLocation;

					formation.push_back({
						cellLocation,
						(FMath::Abs(lineLocal) + FMath::Abs(columnLocal)) * spreadSpeed
					});
				}
			}
		}

		return formation;
	}
	
	std::function<void(bt::StateRef, USummonFormationTask::AddEntryRef)> makeIceRandom(
		const TSubclassOf<AActor>& centerClass,
		const TSubclassOf<AActor>& outerClass,
		int patternIndex,
		float unitSize,
		const bt::Duration& spreadSpeed
	) {
		static Random rnd;

		return [=](bt::StateRef state, USummonFormationTask::AddEntryRef add) {
			if (state.params().target.HasActor()) {
				const AActor* target = state.params().target;
				const auto freeLocation = target->GetActorLocation();

				std::random_device generator;
				const auto seed = generator();

				for (auto&& entry : generateIceFormation(seed, freeLocation, patternIndex, unitSize, spreadSpeed.Seconds())) {

					add(
						1.s * entry.second,
						EntityType::Undefined,
						entry.second == 0.f ? centerClass : outerClass,
						FTransform { entry.first },
						true,
						false
					);
				}
			}			
		};
	}
}}
