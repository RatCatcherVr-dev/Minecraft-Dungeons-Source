//------------------------------------------------------------------------------
// <copyright file="RpcActor.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcBoundingBox.h"
#include "RpcVector.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server {
    class RpcArchive;
}}}}}

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

    /**
     * Actor data representation.
     */
    class UNREALTOOLSFRAMEWORK_API FRpcActor
    {
    public:
	    /** Actor's id. */
	    int32 Id;

	    /** Actor's name. */
	    FString Name;

	    /** Actor's location in the world. */
	    FRpcVector Location;

        /** Actor's bounding box. */
        FRpcBoundingBox BoundingBox;

        /** Is this actor a pawn? */
        bool bIsPawn;

        /** Is this actor a player? */
        bool bIsPlayer;

        /** Is this actor invincible? */
        bool bCanBeDamaged;

        /**
         * Stores the result of serializing or
         * de-serializing an RpcArchive.
         */
        HRESULT Result;

    public:
        /**
         * Initializes data to default values.
         */
        FRpcActor();

        /**
         * Initializes this object with data from the provided actor.
         */
        FRpcActor(AActor* Actor);

        /**
         * Returns an RpcArchive created with this instance's data.
         */
        operator Rpc::Server::RpcArchive();
    };

}}}}

#endif // ENABLE_GAMESTEST_RPC
