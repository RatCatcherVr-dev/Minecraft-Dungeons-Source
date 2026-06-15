//------------------------------------------------------------------------------
// <copyright file="RpcRotator.h" company="Microsoft">
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
     * Simple class representing rotation in degrees.
     */
    class UNREALTOOLSFRAMEWORK_API FRpcRotator : public FRotator
    {
    public:
        /**
         * Stores the result of serializing or
         * de-serializing an RpcArchive.
         */
        HRESULT Result;

    public:
        /**
         * Initializes components to zero.
         */
        FRpcRotator();

        /**
         * Initializes components using the components from the FRotator.
         */
        FRpcRotator(const FRotator& Rotator);

        /**
         * Initializes components using the provided data.
         */
        FRpcRotator(const Rpc::Server::RpcArchive& Data);

        /**
         * Returns an RpcArchive created with this instance's data.
         */
        operator Rpc::Server::RpcArchive();
    };

}}}}

#endif // ENABLE_GAMESTEST_RPC
