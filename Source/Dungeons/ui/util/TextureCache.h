#pragma once

#include "CoreMinimal.h"
#include <SharedPointer.h>
#include "TextureCache.generated.h"

struct FStreamableHandle;

UCLASS()
class DUNGEONS_API UTextureCache : public UObject {
	GENERATED_BODY()
private:
	TSharedPtr<FStreamableHandle> AsyncLoadHandle;

	UPROPERTY()
	TArray<UTexture2D*> LoadedTextures;

	bool mLoaded = false;

	void cancelAsyncLoad();

public:
	void unloadTextures();
	void loadAndCacheTextures(const TArray<TSoftObjectPtr<UTexture2D>>& textures);
	bool isLoadingOrLoaded() const;
};
