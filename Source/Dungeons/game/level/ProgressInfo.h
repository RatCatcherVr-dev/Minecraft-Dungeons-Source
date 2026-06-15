#pragma once

namespace levelgen { struct TileProgress; }

namespace game {

struct LevelDef;

class Progress {
public:
	Progress(int index, int length)
		: mIndex(index)
		, mLength(length) {
	}
	int index() const { return mIndex; }
	int remaining() const { return mLength - mIndex; }
	int length() const { return mLength; }

	float fraction() const { return mIndex / float(mLength); }
	float fractionAfter() const { return (mIndex + 1) / float(mLength); }
private:
	int mIndex;
	int mLength;
};

class ProgressInfo {
public:
	ProgressInfo(const LevelDef&, const levelgen::TileProgress&);
	const Progress& global() const;
	const Progress& globalWithStrayPath() const;
	const Progress& strayPath() const;
	const Progress& stretchLocal() const;
	const Progress& stretches() const;
	int             strayPathSubId() const;

	bool mostlyEquals(const ProgressInfo&) const;
	bool isOnStrayPath() const;

private:
	Progress mLocal;
	Progress mGlobal;
	Progress mGlobalWithStrayPath;
	Progress mStrayPath;
	Progress mStretches;
	int mStrayPathSubId;
};

}
