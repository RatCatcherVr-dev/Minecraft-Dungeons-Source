#include "Dungeons.h"
#include "TextureCache.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Engine/Texture2D.h"
#include <SoftObjectPtr.h>
#include "util/Algo.hpp"

void UTextureCache::cancelAsyncLoad() {
	if (AsyncLoadHandle.IsValid()) {
		AsyncLoadHandle->CancelHandle();
		AsyncLoadHandle.Reset();
	}
}

void UTextureCache::unloadTextures() {	
	cancelAsyncLoad();
	LoadedTextures.Reset();
	mLoaded = false;
}

void UTextureCache::loadAndCacheTextures(const TArray<TSoftObjectPtr<UTexture2D>>& textures) {
	cancelAsyncLoad();
	
	const TArray<FSoftObjectPath> softPaths = algo::map_tarray(textures, RETLAMBDA(it.ToSoftObjectPath()));

	TWeakObjectPtr<UTextureCache> WeakThis(this);

	AsyncLoadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(softPaths,
		[WeakThis, textures]() {
		if (WeakThis.IsValid())
		{			
			WeakThis->LoadedTextures.Reset();
			WeakThis->AsyncLoadHandle.Reset();
			WeakThis->mLoaded = true;
			for (auto& softPtr : textures) {
				if (auto* obj = softPtr.Get()) {
					WeakThis->LoadedTextures.Add(obj);
				}
			}
		}
	}, FStreamableManager::AsyncLoadHighPriority);
	
}

bool UTextureCache::isLoadingOrLoaded() const {
	return AsyncLoadHandle.IsValid() || mLoaded;
}


