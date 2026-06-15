#pragma once
#include "GameFramework/Actor.h"
#include "DecorationAnchor.generated.h"

class UArrowComponent;

UCLASS()
class DUNGEONS_API ADecorationAnchor: public AActor {
	GENERATED_BODY()

	static const FColor color;
public:
	ADecorationAnchor();

	static void initDecorationAnchorArrowHACK(UArrowComponent*);
	static bool isDecorationAnchorArrowHACK(const UArrowComponent*);
};
