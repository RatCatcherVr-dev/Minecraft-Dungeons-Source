#include "DungeonsWorldSettings.h"

ADungeonsWorldSettings::ADungeonsWorldSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) 
{
	bReplicates = false;
	bAlwaysRelevant = false;
}