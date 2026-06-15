//------------------------------------------------------------------------------
// <copyright file="RpcPawn.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcActor.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server {
    class RpcArchive;
}}}}}

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

    /**
     * Pawn data representation.
     */
    class UNREALTOOLSFRAMEWORK_API FRpcPawn
    {
    public:
	    /** Actor's data. */
	    FRpcActor ActorData;

        /** Is the pawn being controlled? */
        bool bIsControlled;

        /**
         * Stores the result of serializing or
         * de-serializing an RpcArchive.
         */
        HRESULT Result;

    public:
        /**
         * Initializes data to default values.
         */
        FRpcPawn();

        /**
         * Initializes this object with data from the provided pawn.
         */
        FRpcPawn(APawn* Pawn);

        /**
         * Returns an RpcArchive created with this instance's data.
         */
        operator Rpc::Server::RpcArchive();
    };

}}}}

#endif // ENABLE_GAMESTEST_RPC
