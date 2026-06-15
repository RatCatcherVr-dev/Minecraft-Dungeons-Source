#pragma once

#include "MediaAssets/Public/PlatformMediaSource.h"
#include "EndVideoDefinitions.generated.h"

UCLASS()
class UEndVideoDefinition : public UObject {
	GENERATED_BODY()
public:
	UEndVideoDefinition() {}

	UEndVideoDefinition(const TCHAR* path) {
		const ConstructorHelpers::FObjectFinder<UPlatformMediaSource> videoPath(path);
		mSource = videoPath.Object;
	}

	virtual UMediaSource* GetSource() const {
		return mSource;
	}

protected:
	UMediaSource* mSource = nullptr;
};

UCLASS()
class UObsidianPinnacleOutroVideo : public UEndVideoDefinition {
	GENERATED_BODY()
public:
	UObsidianPinnacleOutroVideo()
		: UEndVideoDefinition(TEXT("/Game/Movies/GameOutro/OutroVideo_Platform.OutroVideo_Platform"))
	{}
};

UCLASS()
class UTheStrongholdOutroVideo : public UEndVideoDefinition {
	GENERATED_BODY()
public:
	UTheStrongholdOutroVideo()
		: UEndVideoDefinition(TEXT("/Game/Movies/DLC6/End_C2_Stronghold_Outro_Platform.End_C2_Stronghold_Outro_Platform"))
	{}
};

UCLASS()
class UBlightedCitadelOutroVideo : public UEndVideoDefinition {
	GENERATED_BODY()
public:
	UBlightedCitadelOutroVideo()
		: UEndVideoDefinition(TEXT("/Game/Movies/DLC6/End_C4_BrokenCitadel_Outro_Part2_Platform.End_C4_BrokenCitadel_Outro_Part2_Platform"))
	{}
};

template <typename VideoClass>
std::function<UMediaSource*()> CreateEndVideoFunctor() {
	return []() {
		UClass* clazz = VideoClass::StaticClass();
		UEndVideoDefinition* videoDefinition = Cast<UEndVideoDefinition>(clazz->GetDefaultObject());
		check(videoDefinition);
		return videoDefinition->GetSource();
	};
}

struct EndVideoMetadata {

	bool mPlayOnce;

	bool ShouldPlayOnce() const { return mPlayOnce; }
};
