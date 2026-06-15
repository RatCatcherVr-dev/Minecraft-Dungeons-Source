#include "Dungeons.h"
#include "NavAreaLava.h"

UNavAreaLava::UNavAreaLava(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DrawColor = FColor(51, 127, 0);	// greenish
	DefaultCost = 1000000.0f;
}

