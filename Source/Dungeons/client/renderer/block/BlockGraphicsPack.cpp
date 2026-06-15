/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "Dungeons.h"

#include "BlockGraphicsPack.h"
#include "client/renderer/texture/TextureAtlas.h"
#include "world/Facing.h"
#include "world/level/biome/Biome.h"
#include "world/level/block/Block.h"
#include "world/level/block/BlockInclude.h"
#include "world/level/block/LeafBlock.h"
#include "world/level/BlockSource.h"
#include "world/phys/Vec3.h"
// #include "platform/AppPlatform.h"
#include "client/resource/Resource.h"
#include "util/EnumUtil.h"
#include "util/JSONReadVerifier.h"
#include <VoiceConfig.h>

#include "game/Game.h"
#include "util/StringUtil.h"

namespace internal {
	std::map<UWorld*, std::unique_ptr<BlockGraphicsManager>>& managers() {
		static std::map<UWorld*, std::unique_ptr<BlockGraphicsManager>> managers;
		return managers;
	}
}

namespace BlockGraphicsHelper {
	const BlockGraphics& getBlock(UWorld* world, BlockPos pos, BlockID id) {
		return getBlock(getBlockGraphicsPack(world, pos), id);
	}

	const BlockGraphics& getBlock(const BlockGraphicsPack& pack, BlockID id) {
		return *pack.mBlocks[id];
	}

	const BlockGraphicsPack& getBlockGraphicsPack(UWorld* world, BlockPos pos) {
		if (auto game = actorquery::getGame(world)) {
			if (auto tile = game->tiles().getClosestTile(pos)) {
				return tile->dungeon().blockGraphicsPack(world);
			}
		}
		return getInstance(world).getLatest();
	}

	void CreateInstance(UWorld* world) {
		internal::managers().emplace(world, make_unique<BlockGraphicsManager>());
	}

	void RemoveInstance(UWorld* world) {
		auto it = internal::managers().find(world);
		if (it != internal::managers().end()) {
			internal::managers().erase(it);
		}
	}

	BlockGraphicsManager& getInstance(UWorld* world) {
		check(world && "World can't be null!");
		return *internal::managers().at(world).get();
	}
}

BlockGraphicsPack::BlockGraphicsPack(const ResourcePack& resource, std::shared_ptr<TextureAtlas> textureAtlas) {
}

void BlockGraphicsPack::initBlocks(const ResourcePack& resource) {
	struct RequiredFilenamePair {
		std::string filename;
		bool required;
	};
	std::initializer_list<RequiredFilenamePair> filenames{
		{"blocks.json", true},
		{"blocks_overrides.json", false}
	};

#if WITH_EDITOR
	//D11.SC Running multiplayer PIE games can trigger resetting of the static list when other instances are still using it. Just do it when it changes resources
	FString BlocksRes = resource.getFullPathFor(ResourceLocation("blocks.json"));
	FString OverrideBlocksRes = resource.getFullPathFor(ResourceLocation("blocks_overrides.json"));

#endif

	mOwnedBlocks.clear();
	memset(mBlocks, 0, sizeof(mBlocks));//null all the blocks

	std::vector<Json::Value> json;
	for (auto& filename : filenames) {
		const auto val = resource.getJson(filename.filename);
		if (!val) {
			continue;
		}
		json.push_back(val.GetValue());
	}

	registerBlockGraphics(json, "air");
	registerBlockGraphics(json, "stone");
	registerBlockGraphics(json, "grass").setMapColor(Color::fromARGB(0x7FB238));
	registerBlockGraphics(json, "dirt").setMapColor(Color::fromARGB(0x976D4D));
	registerBlockGraphics(json, "cobblestone");
	registerBlockGraphics(json, "planks");
	registerBlockGraphics(json, "sapling");
	registerBlockGraphics(json, "bedrock");
	registerBlockGraphics(json, "flowing_water");
	registerBlockGraphics(json, "water");
	registerBlockGraphics(json, "flowing_lava");
	registerBlockGraphics(json, "lava");
	registerBlockGraphics(json, "sand");
	registerBlockGraphics(json, "gravel");
	registerBlockGraphics(json, "gold_ore");
	registerBlockGraphics(json, "iron_ore");
	registerBlockGraphics(json, "coal_ore");
	registerBlockGraphics(json, "log");
	registerBlockGraphics(json, "leaves");

#ifdef MCPE_EDU
	registerBlockGraphics(json, "allow");
	registerBlockGraphics(json, "deny");
	registerBlockGraphics(json, "border_block");
	registerBlockGraphics(json, "chalkboard");
	registerBlockGraphics(json, "camera");
#endif

	registerBlockGraphics(json, "sponge");
	registerBlockGraphics(json, "glass");
	registerBlockGraphics(json, "lapis_ore");
	registerBlockGraphics(json, "lapis_block").setMapColor(Color::fromARGB(0x4A80FF));

	registerBlockGraphics(json, "dispenser");

	registerBlockGraphics(json, "sandstone");
	registerBlockGraphics(json, "noteblock");
	registerBlockGraphics(json, "bed");
	registerBlockGraphics(json, "golden_rail");
	registerBlockGraphics(json, "detector_rail");
	registerBlockGraphics(json, "sticky_piston");
	registerBlockGraphics(json, "web");
	registerBlockGraphics(json, "tallgrass");
	registerBlockGraphics(json, "deadbush");
	registerBlockGraphics(json, "piston");
	registerBlockGraphics(json, "pistonArmCollision");

	registerBlockGraphics(json, "wool");

	registerBlockGraphics(json, "yellow_flower");
	registerBlockGraphics(json, "red_flower");
	registerBlockGraphics(json, "brown_mushroom");
	registerBlockGraphics(json, "red_mushroom");
	registerBlockGraphics(json, "gold_block").setMapColor(Color::fromARGB(0xFAEE4D));
	registerBlockGraphics(json, "iron_block").setMapColor(Color::fromARGB(0xA7A7A7));
	registerBlockGraphics(json, "double_stone_slab");
	registerBlockGraphics(json, "stone_slab");
	registerBlockGraphics(json, "brick_block").setMapColor(Color::fromARGB(0x993333));
	registerBlockGraphics(json, "tnt");
	registerBlockGraphics(json, "bookshelf");
	registerBlockGraphics(json, "mossy_cobblestone");
	registerBlockGraphics(json, "obsidian");
	registerBlockGraphics(json, "torch");

	registerBlockGraphics(json, "mob_spawner");

	registerBlockGraphics(json, "oak_stairs");
	registerBlockGraphics(json, "chest");
	registerBlockGraphics(json, "redstone_wire");

	registerBlockGraphics(json, "diamond_ore");
	registerBlockGraphics(json, "diamond_block").setMapColor(Color::fromARGB(0x5CDBD5));
	registerBlockGraphics(json, "crafting_table");
	registerBlockGraphics(json, "wheat");
	registerBlockGraphics(json, "farmland");
	registerBlockGraphics(json, "furnace");
	registerBlockGraphics(json, "lit_furnace");
	registerBlockGraphics(json, "standing_sign");
	registerBlockGraphics(json, "wooden_door");
	registerBlockGraphics(json, "ladder");
	registerBlockGraphics(json, "rail");
	registerBlockGraphics(json, "stone_stairs");
	registerBlockGraphics(json, "wall_sign");
	registerBlockGraphics(json, "lever");
	registerBlockGraphics(json, "stone_pressure_plate").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now

	registerBlockGraphics(json, "iron_door");

	registerBlockGraphics(json, "wooden_pressure_plate").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now

	registerBlockGraphics(json, "redstone_ore");
	registerBlockGraphics(json, "lit_redstone_ore");
	registerBlockGraphics(json, "unlit_redstone_torch");
	registerBlockGraphics(json, "redstone_torch");
	registerBlockGraphics(json, "stone_button").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now

	registerBlockGraphics(json, "snow_layer");
	registerBlockGraphics(json, "ice");
	registerBlockGraphics(json, "snow");
	registerBlockGraphics(json, "cactus");
	registerBlockGraphics(json, "clay");
	registerBlockGraphics(json, "reeds");
	registerBlockGraphics(json, "fence").setMapColor(WoodBlock::getWoodMaterialColor(WoodBlockType::Oak));
	registerBlockGraphics(json, "nether_brick_fence").setMapColor(Color::fromARGB(0x700200));

	registerBlockGraphics(json, "pumpkin");
	registerBlockGraphics(json, "netherrack");
	registerBlockGraphics(json, "soul_sand").setMapColor(Color::fromARGB(0x664C33));

	registerBlockGraphics(json, "glowstone");
	registerBlockGraphics(json, "portal");
	registerBlockGraphics(json, "lit_pumpkin");
	registerBlockGraphics(json, "cake").setVisualShape(Vec3(0.0f), Vec3(1.0f, 0.5f, 1.0f)); // Hack hard coded shape for now

	registerBlockGraphics(json, "unpowered_repeater");
	registerBlockGraphics(json, "powered_repeater");

	registerBlockGraphics(json, "invisibleBedrock");
	registerBlockGraphics(json, "trapdoor");

	registerBlockGraphics(json, "monster_egg");
	registerBlockGraphics(json, "stonebrick");
	registerBlockGraphics(json, "brown_mushroom_block").setMapColor(Color::fromARGB(0x976D4D));
	registerBlockGraphics(json, "red_mushroom_block").setMapColor(Color::fromARGB(0x993333));

	// This should be called fenceIron but the iron part is cut out of the crafting screens
	registerBlockGraphics(json, "iron_bars");
	registerBlockGraphics(json, "glass_pane");
	registerBlockGraphics(json, "melon_block").setMapColor(Color::fromARGB(0x7FCC19));
	registerBlockGraphics(json, "pumpkin_stem");
	registerBlockGraphics(json, "melon_stem");
	registerBlockGraphics(json, "vine");
	registerBlockGraphics(json, "fence_gate").setMapColor(WoodBlock::getWoodMaterialColor(WoodBlockType::Oak));
	registerBlockGraphics(json, "brick_stairs");
	registerBlockGraphics(json, "mycelium").setMapColor(Color::fromARGB(0x7F3FB2));
	registerBlockGraphics(json, "waterlily");

	registerBlockGraphics(json, "brewing_stand");
	registerBlockGraphics(json, "cauldron");

	registerBlockGraphics(json, "end_portal").setBlockShape(BlockShape::END_PORTAL);
	registerBlockGraphics(json, "end_portal_frame").setMapColor(Color::fromARGB(0x667F33));

	registerBlockGraphics(json, "end_bricks");
	registerBlockGraphics(json, "end_stone");
	registerBlockGraphics(json, "end_rod");
	registerBlockGraphics(json, "redstone_lamp");
	registerBlockGraphics(json, "lit_redstone_lamp");
	registerBlockGraphics(json, "cocoa");

	registerBlockGraphics(json, "emerald_ore");
	registerBlockGraphics(json, "emerald_block").setMapColor(Color::fromARGB(0x00D93A));

	registerBlockGraphics(json, "spruce_stairs");
	registerBlockGraphics(json, "birch_stairs");
	registerBlockGraphics(json, "jungle_stairs");

	registerBlockGraphics(json, "beacon");

	registerBlockGraphics(json, "wooden_button").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now

	registerBlockGraphics(json, "stone_brick_stairs");
	registerBlockGraphics(json, "nether_brick");
	registerBlockGraphics(json, "nether_brick_stairs");
	registerBlockGraphics(json, "nether_wart").setMapColor(Color::fromARGB(0x993333));
	registerBlockGraphics(json, "enchanting_table").setMapColor(Color::fromARGB(0x993333));

	// Switched places from PC Minecraft
	registerBlockGraphics(json, "dropper");
	registerBlockGraphics(json, "activator_rail");
	registerBlockGraphics(json, "sandstone_stairs");

	registerBlockGraphics(json, "tripwire_hook");
	registerBlockGraphics(json, "tripWire");

	registerBlockGraphics(json, "cobblestone_wall");
	registerBlockGraphics(json, "flower_pot");
	registerBlockGraphics(json, "carrots");
	registerBlockGraphics(json, "potatoes");

	registerBlockGraphics(json, "skull");
	registerBlockGraphics(json, "anvil");
	registerBlockGraphics(json, "trapped_chest");

	registerBlockGraphics(json, "light_weighted_pressure_plate").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now
	registerBlockGraphics(json, "heavy_weighted_pressure_plate").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now

	registerBlockGraphics(json, "unpowered_comparator");
	registerBlockGraphics(json, "powered_comparator");

	registerBlockGraphics(json, "daylight_detector").setVisualShape(Vec3(0.0f), Vec3(1.0f, 6.0f / 16.0f, 1.0f));
	registerBlockGraphics(json, "redstone_block").setMapColor(Color::fromARGB(0xff0000));
	registerBlockGraphics(json, "quartz_ore").setMapColor(Color::fromARGB(0x700200));

	registerBlockGraphics(json, "hopper");

	registerBlockGraphics(json, "quartz_block");
	registerBlockGraphics(json, "quartz_stairs");

	registerBlockGraphics(json, "purpur_block");
	registerBlockGraphics(json, "purpur_stairs");

	registerBlockGraphics(json, "chorus_plant");
	registerBlockGraphics(json, "chorus_flower");

	registerBlockGraphics(json, "double_wooden_slab");
	registerBlockGraphics(json, "wooden_slab");

	registerBlockGraphics(json, "double_stonefloor_slab");
	registerBlockGraphics(json, "stonefloor_slab");

	registerBlockGraphics(json, "stained_hardened_clay");

	registerBlockGraphics(json, "leaves2");
	registerBlockGraphics(json, "log2");

	registerBlockGraphics(json, "acacia_stairs");
	registerBlockGraphics(json, "dark_oak_stairs");
	registerBlockGraphics(json, "slime");

	registerBlockGraphics(json, "iron_trapdoor");

	registerBlockGraphics(json, "hay_block").setMapColor(Color::fromARGB(0xE5E533));
	registerBlockGraphics(json, "carpet").setVisualShape(Vec3(0.4), Vec3(0.6)); //HACK hardcode a small shape for now;

	registerBlockGraphics(json, "hardened_clay");
	registerBlockGraphics(json, "coal_block").setMapColor(Color::fromARGB(0x191919));
	registerBlockGraphics(json, "packed_ice");
	registerBlockGraphics(json, "double_plant").setBlockShape(BlockShape::DOUBLE_PLANT);

	registerBlockGraphics(json, "daylight_detector_inverted").setVisualShape(Vec3(0.0f), Vec3(1.0f, 6.0f / 16.0f, 1.0f));
	registerBlockGraphics(json, "red_sandstone");
	registerBlockGraphics(json, "red_sandstone_stairs");
	registerBlockGraphics(json, "double_stone_slab2");
	registerBlockGraphics(json, "stone_slab2");

	registerBlockGraphics(json, "spruce_fence_gate");
	registerBlockGraphics(json, "birch_fence_gate");
	registerBlockGraphics(json, "jungle_fence_gate");
	registerBlockGraphics(json, "dark_oak_fence_gate");
	registerBlockGraphics(json, "acacia_fence_gate");

	registerBlockGraphics(json, "spruce_door");
	registerBlockGraphics(json, "birch_door");
	registerBlockGraphics(json, "jungle_door");
	registerBlockGraphics(json, "acacia_door");
	registerBlockGraphics(json, "dark_oak_door");

	registerBlockGraphics(json, "dragon_egg");

	registerBlockGraphics(json, "grass_path");

	registerBlockGraphics(json, "frame");
	registerBlockGraphics(json, "end_gateway").setBlockShape(BlockShape::END_GATEWAY);

	registerBlockGraphics(json, "prismarine");
	registerBlockGraphics(json, "seaLantern");

	//registerBlockGraphics(json, "structure_block");
	//registerBlockGraphics(json, "structure_void");

	//
	// Special blocks for Pocket Edition is placed at high IDs
	//

	registerBlockGraphics(json, "void_block");

	for (int i : range(16)) {
		registerBlockGraphics(json, std::string("custom_") + std::to_string(i));
	}
	registerBlockGraphics(json, "stone_gradient");
	registerBlockGraphics(json, "podzol").setMapColor(Color::fromARGB(0x976D4D));
	
	registerBlockGraphics(json, "beetroot");
	registerBlockGraphics(json, "stonecutter");
	registerBlockGraphics(json, "glowingobsidian");
	registerBlockGraphics(json, "netherreactor");
	registerBlockGraphics(json, "info_update");
	registerBlockGraphics(json, "info_update2");
	registerBlockGraphics(json, "movingBlock");
	registerBlockGraphics(json, "observer");

	registerBlockGraphics(json, "stained_glass");
	registerBlockGraphics(json, "stained_glass_pane");

	registerBlockGraphics(json, "reserved6"); //BG_todo: not used anywhere

	for (int i : range(0, 119)) {
		registerBlockGraphics(json, std::string("element_") + std::to_string(i));
	}

	registerBlockGraphics(json, "smooth_stone");
	registerBlockGraphics(json, "warped_nylium");
	registerBlockGraphics(json, "crimson_nylium");
	registerBlockGraphics(json, "stripped_oak_log");
	registerBlockGraphics(json, "stripped_dark_oak_log");
	registerBlockGraphics(json, "stripped_acacia_log");
	registerBlockGraphics(json, "stripped_jungle_log");
	registerBlockGraphics(json, "stripped_birch_log");
	registerBlockGraphics(json, "stripped_spruce_log");
	registerBlockGraphics(json, "ancient_debris");
	registerBlockGraphics(json, "target");
	registerBlockGraphics(json, "polished_basalt");
	registerBlockGraphics(json, "lodestone");
	registerBlockGraphics(json, "blackstone");
	registerBlockGraphics(json, "basalt");
	registerBlockGraphics(json, "bone_block");
	registerBlockGraphics(json, "cracked_polished_blackstone_bricks");
	registerBlockGraphics(json, "crying_obsidian");
	registerBlockGraphics(json, "chiseled_nether_bricks");
	registerBlockGraphics(json, "quartz_bricks");
	registerBlockGraphics(json, "cracked_nether_bricks");
	registerBlockGraphics(json, "gilded_blackstone");
	registerBlockGraphics(json, "chiseled_polished_blackstone");
	registerBlockGraphics(json, "netherite_block");
	registerBlockGraphics(json, "nether_gold_ore");
	registerBlockGraphics(json, "polished_blackstone_bricks");
	registerBlockGraphics(json, "polished_blackstone");
	/*
	registerBlockGraphics(json, "glazed_terracotta_black");
	registerBlockGraphics(json, "glazed_terracotta_red");
	registerBlockGraphics(json, "glazed_terracotta_green");
	registerBlockGraphics(json, "glazed_terracotta_brown");
	registerBlockGraphics(json, "glazed_terracotta_blue");
	registerBlockGraphics(json, "glazed_terracotta_cyan");
	registerBlockGraphics(json, "glazed_terracotta_silver");
	registerBlockGraphics(json, "glazed_terracotta_gray");
	registerBlockGraphics(json, "glazed_terracotta_pink");
	registerBlockGraphics(json, "glazed_terracotta_lime");
	registerBlockGraphics(json, "glazed_terracotta_yellow");
	registerBlockGraphics(json, "glazed_terracotta_light_blue");
	*/
	registerBlockGraphics(json, "glazed_terracotta_magenta");
	registerBlockGraphics(json, "glazed_terracotta_orange");
	registerBlockGraphics(json, "glazed_terracotta_white");
	registerBlockGraphics(json, "glazed_terracotta_purple");
	registerBlockGraphics(json, "purpur_block");
	registerBlockGraphics(json, "red_nether_brick");
	registerBlockGraphics(json, "end_bricks");
	registerBlockGraphics(json, "crimson_slab");
	registerBlockGraphics(json, "warped_slab");
	registerBlockGraphics(json, "crimson_double_slab");
	registerBlockGraphics(json, "warped_double_slab");


	//
	// Stuff that need to be initialized in a specific order (i.e. after the other blocks have been created)
	//
	registerBlockGraphics(json, "fire");

	//add missing blocks
	//TODO start at 0, add a air block!
	for (int i = 1; i < NUM_BLOCK_TYPES; i++) {
        bool missingBlock = false;
        if(mBlocks[i] == nullptr) {
            missingBlock = true;
            // NOTE (venvious): Using [i] as the name because having an empty name is not allowed
            // (Also having the same name is not allowed either)
            registerBlockGraphics(json, Util::toString(i));
        }
        else {
            for(int j=0; j<6 && !missingBlock; ++j) {
                if(mBlocks[i]->mUVItem[j].getName().empty()) {
                    missingBlock = true;
                }
            }
        }
		if (missingBlock) {
			mBlocks[i]->setTextureItem("missing_tile");
		}
	}
}

void BlockGraphicsPack::teardownBlocks() {

#if !WITH_EDITOR
	//D11.SC Running multiplayer PIE games can trigger resetting of the static list when other instances are still using it. 
	for (int i = 0; i < 256; ++i) {
		mBlocks[i] = nullptr;
	}

	mOwnedBlocks.clear();
	mBlockLookupMap.clear();
#endif
	
}

void BlockGraphicsPack::setTextureAtlas(std::shared_ptr<TextureAtlas> terrainAtlas) {
	terrainTextureAtlas = terrainAtlas;
	mBlockUVMapping.generateUV(*terrainAtlas);
}

const BlockGraphics& BlockGraphicsPack::get(BlockID id) const {
	return *mBlocks[id];
}

TextureUVCoordinateSet BlockGraphicsPack::getTextureUVCoordinateSet(const std::string& name, int id) {
	return terrainTextureAtlas->getTextureItem(name)[id];
}

const TextureAtlasItem& BlockGraphicsPack::getTextureItem(const std::string& name) const {
	return terrainTextureAtlas->getTextureItem(name);
}

bool BlockGraphicsPack::shouldRenderFace(BlockSource& region, const BlockPos& pos, FacingID face, const AABB& shape) const {
	// Common
	if ((face == Facing::DOWN && shape.min.y > 0) ||
		(face == Facing::UP && shape.max.y < 1) ||
		(face == Facing::NORTH && shape.min.z > 0) ||
		(face == Facing::SOUTH && shape.max.z < 1) ||
		(face == Facing::WEST && shape.min.x > 0) ||
		(face == Facing::EAST && shape.max.x < 1)) {
		return true;
	}

	const auto& fullBlockData = region.getBlockAndData(pos);
	BlockGraphics* bg = mBlocks[fullBlockData.id];
	const Block* block = Block::mBlocks[fullBlockData.id];

	if (bg == nullptr || block == nullptr) {
		return true;
	}

	if (block == Block::mInvisibleBedrock) {	//this hack is needed because unbreakable is not solid but we don't want it to render
		return false;	//we should really have two different flags for "logically solid" and "fully opaque", leaves are not solid pls
	}

	//HACK this code most definitely doesn't belong here
	if (block->hasProperty(BlockProperty::Leaf)) {
		return !LeafBlock::isDeepLeafBlock(region, pos);
	}

	return (face == Facing::UP && (block == Block::mTopSnow || block == Block::mWoolCarpet)) ? false : !block->isSolid();
}

BlockGraphics& BlockGraphicsPack::registerBlockGraphics(const std::vector<Json::Value>& cascadedBlockDataJsons, const std::string& blockName) {
	auto block = make_unique<BlockGraphics>(*this, blockName);

	auto actualID = block->mID;
	ensureMsgf(actualID >= 0 && actualID <= 0xffff, TEXT("Invalid ID value"));

	auto ptr = block.get();
	ptr->setMaterialType(static_cast<EMaterialTypeEnum>(Block::mBlocks[actualID]->getMaterial().getType()));

	mOwnedBlocks.emplace_back(std::move(block));
	mBlocks[actualID] = ptr;

	for (auto& json : cascadedBlockDataJsons) {
		const Json::Value& blockData = json[blockName];

		if (false == ptr->setTextures(blockData["textures"])) {
			UE_LOG(LogDungeons, Log, TEXT("Loading block data from Json :  \"%s\" : fail to set textures"), *stringutil::toFString(blockName));
		}
		if (false == ptr->setCarriedTextures(blockData["carried_textures"])) {
			UE_LOG(LogDungeons, Log, TEXT("Loading block data from Json :  \"%s\" : fail to set carried textures"), *stringutil::toFString(blockName));
		}
		if (false == ptr->setTextureIsotropic(blockData["isotropic"])) {
			UE_LOG(LogDungeons, Log, TEXT("Loading block data from Json :  \"%s\" : Error in setting texture isotropic"), *stringutil::toFString(blockName));
		}
		if (false == ptr->setBlockShape(blockData["blockshape"])) {
			UE_LOG(LogDungeons, Log, TEXT("Loading block data from Json :  \"%s\" : fail to set block shape"), *stringutil::toFString(blockName));
		}
		if (false == ptr->setBrightnessGamma(blockData["brightness_gamma"])) {
			UE_LOG(LogDungeons, Log, TEXT("Loading block data from Json :  \"%s\" : fail to set brightness gamma"), *stringutil::toFString(blockName));
		}
		if (false == ptr->setSoundType(blockData["sound"])) {
			UE_LOG(LogDungeons, Log, TEXT("Loading block data from Json :  \"%s\" : fail to set sounds"), *stringutil::toFString(blockName));
		}
		if (false == ptr->setMaterialType(blockData["material"])) {
			UE_LOG(LogDungeons, Log, TEXT("Loading block data from Json :  \"%s\" : fail to set material"), *stringutil::toFString(blockName));
		}
	}
	return *ptr;
}


void BlockGraphicsPack::reloadBlockUVs(TextureAtlas& changed) {
	if (&changed != terrainTextureAtlas.get()) {
		return; //not us
	}

	//reload all block UVs
	for (auto&& block : mBlocks) {
		if (block) {
			for (auto& facing : Facing::DIRECTIONS) {
				auto i = enum_cast(facing);

				if (block->mUVItemName[i].size() > 0) {
					block->mUVItem[i] = getTextureItem(block->mUVItemName[i]);
				}

				if (block->mUVCarriedName[i].size() > 0) {
					block->mUVCarried[i] = getTextureItem(block->mUVCarriedName[i]);
				}
			}
		}
	}
}

UMaterialInstanceDynamic* BlockGraphicsPack::createTextureAtlasDynamicMaterialInstance(UMaterialInterface* material, AActor* parentActor) const {
	auto* materialInstance = UMaterialInstanceDynamic::Create(material, parentActor);
	materialInstance->SetTextureParameterValue(FName("T2DParam"), terrainTextureAtlas->atlasTexture);
	materialInstance->SetTextureParameterValue(FName("T2DParamEmissive"), terrainTextureAtlas->atlasTextureEmissive);
	return materialInstance;
}

void BlockGraphicsPack::CreateMaterials(AActor* parentActor, UMaterialInterface* opaque, UMaterialInterface* alphaTest, UMaterialInterface* alphaTestFoliage, UMaterialInterface* leaves) {
	atlasMaterialInstanceOpaque = TStrongObjectPtr<UMaterialInstanceDynamic>(createTextureAtlasDynamicMaterialInstance(opaque, parentActor));
	atlasMaterialInstanceAlpha = TStrongObjectPtr<UMaterialInstanceDynamic>(createTextureAtlasDynamicMaterialInstance(alphaTest, parentActor));
	atlasMaterialInstanceAlphaFoilage = TStrongObjectPtr<UMaterialInstanceDynamic>(createTextureAtlasDynamicMaterialInstance(alphaTestFoliage, parentActor));
	atlasMaterialInstanceLeaves = TStrongObjectPtr<UMaterialInstanceDynamic>(createTextureAtlasDynamicMaterialInstance(leaves, parentActor));
}

UMaterialInstanceDynamic* BlockGraphicsPack::GenerateLeafMaterial(UMaterialInterface* leaves, AActor* parentActor) {
	atlasMaterialInstanceLeaves = TStrongObjectPtr<UMaterialInstanceDynamic>(createTextureAtlasDynamicMaterialInstance(leaves, parentActor));
	return atlasMaterialInstanceLeaves.Get();
}

const BlockUVMapping& BlockGraphicsPack::getBlockUVMapping() const {
	return mBlockUVMapping;
}

BlockGraphicsManager::~BlockGraphicsManager() {
	teardown();
}

BlockGraphicsPack& BlockGraphicsManager::add(const ResourcePack& resource) {
	std::shared_ptr<TextureAtlas> textureAtlas = make_shared<TextureAtlas>();
	auto it = mBlockGraphics.emplace(resource.getBasePath(), make_unique<BlockGraphicsPack>(resource, textureAtlas));
	auto& pack = *it.first->second;
	textureAtlas->loadMetaFile(resource);
#if PLATFORM_WINDOWS
	textureAtlas->redrawAtlas();
#else
	textureAtlas->redrawAtlasD11();
#endif
	pack.setTextureAtlas(textureAtlas);
	textureAtlas->cleanTextureGroups();
	pack.initBlocks(resource);
	
	mSelected = it.first->second.get();
	return pack;
}

BlockGraphicsPack& BlockGraphicsManager::get(const ResourcePack& resource) const {
	return *mBlockGraphics.at(resource.getBasePath());
}

BlockGraphicsPack& BlockGraphicsManager::getLatest() const {
	return *mSelected;
}


void BlockGraphicsManager::teardown() {
	for (auto& pack : mBlockGraphics) {
		pack.second->teardownBlocks();
	}
	mBlockGraphics.clear();
	mSelected = nullptr;
}
