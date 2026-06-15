#pragma once

#include "world/entity/EntityTypes.h"
#include "Containers/Map.h"
#include "UObject/SoftObjectPath.h"
#include "Containers/UnrealString.h"
#include <vector>

class UClass;

namespace game {

FString PrefabPath(const char* p);
FString PrefabPath(const FString& p);
FString PrefabPath(const FString& p, int suffix);
	
/*
 * @todo: We might want to use FStringAssetReference (or bind TAssetPtr properties)
 *        to get lazy loading, instead of loading all _classes_ on initialization.
 */
class TypeMap {
public:
	static const TypeMap& singleton();
	
	UClass* mobClass(EntityType) const;
	const std::vector<EntityType>& mobTypes() const;
	std::vector<FSoftObjectPath> Entities() const;

	FSoftObjectPath PathForEntity(EntityType) const;
	bool IsMapped(EntityType) const; 
	
private:
	TypeMap();

	// Since TypeMap::mobTypes is used to check what entity types are
	// "valid" types in the game, we might want to register additional
	// types here that has no mob class bound (e.g. Zombie (no variant))
	void _registerMobType(EntityType);
	void _register(EntityType, const FString& path);

	struct LazyClass {
		LazyClass(FString);
		UClass* getOrLoad();
		FString Path() const { return mPath; }
	private:
		FString mPath;
		TWeakObjectPtr<UClass> mClass = nullptr;
	};
	mutable TMap<EntityType, LazyClass> mTypeMap;
	
	mutable TMap<EntityType, FSoftObjectPath> mPathLookup;
	std::vector<EntityType> mMobTypes;
};

}
