#include "Dungeons.h"
#include "LevelGenUtil.h"
#include "LevelValidation.h"
#include "SourceDataFilter.h"
#include "game/LevelSettings.h"
#include "game/mission/MissionDefs.h"
#include "game/mission/hyper/HyperMissions.h"
#include "lovika/world/level/levelgen/generator/alpha/AlphaGenerator.h"
#include "lovika/world/level/levelgen/hyper/HyperStitcher.h"
#include "util/UELogHelpers.h"

namespace levelgen {

//
// SourceData
//
TOptional<LevelGenData> sourceDataToGenData(Unique<sourcedata::SourceData> sourceData, const FString& levelFilename, const TArray<FString>& unlockKeys, generator::GeneratorFunc generator, FOutputDevice* output) {
	if (!sourceData) {
		UE_LOG_OUTPUT(output, LogLevelGeneration, ELogVerbosity::Error, TEXT("Can not read source levelfile or objectgroup: %s"), *levelFilename);
		return {};
	}
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_UEngine_AGameBP_GenerateLevel_filterByUnlockKeys);
		sourcedata::filterByUnlockKeys(*sourceData, unlockKeys);
	}
	const auto levelValidation = validate(*sourceData);
	const TCHAR* validationHeaderSuffix = levelValidation.isSuccess() ? TEXT("") : TEXT(" failed");

	UE_LOG_OUTPUT(output, LogLevelGeneration, getDefaultLogVerbosityFor(levelValidation), TEXT("Level validation for %s %s: %s"),
		UTF8_TO_TCHAR(sourceData->level.id.c_str()),
		validationHeaderSuffix,
		UTF8_TO_TCHAR(levelValidation.summary().c_str())
	);

	if (!levelValidation.isSuccess()) {
		return {};
	}
	return LevelGenData{ std::move(*sourceData), std::move(generator) };
}

Unique<sourcedata::SourceData> loadStitchedSourceDataFromFileSystem(const FLevelSettings& settings) {
	if (const auto hyperLevel = sourcedata::hyperLevelFromFileSystem(settings.getLevelFilename())) {
		const auto numSubMissions = missions::hajper::getAncientHuntNumSubMissions();
		const auto input = hajper::generateRandomStitchInput(numSubMissions, settings.getSeed(), *hyperLevel, settings.missionState.partsDiscovered);
		return hajper::stitch(input).data;
	}
	return {};
}

//
// LevelGenData and helpers
//
LevelGenData::LevelGenData(sourcedata::SourceData sourceData, generator::GeneratorFunc generatorFunc)
	: sourceData(std::move(sourceData))
	, generatorFunc(std::move(generatorFunc)) {
}

Runner& LevelGenData::runner() const& {
	if (!mRunner) {
		mRunner = std::make_unique<Runner>(sourceData.tiles, sourceData.props, sourceData.level, generatorFunc);
	}
	return *mRunner;
}

TOptional<LevelGenData> createLevelGenData(const FLevelSettings& settings, FOutputDevice* output) {
	auto sourceData = missions::get(settings.getLevelName()).isHyperMission() ?
		loadStitchedSourceDataFromFileSystem(settings) :
		sourcedata::fromFileSystem(settings.getLevelFilename());

	return createLevelGenData(std::move(sourceData), settings, output);
}

TOptional<LevelGenData> createLevelGenData(Unique<sourcedata::SourceData> sourceData, const FLevelSettings& settings, FOutputDevice* output) {
	if (!sourceData) {
		return {};
	}
	return sourceDataToGenData(
		std::move(sourceData),
		settings.getLevelFilename(),
		settings.getProgressionKeys(),
		missions::get(settings.getLevelName()).isHyperMission()?
			generator::alpha::HyperGenerator(*sourcedata::hyperLevelFromFileSystem(settings.getLevelFilename()), settings) :
			generator::alpha::DefaultGenerator(),
		output);
}

}
