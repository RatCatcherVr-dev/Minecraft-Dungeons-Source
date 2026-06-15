//------------------------------------------------------------------------------
// <copyright file="RpcBoundingBox.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcBoundingBox.h"
#include "RpcArchive.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

FRpcBoundingBox::FRpcBoundingBox()
{
}


FRpcBoundingBox::FRpcBoundingBox(const FVector& BoxOrigin, const FVector& BoxExtents)
{
    this->Origin = BoxOrigin;
    this->Extents = BoxExtents;
}


FRpcBoundingBox::operator Rpc::Server::RpcArchive()
{
    Rpc::Server::RpcArchive Output;
    HRESULT TempResult;
    this->Result = S_OK;

    FRpcVector BoxOrigin(this->Origin);
    HRCHKTHIS(Output.SetNamedValue(L"Origin", BoxOrigin))
    HRCHKTHIS(BoxOrigin.Result)

    FRpcVector BoxExtents(this->Extents);
    HRCHKTHIS(Output.SetNamedValue(L"Extents", BoxExtents))
    HRCHKTHIS(BoxExtents.Result)

    return Output;
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
