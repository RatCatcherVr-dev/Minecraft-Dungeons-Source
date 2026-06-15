#include "Dungeons.h"
#include "OrientationDataCalculator.h"
#include "world/level/block/Block.h"
#include "world/Direction.h"
#include "world/Facing.h"
#include "world/level/block/TorchBlock.h"
#include "world/level/block/StairBlock.h"
#include "world/level/block/RotatedPillarBlock.h"
#include "BlockPosTransform.h"

static const int DoorBlock_C_DIR_MASK = 3;
static const int DoorBlock_C_IS_UPPER_MASK = 8;

OrientationDataCalculator::OrientationDataCalculator(QuadrantAngle angle)
	: orientation(angle) {
}

constexpr unsigned int BitCountMask(int numBits) { return (1 << numBits) - 1; }

template <unsigned int Mask>
static __inline DataID ReplaceWithBitMask(DataID original, DataID replace) {
	return (original & ~Mask) | (replace & Mask);
}

template <unsigned int BitCount>
static __inline DataID ReplaceBits(DataID original, DataID replace) {
	return ReplaceWithBitMask<BitCountMask(BitCount)>(original, replace);
}

static const QuadrantAngle Unused = QuadrantAngle::D0;

DataID OrientationDataCalculator::DirectionConv(DataID in, QuadrantAngle r) {
	return ((in - (DataID) r) + 4) & BitCountMask(2);
}

DataID OrientationDataCalculator::FacingConv(DataID in, QuadrantAngle r) {
	if (in <= Facing::UP) {
		return in;
	}
	static std::array<DataID, 4> AngleToId = { Facing::EAST, Facing::NORTH, Facing::WEST, Facing::SOUTH };
	static std::array<QuadrantAngle, 16> IdToAngle = { Unused, Unused, QuadrantAngle::D90, QuadrantAngle::D270, QuadrantAngle::D180, QuadrantAngle::D0 };
	return AngleToId[int(IdToAngle[in] + r)];
}

DataID OrientationDataCalculator::FaceShift(DataID in, QuadrantAngle r) {
	const int h = in << (4 - (int) r);
	return BitCountMask(4) & (h | (h >> 4));
}

DataID OrientationDataCalculator::TorchConv(DataID in, QuadrantAngle r) {
	if (in == TorchBlock::TORCH_TOP) {
		return in;
	}
	static std::array<DataID, 4> AngleToId = { TorchBlock::TORCH_EAST, TorchBlock::TORCH_NORTH, TorchBlock::TORCH_WEST, TorchBlock::TORCH_SOUTH };
	static std::array<QuadrantAngle, 16> IdToAngle = { Unused, QuadrantAngle::D180, QuadrantAngle::D0, QuadrantAngle::D90, QuadrantAngle::D270};
	return AngleToId[int(IdToAngle[in] + r)];
}

DataID OrientationDataCalculator::StairConv(DataID in, QuadrantAngle r) {
	static std::array<DataID, 4> AngleToId = { StairBlock::DIR_EAST, StairBlock::DIR_NORTH, StairBlock::DIR_WEST, StairBlock::DIR_SOUTH };
	static std::array<QuadrantAngle, 16> IdToAngle = { QuadrantAngle::D0, QuadrantAngle::D180, QuadrantAngle::D270, QuadrantAngle::D90 };
	return ReplaceBits<2>(in, AngleToId[int(IdToAngle[in & BitCountMask(2)] + r)]);
}

DataID OrientationDataCalculator::DoorConv(DataID in, QuadrantAngle r) {
	return (in & DoorBlock_C_IS_UPPER_MASK) ? in : ReplaceWithBitMask<DoorBlock_C_DIR_MASK>(in, OrientationDataCalculator::DirectionConv(in, r));
}

DataID OrientationDataCalculator::RailConv(DataID in, QuadrantAngle r) {
	if (in <= 1) {
		return in ^ ((int) r & 1);
	}
	if (in >= 2 && in <= 5) {
		return (2 + OrientationDataCalculator::StairConv((in ^ 1) - 2, r)) ^ ((int(r) + 1) & 1);
	}
	if (in >= 6 && in <= 9) {
		in -= (int) r;
		return in < 6 ? in + 4 : in;
	}
	return in;
}

DataID OrientationDataCalculator::RotatedPillarConv(DataID in, QuadrantAngle r) {
	auto index = (in & 0xC) >> 2;
	if (index == RotatedPillarBlock::FACING_Y || !isPerpendicularlyRotated(r)) {
		return in;
	}
	return ((3 - index) << 2) | (in & 3);
}

typedef DataID(*OrientationCalculationFuncPtr)(DataID, QuadrantAngle);

OrientationCalculationFuncPtr GetBlockCalculationCallbackFunc(int index)
{
	const auto block = Block::mBlocks[index];
	if (block == Block::mVine) {
		return OrientationDataCalculator::FaceShift;
	}
	else if (block == Block::mHayBlock
		|| block == Block::mLog
		|| block == Block::mLog2
		|| block == Block::mQuartzBlock) {
		return OrientationDataCalculator::RotatedPillarConv;
	}
	else if (block == Block::mLadder
		|| block == Block::mChest
		|| block == Block::mStoneButton
		|| block == Block::mWoodButton) {
		return OrientationDataCalculator::FacingConv;
	}
	else if (block->hasProperty(BlockProperty::Door)) {
		return OrientationDataCalculator::DoorConv;
	}
	else if (block->isStairBlock()
		|| block == Block::mTrapdoor
		|| block == Block::mIronTrapdoor) {
		return OrientationDataCalculator::StairConv;
	}
	else if (block == Block::mTorch
		|| block == Block::mUnlitRedStoneTorch
		|| block == Block::mLitRedStoneTorch) {
		return OrientationDataCalculator::TorchConv;
	}
	else if (block->isRailBlock()) {
		return OrientationDataCalculator::RailConv;
	}
	else if (block == Block::mPoweredRepeater
		|| block == Block::mUnpoweredRepeater
		|| block == Block::mPoweredComparator
		|| block == Block::mUnpoweredComparator
		|| block == Block::mCocoa
		|| block == Block::mPumpkin
		|| block == Block::mLitPumpkin) {
		return OrientationDataCalculator::DirectionConv;
	}

	return nullptr;
};


DataID OrientationDataCalculator::calculate(FullBlock fullBlock) const {
	const auto data = fullBlock.data;


	static const auto CalculationFunctions = [] {
		std::array<OrientationCalculationFuncPtr, Block::NUM_BLOCK_TYPES> out;

		for (int i = 0; i < Block::NUM_BLOCK_TYPES; ++i) 
		{
			out[i] = GetBlockCalculationCallbackFunc(i);
		}
		return out;
	}();

	if (const auto& calculator = CalculationFunctions[fullBlock.id]) 
	{
		return calculator(data, orientation);
	}

	return data;

}

FullBlock OrientationDataCalculator::operator()(FullBlock block) const {
	return FullBlock(block.id, calculate(block));
}


//
// Factories
//
OrientationDataCalculator orientationDataTransform(const BlockPosTransform& f) {
	return OrientationDataCalculator(blockpostransform::rotation(f));
}

BlockSetter orientationDataTransform(const BlockSetter& setter, const BlockPosTransform& transform) {
	auto dt = orientationDataTransform(transform);
	return [=](auto pos, auto block) {
		return setter(transform(pos), dt(block));
	};
}
