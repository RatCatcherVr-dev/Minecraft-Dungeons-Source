#include "Dungeons.h"

#include "AlphaGeneratorTypes.h"
#include "lovika/io/LevelFile.h"

namespace generator { namespace alpha {

MaybeGenResult generateSafe(const Env&, const std::vector<io::Stretch>&, RandomSeed, int tries, int maxFailedPlacements);
ValidationIssues generateValidationIssuesFor(const Env&, const GenerationError&);

}}
