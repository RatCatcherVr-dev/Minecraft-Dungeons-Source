#include "Dungeons.h"
#include "DecorationAnchor.h"
#include "Components/ArrowComponent.h"

const FColor ADecorationAnchor::color(200, 201, 202);

ADecorationAnchor::ADecorationAnchor() {
	auto arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RootComponent"));
	initDecorationAnchorArrowHACK(arrow);
	SetRootComponent(arrow);
}

void ADecorationAnchor::initDecorationAnchorArrowHACK(UArrowComponent* arrow) {
	arrow->ArrowColor = color;
}

bool ADecorationAnchor::isDecorationAnchorArrowHACK(const UArrowComponent* arrow) {
	return arrow->ArrowColor == color;
}
