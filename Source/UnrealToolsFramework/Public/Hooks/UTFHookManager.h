//------------------------------------------------------------------------------
// <copyright file="UTFHookManager.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcServer.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

    /**
     * Structure used for delayed initialization.
     * Contains the hook's name and associated method.
     */
    struct HookDetails
    {
        /** Initializes an instance of the HookDetails struct. */
        HookDetails(FString Name, Rpc::Server::RpcHook Hook)
        {
            this->HookName = Name;
            this->HookMethod = Hook;
        }

        /** Name of the hook. */
        FString HookName;

        /** Method associated to the hook. */
        Rpc::Server::RpcHook HookMethod;
    };

    /**
     * Class than handles the communication between the game and
     * the hook service.
     */
    class UNREALTOOLSFRAMEWORK_API FUTFHookManager
    {
    public:
        /** 
         * Registers a game-specific hook with the hook service.
         * @param HookName - Unique name for the hook to register.
         * @param Hook - Function to register.
         * @return A code indicating either success or failure.
         */
        static UTF_ANNOTATE_HRESULT_METHOD HRESULT RegisterGameHook(const wchar_t* HookName, Rpc::Server::RpcHook Hook);

        /**
         * Raises a game-specific event using the hook service.
         * @param EventName - Unique name of the event to raise.
         * @param EventArgs - Arguments to include with the raised event.
         * @return A code indicating either success or failure.
         */
        static UTF_ANNOTATE_HRESULT_METHOD HRESULT RaiseGameEvent(const wchar_t* EventName, const Rpc::Server::RpcArchive &EventArgs);

    private:
        /** List of hooks to register on initialize. */
        static TArray<HookDetails> HooksToRegisterOnInit;

        /** Allow RpcHookService to access 'HooksToRegisterOnInit'. */
        friend class FRpcHookService;
    };

}}}}

#endif // ENABLE_GAMESTEST_RPC
