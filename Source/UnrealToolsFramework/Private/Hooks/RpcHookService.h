//------------------------------------------------------------------------------
// <copyright file="RpcHookService.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "../IUnrealToolsService.h"
#include "RpcServer.h"

/** Forward declaration of RpcArchive. */
namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server {
    class RpcArchive;
}}}}}

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

    /** Class that handles the hook implementation and communication layer. */
    class FRpcHookService : public IUnrealToolsService
    {
    public:
        /** Creates a new instance of the hook service class. */
        FRpcHookService();

        /**
         * Initializes the hook service. Should be called before Start.
         * On Xbox One, it uses one of the 2 debug ports provided.
         * @return A code indicating either success or failure.
         */
        UTF_ANNOTATE_HRESULT_METHOD HRESULT Initialize();
        
        /**
         * Initializes the hook service. Should be called before Start.
         * @param Port - Port to start the RPC Service on.
         * @return A code indicating either success or failure.
         */
        UTF_ANNOTATE_HRESULT_METHOD HRESULT Initialize(int Port);

        /**
         * Register a new hook.
         * @param HookName - Unique name for the hook to register.
         * @param Hook - Function to register.
         * @return A code indicating either success or failure.
         */
        UTF_ANNOTATE_HRESULT_METHOD HRESULT RegisterHookMethod(const wchar_t* HookName, Rpc::Server::RpcHook Hook);

        /**
         * Raises an event.
         * @param EventName - Unique name of the event to raise.
         * @param EventArgs - Arguments to include with the raised event.
         * @return A code indicating either success or failure.
         */
        UTF_ANNOTATE_HRESULT_METHOD HRESULT RaiseEvent(const wchar_t* EventName, const Rpc::Server::RpcArchive &EventArgs);
    
    public: // IUnrealToolsService

        /**
         * Starts the hook service. Should be called at engine startup.
         * @return A code indicating either success or failure.
         */
        virtual UTF_ANNOTATE_HRESULT_METHOD HRESULT Start() override;

        /**
         * Update function, should be called on every game tick.
         * @param DeltaTime - Time elapsed since last tick in seconds.
         * @return A code indicating either success or failure.
         */
        virtual UTF_ANNOTATE_HRESULT_METHOD HRESULT Tick(float DeltaTime) override;

        /**
         * Stops the hook service. Should be called at engine shutdown.
         * @return A code indicating either success or failure.
         */
        virtual UTF_ANNOTATE_HRESULT_METHOD HRESULT Stop() override;

    private:
        /**
         * Register a series of common Unreal Engine hooks.
         * @return A code indicating either success or failure.
         */
        UTF_ANNOTATE_HRESULT_METHOD HRESULT RegisterEngineHooks();

        /**
         * Register game hooks that were requested before calling Initialize.
         * @return A code indicating either success or failure.
         */
        UTF_ANNOTATE_HRESULT_METHOD HRESULT RegisterOnInitHooks();

    private:
        /** TDK RPC Server for hooks. */
        TSharedPtr<Rpc::Server::RpcServer> RpcServerPtr;
    };

}}}}

#endif // ENABLE_GAMESTEST_RPC
