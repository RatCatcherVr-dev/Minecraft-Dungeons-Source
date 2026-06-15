#pragma once
#include <Runtime/Engine/Public/EngineUtils.h>
#include <Runtime/Engine/Classes/Kismet/BlueprintFunctionLibrary.h>
#include "CommonTypes.h"

class ABaseCharacter;

namespace characterquery {
	using CharacterPred = Pred<const ABaseCharacter*>;
namespace is{
	CharacterPred hostile(const ABaseCharacter*);
	bool hostile(const ABaseCharacter*, const ABaseCharacter*);
	CharacterPred friendly(const ABaseCharacter*);
	bool friendly(const ABaseCharacter*, const ABaseCharacter*);
	bool targetable(const ABaseCharacter*);
	bool movable(const ABaseCharacter*);
	bool boss(const ABaseCharacter*);
}
namespace can {
	CharacterPred heal(const ABaseCharacter*);
	CharacterPred damage(const ABaseCharacter*);
}
}

//#include "util/BooleanOperatorMacros.h"
//GENERATE_BOOLEAN_PRED_OPERATORS(const ABaseCharacter*);