#pragma once

namespace game { namespace objective {

class Completion {
public:
	Completion(size_t count, size_t max);
	static Completion Failed();

	bool isPending() const;
	bool isCompleted() const;
	bool isFailed() const;

	float fraction() const;
	int current() const;
	int max() const;

	bool operator==(const Completion&) const;
	bool operator!=(const Completion&) const;
protected:
	int mCount;
	int mMax;
};

}}
