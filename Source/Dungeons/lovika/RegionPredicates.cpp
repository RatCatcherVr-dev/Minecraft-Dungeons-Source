#include "Dungeons.h"
#include "RegionPredicates.h"

namespace regionpredicates {

const RegionPredicate& all() {
	static const RegionPredicate pred = [](const lovika::Region& region) { return true; };
	return pred;
}

RegionPredicate type(const RegionType& type) {
	return [&type](const lovika::Region& region) { return region.type() == type; };
}

RegionPredicate name(const std::string& name) {
	return [lowerName = Util::toLower(name)](const lovika::Region& region) { return region.lowerName() == lowerName; };
}

RegionPredicate exactName(const std::string& name) {
	return [name](const lovika::Region& region) { return region.name() == name; };
}

RegionPredicate hasTag(const std::string& tag) {
	return[lowerTag = Util::toLower(tag)](const lovika::Region& region) { return region.lowerTagString().find(lowerTag) != std::string::npos; };
}

const RegionPredicate& isCheckPoint() {
	static const RegionPredicate pred = hasTag("checkpoint");
	return pred;
}

const RegionPredicate& isRegularSpawn() {
	static const RegionPredicate pred = type(regiontype::Spawn) && !isRegularSpawnBlocked() && !isArenaSpawn() && !isFixedMob() && !isStaticMesh();
	return pred;
}

const RegionPredicate& isRegularSpawnBlocked()
{
	static const RegionPredicate pred = type(regiontype::Spawn) && hasTag("nodefaultspawn");
	return pred;
}


const RegionPredicate& isPuzzleSpawn()
{
	static const RegionPredicate pred = type(regiontype::Spawn) && !isArenaSpawn() && !isFixedMob() && !isStaticMesh();
	return pred;
}

const RegionPredicate& isArenaSpawn() {
	static const RegionPredicate pred = type(regiontype::Spawn) && (name("arena") || hasTag("arena"));
	return pred;
}

const RegionPredicate& isPlayerStart() {
	static const RegionPredicate pred = name("playerstart");
	return pred;
}

const RegionPredicate& isTrigger() {
	static const RegionPredicate pred = type(regiontype::Trigger);
	return pred;
}

const RegionPredicate& isLoot() {
	static const RegionPredicate pred = type(regiontype::Loot);
	return pred;
}

const RegionPredicate& isDeathTrigger() {
	static const RegionPredicate pred = isTrigger() && (hasTag("death") || hasTag("freeze"));
	return pred;
}

const RegionPredicate& isObjectiveHACK() {
	static const RegionPredicate pred = isTrigger() && !isDeathTrigger();
	return pred;
}

const RegionPredicate& isImmobileMob() {
	static const RegionPredicate pred = hasTag("immob");
	return pred;
}

const RegionPredicate& isFixedMob() {
	static const RegionPredicate pred = hasTag("fixed") || isImmobileMob();
	return pred;
}

const RegionPredicate& isStaticMesh() {
	static const RegionPredicate pred = hasTag("static");
	return pred;
}

const RegionPredicate& isSpawn() {
	static const RegionPredicate pred = type(regiontype::Spawn);
	return pred;
}

const RegionPredicate& isCollisionCullAbove() {
	static const RegionPredicate pred = isTrigger() && hasTag("nocollisionabove");
	return pred;
}

const RegionPredicate& isCollisionCullBelow() {
	static const RegionPredicate pred = isTrigger() && (hasTag("death") || hasTag("freeze") || hasTag("nocollisionbelow"));
	return pred;
}

const RegionPredicate& isShadowCullAbove() {
	static const RegionPredicate pred = isTrigger() && hasTag("noshadowcastabove");
	return pred;
}

const RegionPredicate& isShadowCullBelow() {
	static const RegionPredicate pred = isTrigger() && hasTag("noshadowcastbelow");
	return pred;
}

const RegionPredicate& isQuickCullBelow()
{
	static const RegionPredicate pred = isTrigger() && hasTag("cullblocksbelow");
	return pred;
}

const RegionPredicate& isQuickCullAbove()
{
	static const RegionPredicate pred = isTrigger() && hasTag("cullblocksabove");
	return pred;
}


}
