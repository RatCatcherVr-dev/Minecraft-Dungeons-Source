//------------------------------------------------------------------------------
// <copyright file="IUnrealToolsService.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

    /**
     * Interface to be used by Unreal Tools Services such
     * as the hook service.
     */
    class IUnrealToolsService
    {
    public:
        /**
         * Starts the UTF service.
         * @return A code indicating either success or failure.
         */
        virtual UTF_ANNOTATE_HRESULT_METHOD HRESULT Start() = 0;

        /**
         * Update function, will be called on every game tick.
         * @param DeltaTime - Time elapsed since last tick in seconds.
         * @return A code indicating either success or failure.
         */
        virtual UTF_ANNOTATE_HRESULT_METHOD HRESULT Tick(float DeltaTime) = 0;

        /**
         * Stop the service.
         * @return A code indicating either success or failure.
         */
        virtual UTF_ANNOTATE_HRESULT_METHOD HRESULT Stop() = 0;
    };

}}}}
