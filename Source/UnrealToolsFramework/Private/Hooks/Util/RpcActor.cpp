//------------------------------------------------------------------------------
// <copyright file="RpcActor.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcArchive.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

FRpcActor::FRpcActor()
{
}


FRpcActor::FRpcActor(AActor* Actor)
{
    // Include the main actor data.
    this->Id = Actor->GetUniqueID();
    this->Name = Actor->GetFullName();
    this->Location = Actor->GetActorLocation();
    
    // Add the actor's bounding box.
    FVector Origin, Extents;
    Actor->GetActorBounds(true, Origin, Extents);
    this->BoundingBox = FRpcBoundingBox(Origin, Extents);
    
    // Additional actor data.
    APawn* Pawn = Cast<APawn>(Actor);
    this->bIsPawn = Pawn ? true : false;
    this->bIsPlayer = (Pawn && Pawn->Controller) ? Pawn->Controller->IsA<APlayerController>() : false;
    this->bCanBeDamaged = Actor->bCanBeDamaged;
}


FRpcActor::operator Rpc::Server::RpcArchive()
{
    Rpc::Server::RpcArchive Output;
    HRESULT TempResult;
    this->Result = S_OK;

    HRCHKTHIS(Output.SetNamedValue(TEXT("Id"), this->Id))
    HRCHKTHIS(Output.SetNamedStringValue(TEXT("Name"), *(this->Name)))
    HRCHKTHIS(Output.SetNamedValue(TEXT("Location"), this->Location))
    HRCHKTHIS(Output.SetNamedValue(TEXT("BoundingBox"), this->BoundingBox))
    HRCHKTHIS(Output.SetNamedValue(TEXT("IsPawn"), this->bIsPawn))
    HRCHKTHIS(Output.SetNamedValue(TEXT("IsPlayer"), this->bIsPlayer))
    HRCHKTHIS(Output.SetNamedValue(TEXT("CanBeDamaged"), this->bCanBeDamaged))

    // Also check there was no failure while serializing these.
    HRCHKTHIS(this->Location.Result)
    HRCHKTHIS(this->BoundingBox.Result)

    return Output;
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
