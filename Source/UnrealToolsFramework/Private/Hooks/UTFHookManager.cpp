//------------------------------------------------------------------------------
// <copyright file="UTFHookManager.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"
#include "../UnrealServiceContainer.h"
#include "RpcHookService.h"

#ifdef ENABLE_GAMESTEST_RPC

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

TArray<HookDetails> FUTFHookManager::HooksToRegisterOnInit;


UTF_ANNOTATE_HRESULT_METHOD HRESULT FUTFHookManager::RegisterGameHook(const wchar_t* HookName, Rpc::Server::RpcHook Hook)
{
    UnrealToolsServicePtr Service = FUnrealServiceContainer::GetInstance().GetService(EUTFServiceType::HookService);
    if (Service.IsValid())
    {
        TSharedPtr<FRpcHookService> HookService = StaticCastSharedPtr<FRpcHookService>(Service);
        return HookService->RegisterHookMethod(HookName, Hook);
    }
    else
    {
        // Keep list of hooks to register on service init.
        FUTFHookManager::HooksToRegisterOnInit.Emplace(HookDetails(HookName, Hook));
        return S_OK;
    }
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT FUTFHookManager::RaiseGameEvent(const wchar_t* EventName, const Rpc::Server::RpcArchive &EventArgs)
{
    UnrealToolsServicePtr Service = FUnrealServiceContainer::GetInstance().GetService(EUTFServiceType::HookService);
    if (Service.IsValid())
    {
        TSharedPtr<FRpcHookService> HookService = StaticCastSharedPtr<FRpcHookService>(Service);
        return HookService->RaiseEvent(EventName, EventArgs);
    }

    return UTF_E_RPC_SERVICE_NOT_INITIALIZED;
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
