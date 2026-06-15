//------------------------------------------------------------------------------
// <copyright file="CommonRpcActor.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"

#ifdef ENABLE_GAMESTEST_RPC

namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server {
    class RpcArchive;
}}}}}

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

/**
 * Template to perform an action on each actor in the current level.
 * @param Action - Function that returns a boolean. When false, iteration will stop.
 */
template <typename T>
void ForEachActor(T Action)
{
    UWorld* World = GetWorld();
    if (World)
    {
        ULevel* Level = World->GetCurrentLevel();
        if (Level)
        {
            for (AActor* Actor : Level->Actors)
            {
                if (Actor && Action(Actor) == false)
                {
                    break;
                }
            }
        }
    }
}

/**
 * Retrieves an actor's location.
 * @param Actor - Actor whose location will be retrieved.
 * @param Output - Archive containing the actor's 'Location'.
 * @return A code indicating either success or failure.
 */
UTF_ANNOTATE_HRESULT_METHOD HRESULT GetActorLocation(const AActor* Actor, Rpc::Server::RpcArchive& Output);

/**
 * Updates an actor's location.
 * @param Actor - Actor whose location will be updated.
 * @param Input - Containing new 'Location'.
 * @param Output - Containing error information (if any).
 * @return A code indicating either success or failure.
 */
UTF_ANNOTATE_HRESULT_METHOD HRESULT SetActorLocation(AActor* Actor, Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

/**
 * Gets a value indicating whether an actor can be damaged.
 * @param Actor - Actor to query.
 * @param Output - Archive containing a value indicating whether an actor 'CanBeDamaged'.
 * @return A code indicating either success or failure.
 */
UTF_ANNOTATE_HRESULT_METHOD HRESULT GetActorCanBeDamaged(const AActor* Actor, Rpc::Server::RpcArchive& Output);

/**
 * Enables/disables damage on an actor, i.e. invincibility.
 * @param Actor - Actor to update.
 * @param Input - Containing flag indicating if the actor 'CanBeDamaged'.
 * @param Output - Containing error information (if any).
 * @return A code indicating either success or failure.
 */
UTF_ANNOTATE_HRESULT_METHOD HRESULT SetActorCanBeDamaged(AActor* Actor, Rpc::Server::RpcArchive& Input, Rpc::Server::RpcArchive& Output);

}}}}

#endif // ENABLE_GAMESTEST_RPC
