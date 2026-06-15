//------------------------------------------------------------------------------
// <copyright file="UnrealServiceContainer.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"
#include "UnrealServiceContainer.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

FUnrealServiceContainer& FUnrealServiceContainer::GetInstance()
{
    static FUnrealServiceContainer Instance;
    return Instance;
}


FUnrealServiceContainer::FUnrealServiceContainer()
{
}


UTFServicesMap::TConstIterator FUnrealServiceContainer::ConstIterator() const
{
    return this->Services.CreateConstIterator();
}


UTFServicesMap::TIterator FUnrealServiceContainer::Iterator()
{
    return this->Services.CreateIterator();
}


UnrealToolsServicePtr FUnrealServiceContainer::GetService(EUTFServiceType Type) const
{
    return this->Services.FindRef(Type);
}


HRESULT FUnrealServiceContainer::AddService(EUTFServiceType Type, UnrealToolsServicePtr Service)
{
    if (this->GetService(Type).IsValid())
    {
        return UTF_E_SERVICE_ALREADY_REGISTERED;
    }

    this->Services.Add(Type, Service);
    return S_OK;
}


void FUnrealServiceContainer::Empty()
{
    this->Services.Empty();
}

}}}}
