//------------------------------------------------------------------------------
// <copyright file="UnrealEngineEvents.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "UnrealEngineEvents.h"
#include "../UnrealServiceContainer.h"
#include "RpcHookService.h"
#include "RpcArchive.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

using namespace GamesTest::Rpc::Server;

bool bEventsRegistered = false;
UTF_ANNOTATE_HRESULT_METHOD HRESULT RegisterEngineEvents()
{
    if (!bEventsRegistered)
    {
        FCoreUObjectDelegates::PostLoadMapWithWorld.AddStatic(&PostLoadMap);
        bEventsRegistered = true;
        return S_OK;
    }

    return UTF_E_RPC_EVENTS_ALREADY_REGISTERED;
}

void PostLoadMap(UWorld* World)
{
    if (World)
    {
        RpcArchive EventArgs;
        EventArgs.SetNamedStringValue(TEXT("MapName"), *(World->GetMapName()));
        EventArgs.SetNamedStringValue(TEXT("MapUrl"), *(World->URL.ToString()));

        UnrealToolsServicePtr Service = FUnrealServiceContainer::GetInstance().GetService(EUTFServiceType::HookService);
        if (Service.IsValid())
        {
            TSharedPtr<FRpcHookService> HookService = StaticCastSharedPtr<FRpcHookService>(Service);
            HRESULT Result = HookService->RaiseEvent(TEXT("MapLoadedEvent"), EventArgs);
            if (FAILED(Result))
            {
                UE_LOG(LogUToolsFramework, Warning, TEXT("Failed to raise MapLoadedEvent. Error: %X"), Result);
            }
        }
        else
        {
            UE_LOG(LogUToolsFramework, Warning, TEXT("Raised MapLoadedEvent when hook service wasn't initialized."));
        }
    }
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
