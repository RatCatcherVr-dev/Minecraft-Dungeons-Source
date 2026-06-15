#include "Dungeons.h"
#include "DarkConduitInteraction.h"
#include "DungeonsGameState.h"

ADarkConduitInteraction::ADarkConduitInteraction() 
{
	InteractionRange = 1200000.0f;
}


void ADarkConduitInteraction::BeginPlay() 
{
	Super::BeginPlay();	
	InstanceTracker<ADarkConduitInteraction>::AddInstance(GetWorld(), this);
}

void ADarkConduitInteraction::EndPlay(EEndPlayReason::Type EndPlayReason) 
{
	Super::EndPlay(EndPlayReason);
	InstanceTracker<ADarkConduitInteraction>::RemoveInstance(GetWorld(), this);
}

void ADarkConduitInteraction::TriggerOnInConduitRangeChanged(float ConduitDistSq)
{
	bool IsInRange = ConduitDistSq <= InteractionRange;
	if(InteractionActive != IsInRange)
	{
		if (IsInRange)
			OnConduitInRange();
		else
			OnConduitOutOfRange();

		InteractionActive = IsInRange;
	}
}

void ADarkConduitInteraction::TriggerOnInConduitRangeChanged(bool NewValue)
{
	if (InteractionActive != NewValue)
	{
		if (NewValue)
			OnConduitInRange();
		else
			OnConduitOutOfRange();

		InteractionActive = NewValue;
	}
}