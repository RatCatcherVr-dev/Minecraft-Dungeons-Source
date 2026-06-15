//------------------------------------------------------------------------------
// <copyright file="RpcVector.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcVector.h"
#include "RpcArchive.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

FRpcVector::FRpcVector()
{
}


FRpcVector::FRpcVector(const FVector& Vector)
    : FVector(Vector)
{
}


FRpcVector::FRpcVector(const Rpc::Server::RpcArchive& Data)
{
    HRESULT TempResult;
    this->Result = S_OK;

    HRCHKTHIS(Data.GetNamedValue(L"X", &this->X))
    HRCHKTHIS(Data.GetNamedValue(L"Y", &this->Y))
    HRCHKTHIS(Data.GetNamedValue(L"Z", &this->Z))
}


FRpcVector::operator Rpc::Server::RpcArchive()
{
    Rpc::Server::RpcArchive Output;
    HRESULT TempResult;
    this->Result = S_OK;

    HRCHKTHIS(Output.SetNamedValue(L"X", this->X))
    HRCHKTHIS(Output.SetNamedValue(L"Y", this->Y))
    HRCHKTHIS(Output.SetNamedValue(L"Z", this->Z))

    return Output;
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
