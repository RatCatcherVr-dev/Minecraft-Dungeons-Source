//------------------------------------------------------------------------------
// <copyright file="GameStateQuery.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"
#include "GameStateQuery.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

FString GetPartialPathName(const UObject* Obj, unsigned int PackageLevel)
{
	const UObject* It;
	unsigned int currentPackageLevel = 0;
	for (It = Obj; It != nullptr; It = It->GetOuter())
	{
		if (It->GetClass() == UPackage::StaticClass())
		{
			currentPackageLevel++;
			if (currentPackageLevel >= PackageLevel)
			{
				break;
			}
		}
	}

	return FString(TEXT(".")) + Obj->GetPathName(It);
}

template<typename AllocatorType>
GameStateQuery<AllocatorType>::GameStateQuery()
    : Class(UObject::StaticClass())
{
}

template<typename AllocatorType>
void GameStateQuery<AllocatorType>::SetClass(const TCHAR* ClassName)
{
    Class = FindObject<UClass>(ANY_PACKAGE, ClassName);
}

template<typename AllocatorType>
void GameStateQuery<AllocatorType>::AddSearchString(FString Str)
{
	if (Str.StartsWith(TEXT("/")))
	{
		CompletePaths.Add(Str);
	}
	else if (Str.StartsWith(TEXT(".")))
	{
		PartialPaths.Add(Str);
	}
	else
	{
		Names.Add(*Str);
	}
}

template<typename AllocatorType>
bool GameStateQuery<AllocatorType>::MatchObject(UObject* Input) const
{
	if (Class == nullptr || !Input->IsA(Class))
	{
		return false;
	}
	
	return (!HasNames() && !HasPartialPaths() && !HasCompletePaths())
		|| Names.Contains(Input->GetFName())
		|| PartialPaths.Contains(GetPartialPathName(Input))
		|| CompletePaths.Contains(Input->GetPathName());
}

template<typename AllocatorType>
template<typename OutputAllocator>
void GameStateQuery<AllocatorType>::GetMatchingObjects(UWorld* World, TArray<UObject*, OutputAllocator>& Output) const
{
	// Optimization for UWorld, since we never need to query other worlds than the game world.
	if (Class->IsChildOf(UWorld::StaticClass()))
	{
		if (MatchObject(World))
		{
			Output.Add(World);
		}

		return;
	}

	for (TObjectIterator<UObject> It; It; ++It)
	{
		if (MatchObject(*It))
		{
			UWorld* ItWorld = It->GetWorld();
			if (ItWorld == nullptr || ItWorld == World)
			{
				Output.Add(*It);
			}
		}
	}
}

template<typename AllocatorType>
template<typename T, typename OutputAllocator>
void GameStateQuery<AllocatorType>::GetMatchingActors(T Iterator, TArray<AActor*, OutputAllocator>& Output) const
{
	for (; Iterator; ++Iterator)
	{
		if (MatchObject(Iterator->Get()))
		{
			Output.Add(Iterator->Get());
		}
	}
}

template<typename AllocatorType>
template<typename T, typename OutputAllocator>
void GameStateQuery<AllocatorType>::GetMatchingActors(TActorIterator<T> Iterator, TArray<AActor*, OutputAllocator>& Output) const
{
	for (; Iterator; ++Iterator)
	{
		if (MatchObject(*Iterator))
		{
			Output.Add(*Iterator);
		}
	}
}

template<typename AllocatorType>
template<typename OutputAllocator>
void GameStateQuery<AllocatorType>::GetMatchingActors(UWorld* World, TArray<AActor*, OutputAllocator>& Output) const
{
	// Optimizations to use specific iterators and specialize the TActorIterator template.
	if (Class->IsChildOf(APawn::StaticClass()))
	{
		GetMatchingActors(World->GetPawnIterator(), Output);
	}
	else if (Class->IsChildOf(APlayerController::StaticClass()))
	{
		GetMatchingActors(World->GetPlayerControllerIterator(), Output);
	}
	else if (Class->IsChildOf(AInfo::StaticClass()))
	{
		GetMatchingActors(TActorIterator<AInfo>(World), Output);
	}
	else
	{
		GetMatchingActors(TActorIterator<AActor>(World), Output);
	}
}

// Explicit template instantiation for RPC hooks
template struct GameStateQuery<TInlineAllocator<1>>;
template void GameStateQuery<TInlineAllocator<1>>::GetMatchingObjects<TInlineAllocator<16>>(UWorld* World, TArray<UObject*, TInlineAllocator<16>>& Output) const;
template void GameStateQuery<TInlineAllocator<1>>::GetMatchingActors<TInlineAllocator<16>>(UWorld* World, TArray<AActor*, TInlineAllocator<16>>& Output) const;

}}}}
