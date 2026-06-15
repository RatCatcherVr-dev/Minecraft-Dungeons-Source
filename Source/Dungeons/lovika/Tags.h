#pragma once

class Tags {
public:
	Tags() {}
	Tags(const std::vector<std::string>& tags);

	void add(const std::string& tag);

	bool has(const std::string& tag) const;
	bool has(size_t tag_hash) const;
	bool has_alreadyLowerCase(const std::string& lowerCaseTag) const;

	const std::vector<std::string>& tags() const;
	const std::vector<size_t>&      hashed_tags() const;

	size_t size() const;
private:
	std::vector<std::string> mTags;
	std::vector<std::string> mLowerTags;
	std::vector<size_t>      mHashedTags;
};

Tags fromCommaSeparatedString(const std::string&);
std::string toCommaSeparatedString(const Tags&);
