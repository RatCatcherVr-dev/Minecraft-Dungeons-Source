//------------------------------------------------------------------------------
// <copyright file="UnrealToolsFramework.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

    /** Declare a stats group which is enabled by default for the UnrealToolsFrameworkModule. */
    DECLARE_STATS_GROUP(TEXT("UnrealToolsFramework"), STATGROUP_UnrealToolsFramework, STATCAT_Advanced);

    /** Main manager for the Unreal Tools Framework (external interface). */
    class UNREALTOOLSFRAMEWORK_API FUnrealToolsFramework
    {
    public:
        /**
         * Starts the various tools in UTF.
         * @return A code indicating either success or failure.
         */
        static UTF_ANNOTATE_HRESULT_METHOD HRESULT Start();

        /**
         * Update function, should be called on every game tick.
         * @param DeltaTime - Time elapsed since last tick in seconds.
         * @return A code indicating either success or failure.
         */
        static UTF_ANNOTATE_HRESULT_METHOD HRESULT Tick(float DeltaTime);

        /**
         * Stop all the tools that were started.
         * @return A code indicating either success or failure.
         */
        static UTF_ANNOTATE_HRESULT_METHOD HRESULT Stop();

    public: // Hook Service

        /**
         * Creates a hook service and registers it with UTF.
         * On Xbox One, it uses one of the 2 debug ports provided.
         * IMPORTANT: Make sure you call it before calling Start().
         * @return A code indicating either success or failure.
         */
        static UTF_ANNOTATE_HRESULT_METHOD HRESULT RegisterHookService();

        /**
         * Creates a hook service and registers it with UTF.
         * IMPORTANT: Make sure you call it before calling Start().
         * @param Port - Port to start the hook service on.
         * @return A code indicating either success or failure.
         */
        static UTF_ANNOTATE_HRESULT_METHOD HRESULT RegisterHookService(int Port);
    };

}}}}

// Declare log category for UTF.
UNREALTOOLSFRAMEWORK_API DECLARE_LOG_CATEGORY_EXTERN(LogUToolsFramework, Log, All);
