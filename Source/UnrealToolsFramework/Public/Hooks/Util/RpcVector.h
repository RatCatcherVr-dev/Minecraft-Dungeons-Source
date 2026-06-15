//------------------------------------------------------------------------------
// <copyright file="RpcVector.h" company="Microsoft">
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
     * Class extending FVector for hook usage.
     */
    class UNREALTOOLSFRAMEWORK_API FRpcVector : public FVector
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
        FRpcVector();

        /**
         * Initializes components using the components from the FVector.
         */
        FRpcVector(const FVector& Vector);

        /**
         * Initializes components using the provided data.
         */
        FRpcVector(const Rpc::Server::RpcArchive& Data);

        /**
         * Returns an RpcArchive created with this instance's data.
         */
        operator Rpc::Server::RpcArchive();
    };

}}}}

#endif // ENABLE_GAMESTEST_RPC
