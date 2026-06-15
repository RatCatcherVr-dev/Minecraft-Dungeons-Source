//------------------------------------------------------------------------------
// <copyright file="UnrealEngineHooks.Player.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

// UTF & TDK RPC Includes
#include "Util/CommonRpcActor.h"
#include "RpcErrorCodes.h"
#include "UnrealEngineHooks.h"

// Unreal Engine Includes
#include "Engine/GameInstance.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

using namespace GamesTest::Rpc::Server;

/**
 * Retrieves a player's actor based on the index in input.
 * @param Input - Archive containing the player's 'Index'.
 * @param Actor - Actor that was retrieved or null if not found.
 * @return A code indicating either success or failure.
 */
UTF_ANNOTATE_HRESULT_METHOD HRESULT GetPlayerByIndexFromInput(RpcArchive& Input, AActor*& Actor)
{
    HRESULT Result = S_OK;
    Actor = nullptr;

    int Index;
    HRCHK(Input.GetNamedValue(TEXT("Index"), &Index))

    ULocalPlayer* Player = RetrieveLocalPlayerByIndex(Index);
    if (Player && Player->PlayerController)
    {
        Actor = Cast<AActor>(Player->PlayerController->GetPawn());
        return Result;
    }
    
    return UTF_E_PLAYER_WAS_NOT_FOUND;
}

/**
 * Generates an output archive containing a collection of player data.
 * @param Players - Array of ULocalPlayers to generate the archive from.
 * @param Output - Resulting RpcArchive with the 'Players' data.
 * @return A code indicating either success or failure.
 */
UTF_ANNOTATE_HRESULT_METHOD HRESULT CreateRpcArchiveFromLocalPlayers(const TArray<ULocalPlayer*>& Players, RpcArchive& Output)
{
    TArray<RpcArchive> Results;
    for (int32 Idx = 0; Idx < Players.Num(); ++Idx)
    {
        ULocalPlayer* Player = Players[Idx];
        if (Player)
        {
            // Make sure this is a complete local player.
            APlayerController* Controller = Player->PlayerController;
            if (Controller)
            {
                AActor* Actor = Cast<AActor>(Controller->GetPawn());
                if (Actor)
                {
                    FRpcLocalPlayer NewPlayer(Player, Idx);
                    Results.Add(NewPlayer);
                }
            }
        }
    }

    return Output.SetNamedCollection(TEXT("Players"), Results.GetData(), Results.Num());
}


RpcHookCode GetLocalPlayers(RpcArchive& Input, RpcArchive& Output)
{
    UNREFERENCED_PARAMETER(Input);

    UWorld* World = GetWorld();
    if (World)
    {
        UGameInstance* Game = World->GetGameInstance();
        if (Game)
        {
            return CreateRpcArchiveFromLocalPlayers(Game->GetLocalPlayers(), Output);
        }

        RETURN_ELEMNOTFND("GameInstance");
    }

    RETURN_ELEMNOTFND("World");
}


RpcHookCode GetLocalPlayerByIndex(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;

    int Index;
    HRCHK(Input.GetNamedValue(TEXT("Index"), &Index))

    ULocalPlayer* Player = RetrieveLocalPlayerByIndex(Index);
    if (Player)
    {
        // Make sure this is a complete local player.
        APlayerController* Controller = Player->PlayerController;
        if (Controller)
        {
            AActor* Actor = Cast<AActor>(Controller->GetPawn());
            if (Actor)
            {
                FRpcLocalPlayer NewPlayer(Player, Index);
                Output = NewPlayer;
                HRCHK(NewPlayer.Result)
            }
        }
    }
    
    return Result;
}


RpcHookCode GetPlayerControllerId(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;

    int Index;
    HRCHK(Input.GetNamedValue(TEXT("Index"), &Index))

    ULocalPlayer* Player = RetrieveLocalPlayerByIndex(Index);
    if (Player)
    {
        return Output.SetNamedValue(TEXT("ControllerId"), Player->GetControllerId());
    }
    
    return UTF_E_PLAYER_WAS_NOT_FOUND;
}


RpcHookCode GetPlayerLocation(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;
    AActor* Actor = nullptr;

    HRCHK(GetPlayerByIndexFromInput(Input, Actor))
    return GetActorLocation(Actor, Output);
}


RpcHookCode SetPlayerLocation(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;
    AActor* Actor = nullptr;

    HRCHK(GetPlayerByIndexFromInput(Input, Actor))
    return SetActorLocation(Actor, Input, Output);
}


RpcHookCode GetPlayerRotation(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;
    AActor* Actor = nullptr;

    HRCHK(GetPlayerByIndexFromInput(Input, Actor))
    if (Actor)
    {
        FRpcRotator Rotation(Actor->GetActorRotation());
        HRCHK(Output.SetNamedValue(TEXT("Rotation"), Rotation))
        return Rotation.Result;
    }

    RETURN_ELEMNOTFND("Actor")
}


RpcHookCode SetPlayerRotation(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;

    int Index;
    HRCHK(Input.GetNamedValue(TEXT("Index"), &Index))

    ULocalPlayer* Player = RetrieveLocalPlayerByIndex(Index);
    if (Player && Player->PlayerController)
    {
        APawn* Pawn = Player->PlayerController->GetPawn();
        if (Pawn)
        {
            RpcArchive RotationArchive;
            HRCHK(Input.GetNamedValue(TEXT("Rotation"), &RotationArchive))
            FRpcRotator Rotation(RotationArchive);
            HRCHK(Rotation.Result)

            // Depending on Pawn settings, 'FaceRotation' may ignore the
            // rotation request or not. Making request through both Pawn
            // and Controller to ensure the update happens.
            Pawn->FaceRotation(Rotation, 0.0f);
            Player->PlayerController->SetControlRotation(Rotation);

            return Result;
        }
    }
    
    UNREFERENCED_PARAMETER(Output);
    return UTF_E_PLAYER_WAS_NOT_FOUND;
}


RpcHookCode TeleportPlayer(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;

    int Index;
    HRCHK(Input.GetNamedValue(TEXT("Index"), &Index))

    ULocalPlayer* Player = RetrieveLocalPlayerByIndex(Index);
    if (Player && Player->PlayerController)
    {
        APawn* Pawn = Player->PlayerController->GetPawn();
        if (Pawn)
        {
            // Get information for teleport.
            RpcArchive LocationArchive;
            HRCHK(Input.GetNamedValue(TEXT("Location"), &LocationArchive))
            FRpcVector Location(LocationArchive);
            HRCHK(Location.Result)

            RpcArchive RotationArchive;
            HRCHK(Input.GetNamedValue(TEXT("Rotation"), &RotationArchive))
            FRpcRotator Rotation(RotationArchive);
            HRCHK(Rotation.Result)

            bool bEnforceNotClipping;
            HRCHK(Input.GetNamedValue(TEXT("EnforceNotClipping"), &bEnforceNotClipping))

            // Inverting 'bEnforceNotClipping' as parameter for function is inverted.
            bool bSuccess = Pawn->TeleportTo(Location, Rotation, false, !bEnforceNotClipping);
            if (!bSuccess)
            {
                RETURN_FUNCFAILED("AActor::TeleportTo")
            }

            // Depending on Pawn settings, 'TeleportTo' and 'FaceRotation' may
            // ignore the rotation request or not. Making request through both
            // Pawn and Controller to ensure the update happens.
            Pawn->FaceRotation(Rotation, 0.0f);
            Player->PlayerController->SetControlRotation(Rotation);

            return Result;
        }
    }
    
    UNREFERENCED_PARAMETER(Output);
    return UTF_E_PLAYER_WAS_NOT_FOUND;
}


RpcHookCode GetPlayerCanBeDamaged(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;
    AActor* Actor = nullptr;

    HRCHK(GetPlayerByIndexFromInput(Input, Actor))
    return GetActorCanBeDamaged(Actor, Output);
}


RpcHookCode SetPlayerCanBeDamaged(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;
    AActor* Actor = nullptr;

    HRCHK(GetPlayerByIndexFromInput(Input, Actor))
    return SetActorCanBeDamaged(Actor, Input, Output);
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
