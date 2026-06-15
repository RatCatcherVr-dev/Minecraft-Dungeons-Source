#pragma once

#include "legacy/Core/Resource/ResourceHelper.h"
#include <string>
#include "Json/json.h"
#include "common_header.h"

UENUM(BlueprintType)
enum class EResourcePack: uint8 {
	Vanilla,
	Test,
	//
	// WITHERMORE
	//
	SquidCoast,
	CreeperWoods,
	ForestTemple,
	PumpkinPastures,
	AndesiteHills,
	SoggySwamp,
	//
	// HOWLING PEAKS
	//
	MooncoreCaverns,
	RedstoneMines,
	FieryForge,
	//
	// BLAZELANDS
	//
	ScorchedCrags,
	SandstoneWastes,
	DesertTemple,
	EvergreenOasis,
	//
	// HIGHBLOCK KEEP
	//
	SlimySewers,
	HighblockHalls,
	ObsidianPinnacle,
	CactiCanyon,
	Jungle,
	Mycelia,
	//
	// DOUBLE ELEVEN
	//
	DingyJungle,
	OvergrownTemple,
	BambooBluff,
	FrozenFjord,
	LonelyFortress,
	LostSettlement,

	//
	// Nether
	//
	Nether,
	warpedforest,
	BasaltDeltas,

	// DOUBLE ELEVEN NEXT
	//
	WindsweptPeaks,
	GaleSanctum,
	EndlessRampart,
	
	//
	// DOUBLE ELEVEN - NEW DLC
	//

	CoralRise,
	AbyssalMonument,
	RadiantRavine,

	//
	// DOUBLE ELEVEN END
	//
	TheStronghold,
	EnderWilds,
	BlightedCitadel
};
ENUM_NAME(EResourcePack);

class ResourcePack {
public:
	using Blob = TArray<uint8>;

	static const ResourcePack DEFAULT;
	ResourcePack(const FString& resourcePackFolder);
	ResourcePack(const EResourcePack&);

	TOptional<Json::Value> getJson(std::string) const;
	FString getFullPathFor(const ResourceLocation&) const;
	const FString& getBasePath() const;
private:
	FString BasePath;
};

