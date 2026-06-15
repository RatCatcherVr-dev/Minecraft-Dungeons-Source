#include "Dungeons.h"
#include "HyperPortal.h"
#include "lovika/world/level/levelgen/SourceData.h"
#include "util/Algo.hpp"
#include "util/StringUtils.h"

namespace levelgen { namespace hajper { namespace portal {

PortalUnlockKeys::PortalUnlockKeys(const std::string& prefix)
	: mPrefix(prefix)
	, mNone(get("none")) {
}

bool PortalUnlockKeys::matchesType(const std::string& name) const {
	return Util::startsWith(name, mPrefix);
}

std::string PortalUnlockKeys::get(const std::string& name) const {
	return mPrefix + name;
}

const std::string& PortalUnlockKeys::none() const {
	return mNone;
}

bool isPortalOfType(const UnlockKeys& unlockKeys, const PortalUnlockKeys& portalKeys) {
	return algo::any_of(unlockKeys, RETLAMBDA(portalKeys.matchesType(it)));
}

bool isPortalWithKey(const UnlockKeys& unlockKeys, const std::string& portalKey) {
	return algo::contains(unlockKeys, portalKey);
}

std::string getPortalKey(const PortalUnlockKeys& keys, const io::Level& current, const io::Level* other) {
	if (!other) {
		return keys.none();
	}
	if (other->dimension) {
		return keys.get(other->dimension.GetValue());
	}
	return keys.get("nether");
}

const PortalUnlockKeys& incomingPortalKeys() {
	static const PortalUnlockKeys keys("hm_from_");
	return keys;
}

const PortalUnlockKeys& outgoingPortalKeys() {
	static const PortalUnlockKeys keys("hm_to_");
	return keys;
}

}}}
