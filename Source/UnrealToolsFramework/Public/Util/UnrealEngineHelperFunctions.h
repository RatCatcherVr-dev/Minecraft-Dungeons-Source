//------------------------------------------------------------------------------
// <copyright file="UnrealEngineHelperFunctions.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

/** Returns the active world to call hooks on. */
UNREALTOOLSFRAMEWORK_API UWorld* GetWorld();

/**
 * Returns an actor by id.
 * @param Id - Id of the actor to retrieve.
 * @return The actor with the specified id or null if not found.
 */
UNREALTOOLSFRAMEWORK_API AActor* RetrieveActorById(int Id);

/* Summary: Returns a pawn by id.
 * @param Id - Id of the pawn to retrieve.
 * @return The pawn with the specified id or null if not found.
 */
UNREALTOOLSFRAMEWORK_API APawn* RetrievePawnById(int Id);

/**
 * Returns a local player by index.
 * @param Index - Index of the player to retrieve.
 * @return The local player on the specified index or null if not found.
 */
UNREALTOOLSFRAMEWORK_API ULocalPlayer* RetrieveLocalPlayerByIndex(int Index);

/**
 * Get the viewport console for the first local player that is found.
 * According to documentation in UGameViewPortClient (UE 4.7):
 * "Exactly one GameViewportClient is created for each instance of the game."
 * So getting it for the first player found is enough even on coop.
 * @return Game's viewport console.
 */
UNREALTOOLSFRAMEWORK_API UConsole* GetGameConsole();

/**
 * Executes a console command in the context of the specified player controller.
 * @param Command - Command to execute.
 * @param PlayerController - Player to execute the console command on.
 * @param CmdOutput - Output of the command (if any).
 * @return A result indicating if all commands were executed successfully.
 *
 * Remarks: This function is based on APlayerController::ConsoleCommand found at:
 *          .\Engine\Source\Runtime\Engine\Private\PlayerController.cpp
 *          with 2 main differences:
 *            1. This one provides an additional piece of data back: whether the
 *               command was executed or not.
 *            2. This function may give output back.
 *         KEEP AN EYE on the original function and modify this one if necessary!!!
 *          Last checked: UE 4.7
 *
 * Info: 'Command' can be either one command or various commands separated by pipes (|)
 *       or new lines (CR/LF). This function will execute them in sequence.
 *       This function also allows for comments in 'Command' by using '//'.
 *       Everything after '//' will be ignored as a command until the next CR, LF, or |
 *       is encountered.
 */
UNREALTOOLSFRAMEWORK_API HRESULT PlayerConsoleCommand(const FString& Command, APlayerController* PlayerController, FString& CmdOutput);

/**
 * Executes a console command in the context of the game's viewport client.
 * @param Command - Command to execute.
 * @param GameViewportClient - GameViewportClient to execute the console command on.
 * @param CmdOutput - Output of the command (if any).
 * @return A result indicating if the command was executed successfully.
 *
 * Remarks: This function is based on UGameViewportClient::ConsoleCommand found at:
 *          .\Engine\Source\Runtime\Engine\Private\GameViewportClient.cpp
 *          with 2 main differences:
 *            1. This one provides an additional piece of data back: whether the
 *               command was executed or not.
 *            2. This function always gives output back.
 *         KEEP AN EYE on the original function and modify this one if necessary!!!
 *          Last checked: UE 4.7
 */
UNREALTOOLSFRAMEWORK_API HRESULT ViewportConsoleCommand(const FString& Command, UGameViewportClient* GameViewportClient, FString& CmdOutput);

}}}}
