//------------------------------------------------------------------------------
// <copyright file="UnrealToolsServiceTypes.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"
#include "UnrealToolsServiceTypes.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

FString GetServiceTypeName(EUTFServiceType Type)
{
    switch (Type)
    {
    case EUTFServiceType::HookService:
        return FString("HookService");
    default:
        return FString("Unknown");
    }
}

}}}}
