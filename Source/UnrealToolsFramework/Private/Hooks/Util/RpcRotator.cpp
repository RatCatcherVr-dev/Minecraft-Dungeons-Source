//------------------------------------------------------------------------------
// <copyright file="RpcRotator.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcRotator.h"
#include "RpcArchive.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

FRpcRotator::FRpcRotator()
{
}


FRpcRotator::FRpcRotator(const FRotator& Rotator)
    : FRotator(Rotator)
{
}


FRpcRotator::FRpcRotator(const Rpc::Server::RpcArchive& Data)
{
    HRESULT TempResult;
    this->Result = S_OK;

    HRCHKTHIS(Data.GetNamedValue(L"Pitch", &this->Pitch))
    HRCHKTHIS(Data.GetNamedValue(L"Yaw", &this->Yaw))
    HRCHKTHIS(Data.GetNamedValue(L"Roll", &this->Roll))
}


FRpcRotator::operator Rpc::Server::RpcArchive()
{
    Rpc::Server::RpcArchive Output;
    HRESULT TempResult;
    this->Result = S_OK;

    HRCHKTHIS(Output.SetNamedValue(L"Pitch", this->Pitch))
    HRCHKTHIS(Output.SetNamedValue(L"Yaw", this->Yaw))
    HRCHKTHIS(Output.SetNamedValue(L"Roll", this->Roll))

    return Output;
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
