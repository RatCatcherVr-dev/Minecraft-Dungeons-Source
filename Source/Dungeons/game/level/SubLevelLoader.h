#pragma once

#include "StrongObjectPtr.h"

class UWorld;
class FString;
class ULevelStreamingDynamic;
struct FTransform;

namespace game { namespace sublevel {

class Loader {
public:
	using LevelLoadedCallback = std::function<void(ULevelStreamingDynamic*)>;
	using AllLevelsLoadedCallback = std::function<void()>;

	struct Config {
		int maxParallelLoadCount;
		int immediatelyLoadFirstSublevelsCount;
	};
	static const Config DefaultConfig;

	Loader(UWorld&, Config = DefaultConfig);
	~Loader();

	ULevelStreamingDynamic* schedule(const FString& path, const FString& uniqueButMustBeSameOnClientAndServerName, const FTransform& = {});
	TArray<ULevelStreamingDynamic*> allLevels() const;

	void update();
	bool startLoad(ULevelStreamingDynamic&);
	void registerLevelLoadedCallback(LevelLoadedCallback);
	bool AreAllLevelsLoaded() const;
	
private:
	void _updateLoadFinished();
	void _updateStartLoadBlocking();
	void _updateStartNonBlocking();

	struct Item {
		enum Status { NotLoaded, Started, Loaded };

		FString path;
		TStrongObjectPtr<ULevelStreamingDynamic> level;
		Status status;
	};
	void _startLoad(Item&);

	UWorld& mWorld;
	Config mConfig;
	TArray<Item> mItems;
	std::vector<LevelLoadedCallback> mLevelLoadedCallbacks;
};

}}
