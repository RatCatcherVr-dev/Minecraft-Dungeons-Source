//------------------------------------------------------------------------------
// <copyright file="UnrealEngineHelperFunctions.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"
#include "AssertionMacros.h"
#include "Engine/GameInstance.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

UWorld* GetWorld()
{
    UWorld* World = nullptr;

    // @TODO: Add support for editor and/or PIE if desired.
    if (GEngine && !GIsEditor)
    {
        // According to the description of the FWorldContext class (as of UE 4.7):
        // For the GameEngine, there will be one WorldContext until we decide to support multiple simultaneous worlds.
        for (const FWorldContext& Context : GEngine->GetWorldContexts())
        {
            if (Context.World() && Context.WorldType == EWorldType::Game)
            {
                checkf(World == nullptr, TEXT("This function currently supports games with one world. Modify if this is not the case for your game."))
                World = Context.World();
            }
        }
    }

    return World;
}


AActor* RetrieveActorById(int Id)
{
    UWorld* World = GetWorld();
    if (World)
    {
        ULevel* Level = World->GetCurrentLevel();
        if (Level)
        {
            for (AActor* Actor : Level->Actors)
            {
                if (Actor && Actor->GetUniqueID() == Id)
                {
                    return Actor;
                }
            }
        }
    }

    return nullptr;
}


APawn* RetrievePawnById(int Id)
{
    UWorld* World = GetWorld();
    if (World)
    {
        // Get Pawns in the current level.
        for (FConstPawnIterator Iterator = World->GetPawnIterator(); Iterator; ++Iterator)
        {
            APawn* Pawn = Iterator->Get();
            if (Pawn)
            {
                // Only return if its actor data can be obtained.
                AActor* Actor = Cast<AActor>(Pawn);
                if (Actor && Actor->GetUniqueID() == Id)
                {
                    return Pawn;
                }
            }
        }
    }

    return nullptr;
}


ULocalPlayer* RetrieveLocalPlayerByIndex(int Index)
{
    UWorld* World = GetWorld();
    if (World)
    {
        UGameInstance* Game = World->GetGameInstance();
        if (Game && Index >= 0 && Index < Game->GetNumLocalPlayers())
        {
            return Game->GetLocalPlayerByIndex(Index);
        }
    }

    return nullptr;
}


UConsole* GetGameConsole()
{
    UWorld* World = GetWorld();
    if (World)
    {
        ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
        if (LocalPlayer && LocalPlayer->ViewportClient)
        {
            return LocalPlayer->ViewportClient->ViewportConsole;
        }
    }

    return nullptr;
}


HRESULT PlayerConsoleCommand(const FString& Command, APlayerController* PlayerController, FString& CmdOutput)
{
    UPlayer* Player = PlayerController->Player;
    HRESULT Result = S_OK;

    if (Player)
    {
        // Write to console and log.
        UConsole* ViewportConsole = (GEngine && GEngine->GameViewport) ? GEngine->GameViewport->ViewportConsole : nullptr;
        FConsoleOutputDevice Output(ViewportConsole);

        // The following buffers will allow dividing the input command in case a
        // chain of commands was requested.
        const int32 CmdLen = Command.Len();
        TCHAR* CommandBuffer = (TCHAR*)FMemory::Malloc((CmdLen+1)*sizeof(TCHAR));
        TCHAR* SingleCommand = (TCHAR*)FMemory::Malloc((CmdLen+1)*sizeof(TCHAR));

        // Copying command into a non-const buffer to process the chain of commands (if any).
        const TCHAR* ModifiableCommand = CommandBuffer;
        FCString::Strcpy(CommandBuffer, CmdLen+1, *Command.Left(CmdLen));

        // Iterate over the command, assuming different commands break up on |, CR, and LF.
        while (FParse::Line(&ModifiableCommand, SingleCommand, CmdLen+1))
        {
            // Second half included as the line may be empty if
            // the whole line was a comment and thus ignored.
            if (Player && *SingleCommand != 0)
            {
                if (!Player->Exec(PlayerController->GetWorld(), SingleCommand, Output))
                {
                    // Indicate a command in the chain couldn't be
                    // executed and stop processing.
                    Result = UTF_E_CONSOLE_COMMAND_NOT_EXECUTED;
                    break;
                }
            }
        }

        // Free up allocated memory.
        FMemory::Free(CommandBuffer);
        CommandBuffer = nullptr;

        FMemory::Free(SingleCommand);
        SingleCommand = nullptr;

        // Return any output produced by all successful commands.
        CmdOutput = *Output;
        return Result;
    }

    return UTF_E_PLAYER_WAS_NOT_FOUND;
}


HRESULT ViewportConsoleCommand(const FString& Command, UGameViewportClient* GameViewportClient, FString& CmdOutput)
{
	FString TruncatedCommand = Command.Left(1000);
	FConsoleOutputDevice Output(GameViewportClient->ViewportConsole);

    if (!GameViewportClient->Exec(GameViewportClient->GetWorld(), *TruncatedCommand, Output))
    {
        return UTF_E_CONSOLE_COMMAND_NOT_EXECUTED;
    }

    CmdOutput = *Output;
	return S_OK;
}

}}}}
