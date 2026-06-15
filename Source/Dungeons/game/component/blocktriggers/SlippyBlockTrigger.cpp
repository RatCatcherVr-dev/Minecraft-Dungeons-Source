#include "Dungeons.h"
#include "SlippyBlockTrigger.h"
#include "CommonTypes.h"
#include "world/level/block/Block.h"
#include "world/level/block/ColoredBlock.h"
#include "world/level/block/WoodBlock.h"
#include "world/level/block/StoneSlabBlock.h"
#include "StatTracker.h"
#include "client/renderer/block/BlockGraphics.h"
#include "game/Conversion.h"

void USlippyBlockTrigger::OnEnterStep(EMaterialTypeEnum& stepMaterial, ABaseCharacter* const character) const
{
	stepMaterial = EMaterialTypeEnum::SlippyFloor;
	character->OnModifierSurfaceChanged.Broadcast(EMaterialTypeEnum::SlippyFloor);

	// D11.SSN
	if (auto* playerCharacter = Cast<APlayerCharacter>(character)) {
		if (auto* tracker = playerCharacter->GetStatTracker()) {
			tracker->IceSliding(true);
		}
	}
}

void USlippyBlockTrigger::OnExitStep(EMaterialTypeEnum& stepMaterial, ABaseCharacter* const character) const
{
	character->OnModifierSurfaceChanged.Broadcast(EMaterialTypeEnum::Any);

	// D11.SSN
	if (auto* playerCharacter = Cast<APlayerCharacter>(character)) {
		if (auto* tracker = playerCharacter->GetStatTracker()) {
			tracker->IceSliding(false);
		}
	}
}

bool UFrozenFjordSlippyBlockTrigger::IsTrigger(const FullBlock & fullBlock, ABaseCharacter * const character) const
{
	const auto& block = fullBlock.getBlock();
	if (block.getId() == Block::mIce->getId() ||
		block.getId() == Block::mPackedIce->getId() ||
		block.getId() == Block::mEmeraldBlock->getId() ||
		block.getId() == Block::mIronBlock->getId() ||
		block.getId() == Block::mCustom0->getId()) {
		return true;
	}
	else if (block.getId() == Block::mStainedClay->getId()) {
		ColoredBlockType mappedColor = block.getBlockState(BlockState::MappedType).get<ColoredBlockType>(fullBlock.data);
		switch (mappedColor) {
		case ColoredBlockType::Purple:
		case ColoredBlockType::Brown:
		case ColoredBlockType::Green:
			return true;
		default:
			break;
		}
	}
	return false;
}

bool ULonelyFortressSlippyBlockTrigger::IsTrigger(const FullBlock & fullBlock, ABaseCharacter * const character) const
{
	const auto& block = fullBlock.getBlock();
	if (block.getId() == Block::mSoulSand->getId() ||
		block.getId() == Block::mQuartzOre->getId() ||
		block.getId() == Block::mIce->getId() ||
		block.getId() == Block::mPackedIce->getId() ||
		block.getId() == Block::mCustom0->getId()) {
		return true;
	}
	else if (block.getId() == Block::mStainedClay->getId()) {
		ColoredBlockType mappedColor = block.getBlockState(BlockState::MappedType).get<ColoredBlockType>(fullBlock.data);
		switch (mappedColor) {
		case ColoredBlockType::Red:
			return true;
		default:
			break;
		}
	}
	return false;
}

bool ULostSettlementSlippyBlockTrigger::IsTrigger(const FullBlock & fullBlock, ABaseCharacter * const character) const
{
	const auto& block = fullBlock.getBlock();
	if (block.getId() == Block::mStoneBrick->getId() ||
		block.getId() == Block::mDirt->getId() ||
		block.getId() == Block::mHardenedClay->getId() ||
		block.getId() == Block::mSand->getId() ||
		block.getId() == Block::mNetherrack->getId() ||
		block.getId() == Block::mBedrock->getId() ||
		block.getId() == Block::mJungleStairs->getId() ||
		block.getId() == Block::mStoneBrickStairs->getId() ||
		block.getId() == Block::mCustom0->getId() ||
		block.getId() == Block::mCustom3->getId() ||
		block.getId() == Block::mCustom4->getId() ||
		block.getId() == Block::mCustom13->getId() ||
		block.getId() == Block::mCoalOre->getId() ||
		block.getId() == Block::mIronOre->getId() ||
		block.getId() == Block::mIce->getId() ||
		block.getId() == Block::mPackedIce->getId()
		) {
		return true;
	}
	else if (block.getId() == Block::mStainedClay->getId()) {
		ColoredBlockType mappedColor = block.getBlockState(BlockState::MappedType).get<ColoredBlockType>(fullBlock.data);
		switch (mappedColor) {
		case ColoredBlockType::Orange:
		case ColoredBlockType::Magenta:
		case ColoredBlockType::LightBlue:
		case ColoredBlockType::Pink:
		case ColoredBlockType::Cyan:
		case ColoredBlockType::Purple:
		case ColoredBlockType::Blue:
		case ColoredBlockType::Brown:
		case ColoredBlockType::Green:
			return true;
		default:
			break;
		}
	}
	else if (block.getId() == Block::mWoolCarpet->getId() || block.getId() == Block::mWool->getId()) {
		ColoredBlockType mappedColor = block.getBlockState(BlockState::MappedType).get<ColoredBlockType>(fullBlock.data);
		switch (mappedColor) {
		case ColoredBlockType::Orange:
		case ColoredBlockType::Red:
			return true;
		default:
			break;
		}
	}
	else if (block.getId() == Block::mStoneFloorSlab->getId()) {
		StoneSlabType mappedStyle = block.getBlockState(BlockState::MappedType).get<StoneSlabType>(fullBlock.data);
		switch (mappedStyle) {
		case StoneSlabType::Stone:
		case StoneSlabType::Sandstone:
		case StoneSlabType::Wood:
		case StoneSlabType::StoneBrick:
		case StoneSlabType::Quartz:
		case StoneSlabType::Netherbrick:
			return true;

		}
	}
	else if (block.getId() == Block::mWoodPlanks->getId()) {
		WoodBlockType mappedStyle = block.getBlockState(BlockState::MappedType).get<WoodBlockType>(fullBlock.data);
		if (mappedStyle == WoodBlockType::Jungle) {
			return true;
		}
	}
	return false;
}

bool UWindsweptPeaksSlippyBlockTrigger::IsTrigger(const FullBlock & fullBlock, ABaseCharacter * const character) const
{
	return BlockGraphicsHelper::getBlock(character->GetWorld(), conversion::ueToBlock(character->GetActorLocation()), fullBlock.getBlock().getId()).getMaterialType() == EMaterialTypeEnum::Ice;
}