#include "Dungeons.h"
#include "LostEvocation.h"

ALostEvocation::ALostEvocation() : Super() {
	mobType = "seravex";
}

bool ALostEvocation::WillSummon() const
{
	return (GetAlivePetCount() == 0) && Super::WillSummon();
}