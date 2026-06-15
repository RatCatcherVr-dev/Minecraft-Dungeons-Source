//------------------------------------------------------------------------------
// <copyright file="GameStateQuery.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"
#include "Engine.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

/**
 * Represents a generic query for game objects or actors with a given class and name.
 */
template<typename AllocatorType = FDefaultAllocator>
struct GameStateQuery
{
public:
    GameStateQuery();

	// Gets the class to query.
	const UClass* GetClass() const { return Class; }

	// Sets the class name to query.
	void SetClass(const TCHAR* ClassName);

	// Sets the class to query.
	void SetClass(const UClass* InClass) { Class = InClass; }

	// Gets the short object names to query.
	const TArray<FName, AllocatorType>& GetNames() const { return Names; }

	// Gets the partial object paths (prefixed with ".") to query.
	const TArray<FString, AllocatorType>& GetPartialPaths() const { return PartialPaths; }

	// Gets the complete object paths (prefixed with "/") to query.
	const TArray<FString>& GetCompletePaths() const { return CompletePaths; }

	// True if the query includes short object names.
	bool HasNames() const { return Names.Num() > 0; }

	// True if the query includes partial object paths.
	bool HasPartialPaths() const { return PartialPaths.Num() > 0; }

	// True if the query includes complete object paths.
	bool HasCompletePaths() const { return CompletePaths.Num() > 0; }

	/**
	 * Adds a string to the filter criteria.
	 * @param Input - The string to add.
	 */
	void AddSearchString(FString Str);

	/**
	 * Determines if an object matches the filter criteria.
	 * @param Input - The object to test.
	 * @return True if the object was matched by the query.
	 */
	bool MatchObject(UObject* Input) const;

	/**
	 * Searches the world for objects matching this query.
	 * @param World - The world.
	 * @param Output - The matching objects.
	 */
	template<typename OutputAllocator>
	void GetMatchingObjects(class UWorld* World, TArray<UObject*, OutputAllocator>& Output) const;

	/**
	 * Searches the world for actors matching this query.
	 * @param World - The world.
	 * @param Output - The matching actors.
	 */
	template<typename OutputAllocator>
	void GetMatchingActors(class UWorld* World, TArray<AActor*, OutputAllocator>& Output) const;

private:
	template<typename T, typename OutputAllocator>
	void GetMatchingActors(T Iterator, TArray<AActor*, OutputAllocator>& Output) const;

	template<typename ActorType, typename OutputAllocator>
	void GetMatchingActors(TActorIterator<ActorType> Iterator, TArray<AActor*, OutputAllocator>& Output) const;

	const UClass* Class;

	TArray<FName, AllocatorType> Names;

	TArray<FString, AllocatorType> PartialPaths;

	TArray<FString> CompletePaths;
};

/**
 * Extracts a string from the object's path name containing up to
 * the requested number of package names from the end of the path.
 * @param Obj - The object.
 * @param PackageLevel - The number of packages to include before the object name.
 * @return A string with the specified number of components of the path name.
 */
FString GetPartialPathName(const UObject* Obj, unsigned int PackageLevel = 1);

}}}}
