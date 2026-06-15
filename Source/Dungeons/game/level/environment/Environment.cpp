#include "Dungeons.h"
#include "Environment.h"
#include "Assets/AmbienceFinder.h"
#include "game/level/GameTiles.h"
#include "game/level/TileDecor.h"
#include "game/level/ambience/AmbienceActor.h"
#include "game/level/sound/AudioManager.h"
#include "game/util/ActorQuery.h"
#include "lovika/world/level/levelgen/LevelDef.h"
#include "util/Algo.hpp"
#include "util/StringUtil.h"
#include <Classes/Engine/PostProcessVolume.h>
#include <Components/SkyLightComponent.h>
#include <Engine/AssetManager.h>

namespace game {
	
Environment::Environment(UWorld* world, const game::Tiles& tiles, const levelgen::LevelDef& levelDef)
	: mHasLoadedMainActors(false)
	, mEnvironmentalMainActorsLoading(0)
	, mEnvironmentalEffectActorsLoading(0)
{
	beginLoadEnvironmentalEffects(world, tiles, stringutil::toFString(levelDef.id));
}

Environment::~Environment() {
	for (TSharedPtr<FStreamableHandle>& streamHandle : mAsyncTasks) {
		streamHandle->CancelHandle();
	}
}

void Environment::dimLights(float intensityFraction, FNightColor NightModeColor) {

	for (auto& directional : directionalLights) {
		directional.dim(intensityFraction, NightModeColor);
	}

	for (auto& skylight : skyLights) {
		skylight.dim(intensityFraction, NightModeColor);
	}

	for (auto& light : lights) {
		light.dim(intensityFraction, NightModeColor);
	}

	for (auto& lightFlicker : LightFlickeringComponents) {
		if(lightFlicker.IsValid())
			lightFlicker->SetIntensityFraction(intensityFraction);
	}
}

void Environment::add(const TArray<AActor*>& decorTiles) {
	for (auto& actor : decorTiles) {
		if (!actor) {
			continue;
		}
		
		for (auto& component : actor->GetComponentsByClass(ULightComponent::StaticClass())) {
			lights.Add({ Cast<ULightComponent>(component) });
		}

		for (auto* component : actor->GetComponentsByClass(UFlickeringLightComponent::StaticClass())) {
			LightFlickeringComponents.Add(Cast<UFlickeringLightComponent>(component));
		}

	}
}

bool Environment::completedLoadEnvironmentalEffects()
{
	return mHasLoadedMainActors && (mEnvironmentalMainActorsLoading == 0 && mEnvironmentalEffectActorsLoading == 0);
}

void makeFileNameClassName(FString& Filename) {
	const int32 index = Filename.Find("/", ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	if (index != INDEX_NONE) {
		FString Classname = Filename.RightChop(index + 1);
		Filename = Filename + "." + Classname + "_C";
	}
}

FString classNameForEnvironmental(const FString& group, const FString& actor) {
	FString name = decor::filenameForEnvironmental(group, actor);
	makeFileNameClassName(name);
	return name;
}

AActor* spawnActorFromClass(UWorld* world, UClass* cls) {
	auto actor = world->SpawnActorDeferred<AActor>(cls, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	UGameplayStatics::FinishSpawningActor(actor, FTransform::Identity);
	if (actor) {
		auto origin = decor::getTileOriginInDecorSpace(*actor).Get(FTransform::Identity);
		origin.SetLocation(origin.GetLocation() * -1); // @flipped the coordinates on Gavelli's request
		actor->SetActorTransform(origin);
	}
	return actor;
}

void Environment::asyncSpawnEffectActors(const TArray<FString>& AttachedNames, FString group, UWorld* world, AAmbienceActor* pAmbienceActor)
{
	//work out attached effect actor classes to stream
	TArray<FSoftObjectPath> SrcNames = algo::map_tarray(AttachedNames, RETLAMBDA(FSoftObjectPath(classNameForEnvironmental(group, it))));

	mEnvironmentalEffectActorsLoading += SrcNames.Num();

	//Push attached Affect actors to be async loaded and spawned
	RequestAsyncLoadWrapper(SrcNames, [&, SrcNames, world, pAmbienceActor]()
	{
		for (const auto& name : SrcNames)
		{
			if (auto object = Cast<UClass>(name.ResolveObject()))
			{
				object->AddToRoot();

				//spawn effect actor
				if (auto effectActor = spawnActorFromClass(world, object))
				{
					addEffectActor(effectActor);
					if (pAmbienceActor)
						effectActor->AttachToActor(pAmbienceActor, FAttachmentTransformRules::KeepWorldTransform);
				}
			}
		}

		if (pAmbienceActor)
			pAmbienceActor->forceClearActiveAndHideAll();

		mEnvironmentalEffectActorsLoading -= SrcNames.Num();

	});

}


void Environment::asyncLoadEnvironmentalAmbienceActor(UWorld* world, const TArray<FSoftObjectPath>& paths) {
	const bool requestIsHandled = RequestAsyncLoadWrapper(paths, [=]() {
		const auto* finder = IDungeonsModule::Get().GetAmbienceFinder();
		
		for (const auto& name : paths) {
			if (UClass* pActorClass = Cast<UClass>(name.ResolveObject())) {
				pActorClass->AddToRoot();
				
			//loaded the class, spawn the actor
				if (AAmbienceActor* pAmbienceActor = Cast<AAmbienceActor>(spawnActorFromClass(world, pActorClass))) {
					FString group = finder->GroupForPath(name.GetAssetPathString());
					pAmbienceActor->SetGroupName(group);

					const TArray<FString>& AttachedNames = pAmbienceActor->GetAttachedNames();

					if (AttachedNames.Num()) {
						asyncSpawnEffectActors(AttachedNames, group, world, pAmbienceActor);
					}
					else {
						pAmbienceActor->forceClearActiveAndHideAll();
					}
				}
			}
		}
		mHasLoadedMainActors = true;
	});
	if (!requestIsHandled) {
		mHasLoadedMainActors = true;
	}
}



void Environment::beginLoadEnvironmentalEffects(UWorld* world, const game::Tiles& tiles, const FString& mainGroup) {
	const auto groups = [&] {
		const auto levelIds = algo::map_vector(tiles.getTiles(), RETLAMBDA(it->dungeon().def().level.id));
		return algo::unique_by_less_of(levelIds, RETLAMBDA(it));
	}();

	TSet<FSoftObjectPath> ambiencePaths;

	const auto* finder = IDungeonsModule::Get().GetAmbienceFinder();

	for (const auto& group : groups) {
		ambiencePaths.Append(finder->GetAmbiencesForGroup(group));
	}
	for (auto* tile : tiles.getTiles()) {
		const auto& ambienceGroupName = tile->ambienceGroupName();

		if (ambienceGroupName.Equals(tile->dungeon().def().level.id, ESearchCase::IgnoreCase)) {
			continue;
		}

		if (tile->ambience()) {
			if (const auto path = finder->GetVisualAmbienceForGroupWithId(ambienceGroupName, tile->ambience().GetValue())) {
				ambiencePaths.Add(path.GetValue());
			}
		}
		if (tile->ambienceAudio()) {
			if (const auto path = finder->GetAudioAmbienceForGroupWithId(ambienceGroupName, tile->ambienceAudio().GetValue())) {
				ambiencePaths.Add(path.GetValue());
			}
		}
	}

	asyncLoadEnvironmentalAmbienceActor(world, ambiencePaths.Array());

	//Legacy support
	TArray<FString> LegacyNames{ "directional_0" ,"skylight_0" , "fog_0", "postprocess_0" };
	asyncSpawnEffectActors(LegacyNames, mainGroup, world, nullptr);

	if (!actorquery::getFirstActor<AAudioManager>(world)) {
		FSoftObjectPath SoftPath = classNameForEnvironmental(mainGroup, "bpaudiomanager_0");
		++mEnvironmentalMainActorsLoading;

		//Stream the bpaudiomanager_0 actor class in and instance it
		RequestAsyncLoadWrapper({ SoftPath },
			[&, world, SoftPath]() {

			if (UClass* pActorClass = Cast<UClass>(SoftPath.ResolveObject()))
			{
				pActorClass->AddToRoot();

				//loaded the class, spawn the actor
				spawnActorFromClass(world, pActorClass);
			}
			--mEnvironmentalMainActorsLoading;
		});
	}
}

void Environment::addEffectActor(AActor* pActor)
{
	if (pActor )
	{
		if (const auto skylightComponent = pActor->FindComponentByClass<USkyLightComponent>()) {
			skyLights.Add({ skylightComponent });
		}
		else if (const auto lightComponent = pActor->FindComponentByClass<ULightComponent>()) {
			directionalLights.Add({ lightComponent });
		}
		else if (pActor->IsA<APostProcessVolume>()) {
			postprocess.Add(pActor);
		}
	}
}

bool Environment::RequestAsyncLoadWrapper(const TArray<FSoftObjectPath>& TargetsToStream, TFunction<void()>&& Callback) {
	// Wrapping the callback so the callback can remove its own handle when it's done
	TSharedPtr<TSharedPtr<FStreamableHandle>> futureStreamHandle = MakeShared<TSharedPtr<FStreamableHandle>>();
	auto callbackWrapper = [futureStreamHandle, Callback = std::move(Callback), this] {
		Callback();

		int removedItems = mAsyncTasks.Remove(*futureStreamHandle);
		check(removedItems == 1);
	};

	auto streamHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(TargetsToStream, std::move(callbackWrapper), FStreamableManager::AsyncLoadHighPriority);
	mAsyncTasks.Add(streamHandle);

	*futureStreamHandle = streamHandle;
	return static_cast<bool>(streamHandle);
}

}
