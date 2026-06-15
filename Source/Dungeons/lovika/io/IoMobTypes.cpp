#include "Dungeons.h"
#include "IoMobTypes.h"
#include "world/entity/EntityTypes.h"

namespace io {

//
// MobType
//
MobType::Expr::Expr(EntityType type) : Expr(EntityTypeToString(type)) {}

MobType::MobType(const Expr& expr, float weight, unsigned int min, unsigned int max, float maxFractionOfTotal)
	: expr(expr.expr)
	, weight(weight)
	, min(min)
	, max(max)
	, maxFractionOfTotal(maxFractionOfTotal) {
}

io::MobType MobType::exactly(const Expr& expr, int count, float weight) {
	return MobType(expr, weight, count, count);
}

io::MobType MobType::one(const Expr& expr, float weight) {
	return exactly(expr, 1, weight);
}

//
// MobGroup
//
MobGroup::MobGroup(std::vector<MobType> types, const std::string& id, float weight)
	: types(std::move(types))
	, id(id)
	, lowerId(Util::toLower(id))
	, weight(weight) {
}

}
