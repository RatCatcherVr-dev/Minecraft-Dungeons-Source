//------------------------------------------------------------------------------
// <copyright file="UnrealToolsFramework.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"
#include "IUnrealToolsService.h"
#include "UnrealServiceContainer.h"
#include "Hooks/RpcHookService.h"

/** Defining the log category for UTF. */
DEFINE_LOG_CATEGORY(LogUToolsFramework);

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

UTF_ANNOTATE_HRESULT_METHOD HRESULT FUnrealToolsFramework::Start()
{
    DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UnrealToolsFramework_Start"),STAT_UnrealToolsFramework_Start,STATGROUP_UnrealToolsFramework);

    HRESULT Result = S_OK;

    for (UTFServicesMap::TIterator Service = FUnrealServiceContainer::GetInstance().Iterator(); Service; ++Service)
    {
        HRESULT ServiceResult = Service->Value->Start();
        if (FAILED(ServiceResult))
        {
            UE_LOG(LogUToolsFramework, Error, TEXT("Unable to start UTF service of type %s. Error: %X"), *GetServiceTypeName(Service->Key), ServiceResult);
            Result = UTF_E_ONE_OR_MORE_SERVICE_CALLS_FAILED;
        }
    }

    return Result;
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT FUnrealToolsFramework::Tick(float DeltaTime)
{
    DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UnrealToolsFramework_Tick"),STAT_UnrealToolsFramework_Tick,STATGROUP_UnrealToolsFramework);

    HRESULT Result = S_OK;

    for (UTFServicesMap::TIterator Service = FUnrealServiceContainer::GetInstance().Iterator(); Service; ++Service)
    {
        HRESULT ServiceResult = Service->Value->Tick(DeltaTime);
        if (FAILED(ServiceResult))
        {
            UE_LOG(LogUToolsFramework, Error, TEXT("Unable to tick UTF service of type %s. Error: %X"), *GetServiceTypeName(Service->Key), ServiceResult);
            Result = UTF_E_ONE_OR_MORE_SERVICE_CALLS_FAILED;
        }
    }

    return Result;
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT FUnrealToolsFramework::Stop()
{
    DECLARE_SCOPE_CYCLE_COUNTER(TEXT("UnrealToolsFramework_Stop"),STAT_UnrealToolsFramework_Stop,STATGROUP_UnrealToolsFramework);

    HRESULT Result = S_OK;

    for (UTFServicesMap::TIterator Service = FUnrealServiceContainer::GetInstance().Iterator(); Service; ++Service)
    {
        HRESULT ServiceResult = Service->Value->Stop();
        if (FAILED(ServiceResult))
        {
            UE_LOG(LogUToolsFramework, Error, TEXT("Unable to stop UTF service of type %s. Error: %X"), *GetServiceTypeName(Service->Key), ServiceResult);
            Result = UTF_E_ONE_OR_MORE_SERVICE_CALLS_FAILED;
        }
    }

    FUnrealServiceContainer::GetInstance().Empty();
    return Result;
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT FUnrealToolsFramework::RegisterHookService()
{
    HRESULT Result = S_OK;

#ifdef ENABLE_GAMESTEST_RPC
    TSharedPtr<FRpcHookService> HookService = MakeShareable(new FRpcHookService());
    HRCHK(FUnrealServiceContainer::GetInstance().AddService(EUTFServiceType::HookService, HookService))
    return HookService->Initialize();
#endif

    return UTF_S_RPC_SERVICE_NOT_ACTIVE;
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT FUnrealToolsFramework::RegisterHookService(int Port)
{
    HRESULT Result = S_OK;

#ifdef ENABLE_GAMESTEST_RPC
    TSharedPtr<FRpcHookService> HookService = MakeShareable(new FRpcHookService());
    HRCHK(FUnrealServiceContainer::GetInstance().AddService(EUTFServiceType::HookService, HookService))
    return HookService->Initialize(Port);
#endif

    return UTF_S_RPC_SERVICE_NOT_ACTIVE;
}

}}}}
