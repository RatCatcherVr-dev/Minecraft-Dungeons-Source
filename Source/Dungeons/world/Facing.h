/*********************************************************
*   (c) Mojang. All rights reserved                      *
*   (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "world/level/BlockPos.h"
#include "world/phys/Vec3.h"
#include "util/Random.h"

class Facing {
public:

	enum Name : FacingID {
		DOWN = 0,
		UP = 1,
		NORTH = 2,
		SOUTH = 3,
		WEST = 4,
		EAST = 5,
		MAX = 6,
		NOT_DEFINED = MAX,			
		NUM_CULLING_IDS = 7
	};

	static const Name DIRECTIONS[6];
	static const BlockPos DIRECTION[6];
	static const Vec3 NORMAL[6];
	static const int FACING2D[6];
	static const Name BY2DDATA[4];

	typedef std::vector<FacingID> FacingIDList;

	static const FacingIDList ALL_FACES;
	static const std::array<FacingIDList, 6> ALL_EXCEPT;
	static const std::array<FacingIDList, 6> ALL_EXCEPT_AXIS_Y;
	static const std::array<FacingIDList, 6> ALL_EXCEPT_OPPOSITE_FACING_AND;

	static const char* toString(FacingID face) {
		static const char* s_names[MAX] = { "Down" ,"Up" ,"North" ,"South" ,"West", "East" };
		return s_names[face];
		//if (face == DOWN) {
		//	return "Down";
		//}
		//if (face == UP  ) {
		//	return "Up";
		//}
		//if (face == NORTH) {
		//	return "North";
		//}
		//if (face == SOUTH) {
		//	return "South";
		//}
		//if (face == WEST) {
		//	return "West";
		//}
		//if (face == EAST) {
		//	return "East";
		//}
		//return "Unknown facing";
	}

	static int getRandomFace(Random& random) {
		return random.nextInt(6);
	}

	static int getRandomFaceHorizontal(Random& random) {
		return random.nextInt(4) + 2; //excludes up/down
	}

	static Name from2DDataValue(const int data) {
		return BY2DDATA[std::abs(data % 4)];
	}

	static FacingID getClockWise(FacingID face);
	
	static const FacingID OPPOSITE_FACING[6];
	static const int STEP_X[6];
	static const int STEP_Y[6];
	static const int STEP_Z[6];

	static int getStepX(const FacingID facing) {
		return STEP_X[facing];
	}

	static int getStepY(const FacingID facing) {
		return STEP_Y[facing];
	}

	static int getStepZ(const FacingID facing) {
		return STEP_Z[facing];
	}

	static bool isHorizontal(const FacingID facing) {
		switch (facing) {
		case NORTH:
		case SOUTH:
		case WEST:
		case EAST:
			return true;
		default:
			return false;
		}
	}

	class Plane {
	public:
		static const FacingIDList HORIZONTAL;
	};

};
