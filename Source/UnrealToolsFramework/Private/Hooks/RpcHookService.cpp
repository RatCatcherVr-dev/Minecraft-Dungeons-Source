//------------------------------------------------------------------------------
// <copyright file="RpcHookService.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcHookService.h"
#include "RpcArchive.h"
#include "UnrealEngineEvents.h"
#include "UnrealEngineHooks.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

using namespace GamesTest::Rpc::Server;

FRpcHookService::FRpcHookService()
{
}
     

UTF_ANNOTATE_HRESULT_METHOD HRESULT FRpcHookService::Initialize()
{
    if (!this->RpcServerPtr.IsValid())
    {
        HRESULT Result = S_OK;

        this->RpcServerPtr = MakeShareable(new RpcServer());
		int port = this->RpcServerPtr->GetPort();
        HRCHK(this->RegisterEngineHooks())
        HRCHK(this->RegisterOnInitHooks())
        return RegisterEngineEvents();
    }

    return UTF_E_RPC_SERVICE_ALREADY_INITIALIZED;
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT FRpcHookService::Initialize(int Port)
{
    if (!this->RpcServerPtr.IsValid())
    {
        HRESULT Result = S_OK;

        this->RpcServerPtr = MakeShareable(new RpcServer(Port));
        HRCHK(this->RegisterEngineHooks())
        HRCHK(this->RegisterOnInitHooks())
        return RegisterEngineEvents();
    }

    return UTF_E_RPC_SERVICE_ALREADY_INITIALIZED;
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT FRpcHookService::Start()
{
    if (this->RpcServerPtr.IsValid())
    {
        return this->RpcServerPtr->Start();
    }

    return UTF_E_RPC_SERVICE_NOT_INITIALIZED;
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT FRpcHookService::Tick(float DeltaTime)
{
    if (this->RpcServerPtr.IsValid())
    {
        return this->RpcServerPtr->ProcessPendingRequests();
    }

    return UTF_E_RPC_SERVICE_NOT_INITIALIZED;
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT FRpcHookService::Stop()
{
    if (this->RpcServerPtr.IsValid())
    {
        HRESULT Result = this->RpcServerPtr->Stop();
        this->RpcServerPtr.Reset();
        return Result;
    }

    return UTF_S_RPC_SERVICE_NOT_INITIALIZED;
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT FRpcHookService::RegisterHookMethod(const wchar_t* HookName, Rpc::Server::RpcHook Hook)
{
    if (this->RpcServerPtr.IsValid())
    {
        return this->RpcServerPtr->RegisterHookMethod(HookName, Hook);
    }

    return UTF_E_RPC_SERVICE_NOT_INITIALIZED;
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT FRpcHookService::RaiseEvent(const wchar_t* EventName, const Rpc::Server::RpcArchive &EventArgs)
{
    if (this->RpcServerPtr.IsValid())
    {
        return this->RpcServerPtr->RaiseEvent(EventName, EventArgs);
    }

    return UTF_E_RPC_SERVICE_NOT_INITIALIZED;
}

// Registers a series of common Unreal Engine hooks.
UTF_ANNOTATE_HRESULT_METHOD HRESULT FRpcHookService::RegisterEngineHooks()
{
    HRESULT Result = S_OK;

    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetCurrentMapData"), GetCurrentMapData))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("LoadMap"), LoadMap))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetEnumValues"), GetEnumValues))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("ExecuteCommand"), ExecuteCommand))
	HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetAutoCompleteCommands"), GetAutoCompleteCommands))

    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetActors"), GetActors))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetActorById"), GetActorById))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetActorLocation"), GetActorLocation))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("SetActorLocation"), SetActorLocation))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetActorCanBeDamaged"), GetActorCanBeDamaged))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("SetActorCanBeDamaged"), SetActorCanBeDamaged))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("DestroyActor"), DestroyActor))
	HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetObjectProperties"), GetObjectProperties))
	HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetActorProperties"), GetActorProperties))

    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetPawns"), GetPawns))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetPawnById"), GetPawnById))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetPawnIsControlled"), GetPawnIsControlled))
    
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetLocalPlayers"), GetLocalPlayers))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetLocalPlayerByIndex"), GetLocalPlayerByIndex))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetPlayerControllerId"), GetPlayerControllerId))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetPlayerLocation"), GetPlayerLocation))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("SetPlayerLocation"), SetPlayerLocation))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetPlayerRotation"), GetPlayerRotation))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("SetPlayerRotation"), SetPlayerRotation))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("TeleportPlayer"), TeleportPlayer))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("GetPlayerCanBeDamaged"), GetPlayerCanBeDamaged))
    HRCHK(this->RpcServerPtr->RegisterHookMethod(TEXT("SetPlayerCanBeDamaged"), SetPlayerCanBeDamaged))

    return Result;
}

// Register game hooks that were requested before calling Initialize.
UTF_ANNOTATE_HRESULT_METHOD HRESULT FRpcHookService::RegisterOnInitHooks()
{
    HRESULT Result = S_OK;

    for (HookDetails& Hook : FUTFHookManager::HooksToRegisterOnInit)
    {
        HRCHK(this->RpcServerPtr->RegisterHookMethod(*Hook.HookName, Hook.HookMethod))
    }

    FUTFHookManager::HooksToRegisterOnInit.Empty();
    return Result;
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
