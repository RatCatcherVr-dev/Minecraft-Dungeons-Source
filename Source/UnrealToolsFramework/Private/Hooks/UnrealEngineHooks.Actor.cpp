//------------------------------------------------------------------------------
// <copyright file="UnrealEngineHooks.Actor.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "Util/CommonRpcActor.h"
#include "RpcErrorCodes.h"
#include "UnrealEngineHooks.h"
#include <vector>

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

using namespace GamesTest::Rpc::Server;

/**
 * Generates an output archive containing a collection of actor data.
 * @param Actors - Array of AActors to generate the archive from.
 * @param Output - Resulting RpcArchive with the 'Actors' data.
 * @return A code indicating either success or failure.
 */
UTF_ANNOTATE_HRESULT_METHOD HRESULT CreateRpcArchiveFromActors(const TArray<AActor*>& Actors, RpcArchive& Output)
{
    TArray<RpcArchive> Results;
    for (AActor* Actor : Actors)
    {
        if (Actor)
        {
            FRpcActor NewActor(Actor);
            Results.Add(NewActor);
        }
    }

    return Output.SetNamedCollection(TEXT("Actors"), Results.GetData(), Results.Num());
}


RpcHookCode GetActors(RpcArchive& Input, RpcArchive& Output)
{
    // Get the optional name filter.
    FString NameFilter;
    HRESULT Result = GetStringValue(TEXT("NameFilter"), Input, &NameFilter);
    if (FAILED(Result) && Result != GAMESTEST_RPC_E_MEMBER_NOT_FOUND_IN_ARCHIVE)
    {
        return Result;
    }

    // Get all actors filtered by NameFilter (if any).
    TArray<AActor*> Actors;

    ForEachActor([&] (AActor* Actor) 
    { 
        FString Name = Actor->GetFullName();
        if (NameFilter.IsEmpty() || Name.Contains(NameFilter))
        {
            Actors.Add(Actor);
        }

        return true;
    });

    return CreateRpcArchiveFromActors(Actors, Output);
}


RpcHookCode GetActorById(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;

    int Id;
    HRCHK(Input.GetNamedValue(TEXT("Id"), &Id))

    AActor* Actor = RetrieveActorById(Id);
    if (Actor)
    {
        FRpcActor NewActor(Actor);
        Output = NewActor;
        HRCHK(NewActor.Result)
    }
    
    return Result;
}


RpcHookCode GetActorLocation(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;

    int Id;
    HRCHK(Input.GetNamedValue(TEXT("Id"), &Id))

    AActor* Actor = RetrieveActorById(Id);
    return GetActorLocation(Actor, Output);
}


RpcHookCode SetActorLocation(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;

    int Id;
    HRCHK(Input.GetNamedValue(TEXT("Id"), &Id))

    AActor* Actor = RetrieveActorById(Id);
    return SetActorLocation(Actor, Input, Output);
}


RpcHookCode GetActorCanBeDamaged(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;

    int Id;
    HRCHK(Input.GetNamedValue(TEXT("Id"), &Id))

    AActor* Actor = RetrieveActorById(Id);
    return GetActorCanBeDamaged(Actor, Output);
}


RpcHookCode SetActorCanBeDamaged(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;

    int Id;
    HRCHK(Input.GetNamedValue(TEXT("Id"), &Id))

    AActor* Actor = RetrieveActorById(Id);
    return SetActorCanBeDamaged(Actor, Input, Output);
}


RpcHookCode DestroyActor(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;

    int Id;
    HRCHK(Input.GetNamedValue(TEXT("Id"), &Id))

    AActor* Actor = RetrieveActorById(Id);
    if (Actor)
    {
        // Destroy controller first if actor is a pawn.
        APawn* Pawn = Cast<APawn>(Actor);
        if (Pawn && Pawn->Controller && !Pawn->Controller->Destroy())
        {
            RETURN_FUNCFAILED("AActor::Destroy")
        }

        // Destroy actor.
        if (!Actor->Destroy())
        {
            RETURN_FUNCFAILED("AActor::Destroy")
        }

        // Actor has been destroyed.
        UNREFERENCED_PARAMETER(Output);
        return Result;
    }

    RETURN_ELEMNOTFND("Actor")
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
