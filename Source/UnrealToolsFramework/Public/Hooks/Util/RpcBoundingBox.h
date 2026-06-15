//------------------------------------------------------------------------------
// <copyright file="RpcBoundingBox.h" company="Microsoft">
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
     * Simple class representing a bounding box.
     */
    class UNREALTOOLSFRAMEWORK_API FRpcBoundingBox
    {
    public:
	    /** Origin of the bounding box. */
	    FVector Origin;
        
	    /** Extents of the bounding box. */
	    FVector Extents;

        /**
         * Stores the result of serializing or
         * de-serializing an RpcArchive.
         */
        HRESULT Result;

    public:
        /**
         * Initializes the bounding box components to zero.
         */
        FRpcBoundingBox();

        /**
         * Initializes bounding box with the provided data.
         */
        FRpcBoundingBox(const FVector& BoxOrigin, const FVector& BoxExtents);

        /**
         * Returns an RpcArchive created with this instance's data.
         */
        operator Rpc::Server::RpcArchive();
    };

}}}}

#endif // ENABLE_GAMESTEST_RPC
