#pragma once

#include "Intersection.generated.h"

struct Location {
	FVector location;

	Location(const FVector& location)
		: location { location }
	{};
};

struct LocationDirection {
	FVector location;
	FVector direction;

	LocationDirection(const FVector& location, const FVector& direction)
		: location { location }
		, direction { direction }
	{};

	LocationDirection(const AActor& actor)
		: location { actor.GetActorLocation() }
		, direction { actor.GetActorForwardVector() }
	{};
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FCapsule {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float halfHeight;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FWedge {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float radiusInner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float radiusOuter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float halfAngleRadians;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FFullWedge {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float halfAngleRadians;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float halfHeight;
};

namespace intersection {
	bool intersects(const Location&, const FCapsule&, const LocationDirection&, const FFullWedge&);
	bool intersects(const LocationDirection&, const FFullWedge&, const Location&, const FCapsule&);
}