#include "DungeonsLevelScriptActor.h"

ADungeonsLevelScriptActor::ADungeonsLevelScriptActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = false;
	bAlwaysRelevant = false;
	bReplayRewindable = false;
}