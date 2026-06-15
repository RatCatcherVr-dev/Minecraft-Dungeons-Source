//------------------------------------------------------------------------------
// <copyright file="UnrealToolsServiceTypes.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

    /** Types of service. */
    enum EUTFServiceType
    {
        HookService = 0
    };

    /**
     * Returns a string representation of the type.
     * @param Type - Type to get the name for.
     */
    FString GetServiceTypeName(EUTFServiceType Type);

}}}}
