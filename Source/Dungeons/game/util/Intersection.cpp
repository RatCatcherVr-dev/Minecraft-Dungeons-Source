#include "Dungeons.h"
#include "Intersection.h"

namespace intersection {
	bool intersects(
		const Location& capsuleTransform,
		const FCapsule& capsule,
		const LocationDirection& fullWedgeTransform,
		const FFullWedge& fullWedge
	) {		
		const auto& capsuleLocation = capsuleTransform.location;		

		// test z difference first (cheapest, but rejects very few)
		const auto capsuleHalfHeight = capsule.halfHeight;

		if (
			fullWedgeTransform.location.Z + fullWedge.halfHeight < capsuleLocation.Z - capsuleHalfHeight ||
			fullWedgeTransform.location.Z - fullWedge.halfHeight > capsuleLocation.Z + capsuleHalfHeight
		) {
			return false;
		}

		// test angle vs actor center (rejects a lot)
		const auto deltaNormal = (capsuleLocation - fullWedgeTransform.location).GetSafeNormal2D();

		if (
			FVector2D::DotProduct(FVector2D { fullWedgeTransform.direction }, FVector2D { deltaNormal }) <
			FMath::Cos(fullWedge.halfAngleRadians)
		) {
			return false;
		}

		// test wedge radius vs actor radius		
		const auto distanceSquared = FVector::DistSquared2D(fullWedgeTransform.location, capsuleLocation - deltaNormal * capsule.radius);

		if (distanceSquared > FMath::Square(fullWedge.radius)) {
			return false;
		}

		// test wedge end-points vs actor radius
		// ...

		// test wedge edges vs actor radius (angle vs actor center should not return early anymore)
		// ...

		return true;
	}
	
	bool intersects(
		const LocationDirection& wedgeTransform,
		const FFullWedge& wedge,
		const Location& capsuleTransform,
		const FCapsule& capsule
	) {
		return intersects(capsuleTransform, capsule, wedgeTransform, wedge);
	}
}
