//------------------------------------------------------------------------------
// <copyright file="RpcHookCode.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#ifdef ENABLE_GAMESTEST_RPC

// The facility in an HRESULT code represents the source of the failure.
// Custom facility codes can have values: 257-4095.
// We are using a facility code of 500 for use with hooks.
#define RPC_HOOK_FACILITY 500

// This macro is provided to create the return code of a hook. It helps
// ensure system or TDK RPC HRESULTs aren't overridden.
// The provided code will be OR'd with a severity bit of 1, indicating
// failure, and a custom facility number in order to create an HRESULT.
#define MAKE_HOOK_ERROR(code)	MAKE_HRESULT(1,RPC_HOOK_FACILITY,code)


// Example of error code creation using the maximum allowed code. Min is 0.
// #define MY_CUSTOM_ERROR_EXAMPLE MAKE_HOOK_ERROR(65535)

#define BA_E_LOADING_SCREEN_ACTIVE MAKE_HOOK_ERROR(1)


#endif // ENABLE_GAMESTEST_RPC
