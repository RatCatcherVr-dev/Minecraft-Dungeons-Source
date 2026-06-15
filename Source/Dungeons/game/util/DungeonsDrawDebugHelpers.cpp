#include "Dungeons.h"
#include "DungeonsDrawDebugHelpers.h"
#include "DrawDebugHelpers.h"

namespace DungeonsDebug {

void drawDebugWedge(UWorld* world, FVector fromLocation, FVector direction, float minDistUnits, float maxDistUnits, float maxAngleRadians, float maxZDiff, float coneOffset, FColor const& color) {
	float numLines = 7;
	for (auto i = 0; i <= numLines; i++) {
		auto angle = -maxAngleRadians + i * maxAngleRadians * 2 / numLines;
		auto dir = direction.RotateAngleAxis(angle, FVector::UpVector);
		dir.Normalize();
		auto start = fromLocation + dir * minDistUnits;
		auto end = fromLocation + dir * maxDistUnits;
		auto zDiff = FVector(0, 0, -maxZDiff);
		DrawDebugLine(world, start + zDiff, end + zDiff, color, false, 2.0f, 0);
		zDiff = FVector(0, 0, maxZDiff);
		DrawDebugLine(world, start + zDiff, end + zDiff, color, false, 2.0f, 0);
		if (i == 0 || i == numLines) {
			for (auto j = 0; j <= numLines; j++) {
				zDiff = FVector(0, 0, -maxZDiff + j * maxZDiff * 2 / numLines);
				DrawDebugLine(world, start + zDiff, end + zDiff, color, false, 2.0f, 0);
			}
		}
	}


}


}