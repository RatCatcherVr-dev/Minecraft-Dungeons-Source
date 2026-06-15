//------------------------------------------------------------------------------
// <copyright file="UnrealEngineHooks.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcArchive.h"
#include "RpcServer.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

//********************************
// ENGINE
//********************************

/**
 * Hook to return the name and url of the current map.
 * @param Input - N/A
 * @param Output - 'MapName'. Name of the current map.
 *               - 'MapUrl'. URL used to load the current map.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetCurrentMapData(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to load a specified map.
 * @param Input - 'Url' to load the map.
 * @param Output - N/A
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode LoadMap(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to return the names (values) of a specified enum.
 * @param Input - 'Enum'. Name of the Enum to get the values for.
 * @param Output - List of 'EnumNames'.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetEnumValues(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to execute a console command.
 * @param Input - 'Command' to execute (include arguments).
 * @param Output - 'CmdOutput' of the command (if any).
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode ExecuteCommand(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
* Hook to gather auto complete command data from the console.
* @param Input - N/A.
* @param Output - 'AutoCompleteCommands' List of the auto complete commands.
* @return A code indicating either success or failure.
*/
Rpc::Server::RpcHookCode GetAutoCompleteCommands(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

//********************************
// ACTOR
//********************************

/**
 * Hook to return all actors.
 * @param Input - 'NameFilter'. Optional. Only actors whose class name
 *                contains this filter will be returned.
 * @param Output - List of all 'Actors' in the current level.
 *                 NameFilter is applied if one is provided.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetActors(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to return an actor by id.
 * @param Input - 'Id' of the actor to retrieve.
 * @param Output - Actor that was found. Or empty archive if not found.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetActorById(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to return an actor's location.
 * @param Input - 'Id' of the actor whose location will be retrieved.
 * @param Output - 'Location' of the actor.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetActorLocation(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to update an actor's location.
 * @param Input - 'Id' of the actor whose location will be updated.
 *              - 'Location' to move the actor to.
 * @param Output - N/A
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode SetActorLocation(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to get a value indicating whether an actor can be damaged.
 * @param Input - 'Id' of the actor.
 * @param Output - 'CanBeDamaged'. Indicates if the actor can be damaged.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetActorCanBeDamaged(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to enable/disable damage on an actor, i.e. invincibility.
 * @param Input - 'Id' of the actor to update.
 *              - 'CanBeDamaged'. Indicates if the actor should have damage applied.
 * @param Output - N/A
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode SetActorCanBeDamaged(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to destroy an actor by its unique id.
 * @param Input - 'Id' of the actor to destroy.
 * @param Output - N/A
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode DestroyActor(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
* Hook to find the value of properties on game objects based on class or object name filters.
* @param Input - 'ClassName' and 'ObjectName' to look for the values from 'Properties'.
* @param Output - The value of the specified properties for the matching object(s).
* @return A code indicating either success or failure.
*/
Rpc::Server::RpcHookCode GetObjectProperties(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
* Hook to find the value of properties on game actors based on class or object name filters.
* @param Input - 'ClassName' and 'ObjectName' to look for the values from 'Properties'.
* @param Output - The value of the specified properties for the matching actor(s).
* @return A code indicating either success or failure.
*/
Rpc::Server::RpcHookCode GetActorProperties(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

//********************************
// PAWN
//********************************

/**
 * Hook to return all pawns.
 * @param Input - N/A
 * @param Output - List of all 'Pawns' in the current level.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetPawns(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to return a pawn by id.
 * @param Input - 'Id' of the pawn to retrieve.
 * @param Output - Pawn that was found. Or empty archive if not found.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetPawnById(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to get a value indicating whether a pawn is being controlled.
 * @param Input - 'Id' of the pawn.
 * @param Output - 'IsControlled'. Indicates if the pawn is being controlled.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetPawnIsControlled(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

//********************************
// PLAYER
//********************************

/**
 * Hook to return all local players.
 * @param Input - N/A.
 * @param Output - List of player 'Players' in the current scene.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetLocalPlayers(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to return a local player by index.
 * @param Input - 'Index' of the player to retrieve.
 * @param Output - Player that was found. Or empty archive if not found.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetLocalPlayerByIndex(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to return a local player's controller id.
 * @param Input - 'Index' of the player whose location will be retrieved.
 * @param Output - 'ControllerId' assigned to the player.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetPlayerControllerId(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to return a local player's location.
 * @param Input - 'Index' of the player whose location will be retrieved.
 * @param Output - 'Location' of the player.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetPlayerLocation(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to update a local player's location.
 * @param Input - 'Index' of the player whose location will be updated.
 *              - 'Location' to move the player to.
 * @param Output - N/A
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode SetPlayerLocation(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to return a local player's rotation.
 * @param Input - 'Index' of the player whose rotation will be retrieved.
 * @param Output - 'Rotation' of the player.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetPlayerRotation(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to update a local player's rotation.
 * @param Input - 'Index' of the player whose rotation will be updated.
 *              - 'Rotation' to orient the player.
 * @param Output - N/A
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode SetPlayerRotation(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to teleport the local player with the specified index to a
 *          certain position and rotation in the current level.
 * @param Input - 'Index' of the player to teleport.
 *              - 'Location' to teleport the player to.
 *              - 'Rotation' to place the player in.
 *              - 'EnforceNotClipping' indicates if the teleport should be done no
 *                 matter what. If set to true the engine will look for and adjust
 *                 or fail due to clipping issues.
 * @param Output - N/A
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode TeleportPlayer(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to get a value indicating whether a local player can be damaged.
 * @param Input - 'Index' of the player.
 * @param Output - 'CanBeDamaged'. Indicates if the player can be damaged.
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode GetPlayerCanBeDamaged(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Hook to enable/disable damage on a local player, i.e. invincibility.
 * @param Input - 'Index' of the player to update.
 *              - 'CanBeDamaged'. Indicates if the player should have damage applied.
 * @param Output - N/A
 * @return A code indicating either success or failure.
 */
Rpc::Server::RpcHookCode SetPlayerCanBeDamaged(Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

}}}}

#endif // ENABLE_GAMESTEST_RPC
