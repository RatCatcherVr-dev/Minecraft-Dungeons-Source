//------------------------------------------------------------------------------
// <copyright file="RpcPawn.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcArchive.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

FRpcPawn::FRpcPawn()
{
}


FRpcPawn::FRpcPawn(APawn* Pawn)
{
    AActor* Actor = Cast<AActor>(Pawn);
    this->ActorData = Actor;
    this->bIsControlled = Pawn->IsControlled();
}


FRpcPawn::operator Rpc::Server::RpcArchive()
{
    Rpc::Server::RpcArchive Output;
    HRESULT TempResult;
    this->Result = S_OK;

    HRCHKTHIS(Output.SetNamedValue(TEXT("Actor"), this->ActorData))
    HRCHKTHIS(Output.SetNamedValue(TEXT("IsControlled"), this->bIsControlled))

    // Also check there was no failure while serializing the actor.
    HRCHKTHIS(this->ActorData.Result)

    return Output;
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
