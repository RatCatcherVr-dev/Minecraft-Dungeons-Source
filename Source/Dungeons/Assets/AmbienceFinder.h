#pragma once

#include "AssetData.h"
#include "CoreMinimal.h"
#include "game/level/ambience/AmbienceActor.h"
#include "AmbienceFinder.Generated.h"

class IAssetRegistry;

struct FAssetWrapper {
	FAssetData data;

	TOptional<EAmbienceAudioID> audioId;
	TOptional<EAmbienceID> visualId;
};

UCLASS(Config=Game)
class DUNGEONS_API UAmbienceFinder : public UObject {
	GENERATED_BODY()
public:

	bool IsInitalized() const { return bInitialized; }
	void Initialize(bool force = false);

	TArray<FSoftObjectPath> GetAmbiencesForGroup(const FString&) const;
	TOptional<FSoftObjectPath> GetAudioAmbienceForGroupWithId(const FString&, EAmbienceAudioID) const;
	TOptional<FSoftObjectPath> GetVisualAmbienceForGroupWithId(const FString&, EAmbienceID) const;

	FString GroupForPath(const FString&) const;
	
private:
	FString GroupForData(const FAssetData&) const;
	
	UPROPERTY(Config)
	FString AmbienceRoot; 
	
	TMap<FName,TArray<FAssetWrapper>> MappedAssets;

	bool bInitialized = false;
};
