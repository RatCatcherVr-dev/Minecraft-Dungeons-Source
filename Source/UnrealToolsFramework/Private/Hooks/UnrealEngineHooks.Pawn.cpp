//------------------------------------------------------------------------------
// <copyright file="UnrealEngineHooks.Pawn.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "Util/CommonRpcActor.h"
#include "RpcErrorCodes.h"
#include "UnrealEngineHooks.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

using namespace GamesTest::Rpc::Server;

/**
 * Generates an output archive containing a collection of pawn data.
 * @param Pawns - Array of APawns to generate the archive from.
 * @param Output - Resulting RpcArchive with the 'Pawns' data.
 * @return A code indicating either success or failure.
 */
UTF_ANNOTATE_HRESULT_METHOD HRESULT CreateRpcArchiveFromPawns(const TArray<APawn*>& Pawns, RpcArchive& Output)
{
    TArray<RpcArchive> Results;
    for (APawn* Pawn : Pawns)
    {
        if (Pawn)
        {
            // FRpcPawn requires the actor to be valid.
            AActor* Actor = Cast<AActor>(Pawn);
            if (Actor)
            {
                FRpcPawn NewPawn(Pawn);
                Results.Add(NewPawn);
            }
        }
    }

    return Output.SetNamedCollection(TEXT("Pawns"), Results.GetData(), Results.Num());
}


RpcHookCode GetPawns(RpcArchive& Input, RpcArchive& Output)
{    
    UNREFERENCED_PARAMETER(Input);
    
    TArray<APawn*> Pawns;
    UWorld* World = GetWorld();
    if (World)
    {
        // Get Pawns in the current level.
        for (FConstPawnIterator Iterator = World->GetPawnIterator(); Iterator; ++Iterator)
        {
            APawn* Pawn = Iterator->Get();
            if (Pawn)
            {
                Pawns.Add(Pawn);
            }
        }

        return CreateRpcArchiveFromPawns(Pawns, Output);
    }

    RETURN_ELEMNOTFND("World");
}


RpcHookCode GetPawnById(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;

    int Id;
    HRCHK(Input.GetNamedValue(TEXT("Id"), &Id))

    APawn* Pawn = RetrievePawnById(Id);
    if (Pawn)
    {
        FRpcPawn NewPawn(Pawn);
        Output = NewPawn;
        HRCHK(NewPawn.Result)
    }
    
    return Result;
}


RpcHookCode GetPawnIsControlled(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;

    int Id;
    HRCHK(Input.GetNamedValue(TEXT("Id"), &Id))

    APawn* Pawn = RetrievePawnById(Id);
    if (Pawn)
    {
        HRCHK(Output.SetNamedValue(TEXT("IsControlled"), Pawn->IsControlled()))
        return Result;
    }

    RETURN_ELEMNOTFND("Pawn")
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
