#pragma once
#include "CommonTypes.h"
#include "QuadrantAngle.h"
#include "world/level/BlockPos.h"

class OrientationDataCalculator { // : BlockTransform
public:
	OrientationDataCalculator(QuadrantAngle);

	DataID calculate(FullBlock) const;
	FullBlock operator()(FullBlock) const;


	static DataID FaceShift(DataID in, QuadrantAngle r);
	static DataID RotatedPillarConv(DataID in, QuadrantAngle r);
	static DataID FacingConv(DataID in, QuadrantAngle r);
	static DataID StairConv(DataID in, QuadrantAngle r);
	static DataID TorchConv(DataID in, QuadrantAngle r);
	static DataID RailConv(DataID in, QuadrantAngle r);
	static DataID DirectionConv(DataID in, QuadrantAngle r);
	static DataID DoorConv(DataID in, QuadrantAngle r);

private:
	QuadrantAngle orientation;
};

using BlockSetter = BlockCallback;

OrientationDataCalculator orientationDataTransform(const BlockPosTransform&);
BlockSetter orientationDataTransform(const BlockSetter& nonTransformingSetter, const BlockPosTransform&);
