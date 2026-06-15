//------------------------------------------------------------------------------
// <copyright file="RpcLocalPlayer.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcArchive.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

FRpcLocalPlayer::FRpcLocalPlayer()
{
}


FRpcLocalPlayer::FRpcLocalPlayer(const ULocalPlayer* Player, int32 Index)
{
    APlayerController* PlayerController = Player->PlayerController;
    APawn* Pawn = PlayerController->GetPawn();
	this->PawnData = Pawn;

    this->PlayerIndex = Index;
    this->Rotation = Pawn->GetActorRotation();
    this->ControllerId = Player->GetControllerId();
    this->bIsPrimaryPlayer = PlayerController->IsPrimaryPlayer();
    this->bIsSplitscreenPlayer = PlayerController->IsSplitscreenPlayer();
}


FRpcLocalPlayer::operator Rpc::Server::RpcArchive()
{
    Rpc::Server::RpcArchive Output;
    HRESULT TempResult;
    this->Result = S_OK;

    HRCHKTHIS(Output.SetNamedValue(TEXT("Pawn"), this->PawnData))
    HRCHKTHIS(Output.SetNamedValue(TEXT("PlayerIndex"), this->PlayerIndex))
    HRCHKTHIS(Output.SetNamedValue(TEXT("Rotation"), this->Rotation))
    HRCHKTHIS(Output.SetNamedValue(TEXT("ControllerId"), this->ControllerId))
    HRCHKTHIS(Output.SetNamedValue(TEXT("IsPrimaryPlayer"), this->bIsPrimaryPlayer))
    HRCHKTHIS(Output.SetNamedValue(TEXT("IsSplitscreenPlayer"), this->bIsSplitscreenPlayer))

    // Also check there was no failure while serializing these.
    HRCHKTHIS(this->PawnData.Result)
    HRCHKTHIS(this->Rotation.Result)

    return Output;
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
