#pragma once

#include <string>
#include <vector>

using UnlockKeys = std::vector<std::string>;

namespace io { struct Level; }

namespace levelgen { namespace hajper { namespace portal {

struct PortalUnlockKeys {
	PortalUnlockKeys(const std::string& prefix);

	bool matchesType(const std::string& name) const;
	std::string get(const std::string& name) const;

	const std::string& none() const;
private:
	std::string mPrefix;
	std::string mNone;
};

const PortalUnlockKeys& incomingPortalKeys();
const PortalUnlockKeys& outgoingPortalKeys();

bool isPortalOfType(const UnlockKeys&, const PortalUnlockKeys&);
bool isPortalWithKey(const UnlockKeys&, const std::string&);
std::string getPortalKey(const PortalUnlockKeys&, const io::Level& current, const io::Level* other);

}}}
