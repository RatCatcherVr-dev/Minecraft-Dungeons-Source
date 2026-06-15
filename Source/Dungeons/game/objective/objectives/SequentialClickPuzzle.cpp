#include "Dungeons.h"
#include "SequentialClickPuzzle.h"
#include "SpawnQueue.h"
#include "game/Conversion.h"
#include "game/Game.h"
#include "game/GameBP.h"
#include "game/GameTypes.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/BackpackComponent.h"
#include "game/component/ReplicatedInteractableComponent.h"

#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "game/mobspawn/SpawnRegions.h"
#include "game/objective/ObjectiveUtil.h"
#include "game/objective/ObjectiveDoorLocker.h"
#include "game/objective/TargetLocators.h"
#include "game/util/ActorQuery.h"
#include "game/util/Tags.h"
#include "lovika/RegionPredicates.h"
#include "lovika/io/IoObjectiveTypes.h"
#include "ui/MissionProgressHandler.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"
#include "util/RandomUtil.h"
#include "util/Validation.h"

#include <Components/ActorComponent.h>
#include <UnrealString.h>
#include "game/actor/interfaces/Activatable.h"

namespace game { namespace objective {

SequentialClickPuzzle::SequentialClickPuzzle(const io::ObjectiveClickData& data)
	: mData{ data }
	, firstPickup{ false } {

	if (mData.mobs.IsSet()) {
		for (auto&& group : mData.mobs->groups) {
			mobspawn::prepareMobGroup(group);
		}
	}
	markPartiallyCompleted(0, mData.count);
}

SequentialClickPuzzle::~SequentialClickPuzzle() {}

Validation SequentialClickPuzzle::validate(ValidationType validationType) const {
	if (validationType == ValidationType::Start) {
		return {};
	}

	const bool enoughRegions = mLocations.size() >= mData.count;
	const bool foundKeyRegions = !targetFinder().get(mData.keyLocations).empty();

	auto validation = validationErrorsIf({
		{ mData.locations.empty(), "No region locators defined in objective json" },
		{ !enoughRegions, std::string("Fewer matching regions than requested: ") + std::to_string(mLocations.size()) + " < " + std::to_string(mData.count) },
		{ enoughRegions && mObjects.Num() != mData.count, std::string("Failed to create all objects. ObjectNotFound, No UReplicatedInteractableComponent, Couldn't Spawn?: ") + std::to_string(mObjects.Num()) + " of " + std::to_string(mData.count) },
		{ !mData.keyLocations.empty() && !foundKeyRegions, "Failed to locate key regions (key-locations)" },
		{ !mData.keyLocations.empty() && foundKeyRegions && mKeyMob == nullptr, "Failed to spawn key" }
		});
	const bool hasSingleMobStretchTile = mData.mobStretch && targetFinder().singleTile(targetlocators::allTilesInStretch(mData.mobStretch.GetValue()));
	validationIssuesIf(validation, {
		std::make_tuple(IssueType::Warning, mData.mobStretch && !hasSingleMobStretchTile, "\"mob-stretch\" must refer to a single tile stretch: " + mData.mobStretch.Get("<unnamed stretch>") + ". NOTE: This limitation doesn't exist if you use \"spawn-regions\": [] instead of \"mob-stretch\""),
		std::make_tuple(IssueType::Warning, mData.mobStretch && mSpawnRegions.isEmpty(), "Didn't find any arena spawn regions for stretch: " + mData.mobStretch.Get("<unnamed stretch>")),
		std::make_tuple(IssueType::Warning, mData.spawnRegions && mSpawnRegions.isEmpty(), "Didn't find any matching spawn regions: " + Util::join(mData.spawnRegions.Get({}))),
		});
	return validation;
}

void SequentialClickPuzzle::onInit() {
	mLocations = targetFinder().get(mData.locations);

	Random rnd(game().settings().randomSeed);

	if (!_spawnKeyIfWanted(rnd)) {
		return;
	}

	mDoorLocker = make_unique<DoorLocker>(
		game().world(),
		doorsFromRegionLocators(game().tiles(), mData.lockedDoors),
		mData.doorPath.Get("")
		);

	if (mData.mobs.IsSet()) {
		mSpawnQueue = make_unique<SpawnQueue>(
			game().world(),
			mobspawn::configs::Default(game().settings().difficulty, true),
			"normal",
			[this](AMobCharacter* mob) { mMobs.Add(mob); }
		);

		if (auto locators = _getSpawnRegionLocators()) {
			auto spawnRegions = filtered(targetFinder().get(locators.GetValue()), regionpredicates::isPuzzleSpawn());
			mSpawnRegions = mobspawn::Regions(std::move(spawnRegions));
		}
	}

	const auto objectPath = game::PrefabPath(mData.object.c_str());
	const bool requiresKey = !mData.keyLocations.empty();

	const Positions positions = generatePositions(mData.count, rnd);
	for (auto&& pos : positions.objects) {
		auto rotation = 90;// * (1 + rnd.nextInt(2));
		if (auto object = placeObject(game().world(), objectPath, pos, rotation)) {
			if (auto interactable = object->FindComponentByClass<UReplicatedInteractableComponent>()) {
				interactable->ValidateCallback = [this, requiresKey](const UReplicatedInteractableComponent& component, const AActor* instigator) {
					if (!isStarted()) {
						return false;
					}
					if (mData.isSequential) {
						if (indexOf(*component.GetOwner()) != algo::count_if(mObjects, RETLAMBDA(it.clicked))) {
							return false;
						}
					}
					if (requiresKey) {
						const auto backpackComponent = instigator->FindComponentByClass<UMochilaComponent>();
						return backpackComponent && backpackComponent->Has(keyType());
					}
					return true;
				};

				interactable->OnInteractCallback = [this, requiresKey](const UReplicatedInteractableComponent& component, AActor* instigator) {
					if (requiresKey) {
						if (const auto backpackComponent = instigator->FindComponentByClass<UMochilaComponent>()) {
							backpackComponent->Clear();
						}
					}

					_onClickedObject(*component.GetOwner());
				};

				mObjects.Add({ object, interactable, false });
			}
		}
	}
	if (mData.dummyObject) {
		const auto dummyObjectPath = game::PrefabPath(mData.dummyObject.GetValue());
		for (auto&& pos : positions.dummies) {
			placeObject(game().world(), dummyObjectPath, pos, 90);
		}
	}
}

bool SequentialClickPuzzle::_spawnKeyIfWanted(Random& rnd) {
	if (mData.keyLocations.empty()) {
		return true;
	}
	const auto spawnConfig = mobspawn::configs::DefaultNoVariants(true).AddTag(tags::pristine);

	// @note: we normally shouldn't need this, but since we REALLY want this thing to spawn, we try all spawn regions explicitly (instead of trying X times randomly)
	for (auto& region : algo::random::shuffledCopy(targetFinder().get(mData.keyLocations), rnd)) {
		mobspawn::Regions spawnRegions({ region });
		const auto transformProvider = mobspawn::providers::FromProviders(mobspawn::providers::position::Regions(spawnRegions, &rnd));

		if (const auto mob = mobspawn::spawnNow(game().world(), keyType(), transformProvider, spawnConfig)) {
			mKeyMob = mob;
			return true;
		}
	}
	return false;
}

int SequentialClickPuzzle::indexOf(const AActor& actor) const {
	return algo::index_of_if(mObjects, RETLAMBDA(it.actor == &actor)).Get(-1);
}

void SequentialClickPuzzle::tryActivateObjectAtIndex(int index) const {
	if (!mObjects.IsValidIndex(index)) {
		return;
	}
	AActor* object = mObjects[index].actor.Get();
	if (!object) {
		return;
	}
	if (auto interactable = object->FindComponentByClass<UReplicatedInteractableComponent>()) {
		interactable->initActivatable();
	}
}

void SequentialClickPuzzle::onTick() {
	if (!mSpawnQueue || mSpawnRegions.isEmpty()) {
		return;
	}
	Util::removeNullsSwap(mMobs);

	const auto now = currentTimeSeconds();

	mSpawnQueue->tick(now);

	if (!actorquery::is::alive(mKeyMob.Get()) && !firstPickup) {
		firstPickup = true;
	}

	if (
		firstPickup &&
		mSpawnQueue->isEmpty() &&
		mMobs.Num() == 0
		) {
		float delay{ 0.f };
		for (auto group : mobspawn::calculateMobsWithEnchantments(mData.mobs->groups, 20, game().settings().difficultyStats)) {
			group.disenchant();
			auto locations = algo::generate::generate_n(group.count, RETLAMBDA(mSpawnRegions.getRandomPosFromRandomRegion()));
			mSpawnQueue->enqueue(now + delay, 1.f, group, group.type == EntityType::RedstoneGolem || group.type == EntityType::RedstoneMonstrosity, locations);
			delay += .1f;
		}
	}
}

void SequentialClickPuzzle::onStop() {
	if (mDoorLocker) {
		mDoorLocker->unlock();
	}
}

void SequentialClickPuzzle::onStart() {
	tryActivateObjectAtIndex(0);
}

FObjectiveLocations SequentialClickPuzzle::getLocations() const {
	const auto index = completion().current();
	if (!mObjects.IsValidIndex(index)) {
		return {};
	}
	TArray<FVector> locations;
	const auto& current = mObjects[index];
	const auto* component = current.interactable.Get();
	if (!current.clicked && current.actor.IsValid() && component && component->IsInteractionEnalbed()) {
		locations.Add(current.actor->GetActorLocation());
	}
	return locations;
}

void SequentialClickPuzzle::_onClickedObject(AActor& actor) {
	int numClicked = 0;

	for (auto&& object : mObjects) {
		if (object.actor == &actor) {
			if (!object.clicked) {
				_onClickedObjectOnce(actor);
				tryActivateObjectAtIndex(indexOf(actor) + 1);
			}
			object.clicked = true;
		}
		if (object.clicked) {
			numClicked++;
		}
	}
	markPartiallyCompleted(numClicked, mData.count);
}

void SequentialClickPuzzle::_onClickedObjectOnce(AActor& actor) {
	if (progressHandler()) {
		for (auto&& tag : actor.Tags) {
			progressHandler()->FinishedObjectiveTag(tag.ToString());
		}
	}
}

EntityType SequentialClickPuzzle::keyType() const {
	return mData.keyType.Get("") == "gold" ? EntityType::GoldBabyKey : EntityType::SilverBabyKey;
}

SequentialClickPuzzle::Positions SequentialClickPuzzle::generatePositions(size_t count, Random& rnd) const {
	if (mLocations.size() < count) {
		return {};
	}
	const auto numReal = std::min(count, mLocations.size());
	const auto regionIndices = Util::randomIndices(mLocations.size(), &rnd);

	Positions out;
	for (size_t i = 0; i < mLocations.size(); ++i) {
		const auto pos = centerFloor(mLocations[regionIndices[i]].area());
		if (i < numReal) {
			out.objects.Add(pos);
		}
		else {
			out.dummies.Add(pos);
		}
	}
	return out;
}

TOptional<std::vector<io::RegionLocator>> SequentialClickPuzzle::_getSpawnRegionLocators() const {
	// @note: The second part of this condition is an artificial limitation, to be fully
	//        backwards compatible. In the old code, no mobs spawned if mob-stretch referred
	//        to a stretch with number of tiles != 1.
	if (mData.spawnRegions) {
		return mData.spawnRegions;
	}
	if (mData.mobStretch && targetFinder().singleTile(targetlocators::allTilesInStretch(mData.mobStretch.GetValue()))) {
		return std::vector<std::string>{ targetlocators::allRegionsInStretch(mData.mobStretch.GetValue()) };
	}
	return {};
}

}}
