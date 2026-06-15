#include "Dungeons.h"
#include "Tags.h"
#include "util/StringUtils.h"

Tags::Tags(const std::vector<std::string>& tags) {
	for (auto& tag : tags) {
		add(tag);
	}
}

void Tags::add(const std::string& tag_) {
	auto tag = Util::stringTrim(tag_);
	mTags.push_back(tag);
	mLowerTags.push_back(Util::toLower(tag));
	mHashedTags.push_back(std::hash<std::string>{}(tag));

}

bool Tags::has(const std::string& tag) const {
	return has_alreadyLowerCase(Util::toLower(tag));
}

bool Tags::has_alreadyLowerCase(const std::string& tag) const {
	return std::find(begin(mLowerTags), end(mLowerTags), tag) != end(mLowerTags);
}

bool Tags::has(size_t tag_hash) const {
	return std::find(begin(mHashedTags), end(mHashedTags), tag_hash) != end(mHashedTags);
}


const std::vector<std::string>& Tags::tags() const {
	return mTags;
}

const std::vector<size_t>& Tags::hashed_tags() const {
	return mHashedTags;
}

size_t Tags::size() const {
	return mTags.size();
}

/*
 *
 */
Tags fromCommaSeparatedString(const std::string& s) {
	return Tags(Util::split(s, ','));
}

std::string toCommaSeparatedString(const Tags& tags) {
	return Util::join(tags.tags());
}
