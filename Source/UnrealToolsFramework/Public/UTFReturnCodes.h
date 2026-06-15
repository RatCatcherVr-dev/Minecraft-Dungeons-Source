//------------------------------------------------------------------------------
// <copyright file="UTFErrorCodes.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFHResult.h"

/******************************   SUCCESS CODES   ******************************/

/** Code to be used when an attempt to register the hook service is made on a build where the service is defined out. */
#define UTF_S_RPC_SERVICE_NOT_ACTIVE MAKE_UTF_SUCCESS(0)

/**
 * Code to be used when a function is called without the RPC Service being initialized.
 * There is an equivalent error code below. This one is to be used when the call won't result in failure.
 */
#define UTF_S_RPC_SERVICE_NOT_INITIALIZED MAKE_UTF_SUCCESS(1)

/******************************   ERROR CODES   ******************************/

/** Macro to be used to check HRESULT and result on failure. */
#define HRCHK(expr) { Result = (expr); if(FAILED(Result)) { return Result; } }

/**
 * Macro to be used to check HRESULT.
 * This macro is specifically used when there is a cleanup section for allocated memory.
 */
#define HRCHKCLEANUP(expr) { Result = (expr); if(FAILED(Result)) { goto Cleanup; } }

/** 
 * Macro to be used to check HRESULT. HRESULT will be stored in a class member.
 * This macro is specifically used when serializing/de-serializing an object from an RpcArchive.
 */
#define HRCHKTHIS(exp) { TempResult = (exp); if(FAILED(TempResult)) { this->Result = TempResult; } }

/** Error to be used when an attempt to register an already registered service is made. */
#define UTF_E_SERVICE_ALREADY_REGISTERED MAKE_UTF_ERROR(0)

/** Error to be used when calling a method on several services resulted in total or partial failure. */
#define UTF_E_ONE_OR_MORE_SERVICE_CALLS_FAILED MAKE_UTF_ERROR(1)

/** Error to be used when an RPC Service already existed and Initialize is called again. */
#define UTF_E_RPC_SERVICE_ALREADY_INITIALIZED MAKE_UTF_ERROR(2)

/** Error to be used when a function is called without the RPC Service being initialized. */
#define UTF_E_RPC_SERVICE_NOT_INITIALIZED MAKE_UTF_ERROR(3)

/** Error to be used when event registration is called more than once. */
#define UTF_E_RPC_EVENTS_ALREADY_REGISTERED MAKE_UTF_ERROR(4)

/** Error to be used when trying to get a player objects gets a null pointer. */
#define UTF_E_PLAYER_WAS_NOT_FOUND MAKE_UTF_ERROR(5)

/** Error to be used when trying to get a required component, system, or object gets a null pointer. */
#define UTF_E_A_REQUIRED_ELEMENT_WAS_NOT_FOUND MAKE_UTF_ERROR(6)

/** Macro to be used to trigger the error code above. Should specify the name of the element that wasn't found. */
#define RETURN_ELEMNOTFND(elem) { Output.SetNamedStringValue(L"ElementNotFound", L#elem); return UTF_E_A_REQUIRED_ELEMENT_WAS_NOT_FOUND; }

/** Error to be used when the parameter of a hook has an invalid value. */
#define UTF_E_INVALID_INPUT_PARAMETER MAKE_UTF_ERROR(7)

/** Macro to be used to trigger the error code above. Should specify the message you want to see on the exception in the managed side. */
#define RETURN_INVALIDPARAM(msg) { Output.SetNamedStringValue(L"InvalidParamMsg", L#msg); return UTF_E_INVALID_INPUT_PARAMETER; }

/** Error to be used when there was an error returning all required elements from a collection. */
#define UTF_E_NOT_ALL_ELEMENTS_INCLUDED MAKE_UTF_ERROR(8)

/** 
 * Macro to be used to check HRESULT when storing values on a loop to warn people when an
 * element wasn't included instead of causing an error. This macro is designed to be used
 * before storing the value so the storing is skipped.
 */
#define HRCHKELEM(expr) { if(FAILED(expr)) { ElementsResult = UTF_E_NOT_ALL_ELEMENTS_INCLUDED; continue; } }

/** Error to be used when there wasn't enough memory available to allocate an object. */
#define UTF_E_CUSTOM_OUT_OF_MEMORY MAKE_UTF_ERROR(9)

/** Error to be used when a game or engine function returns failure. */
#define UTF_E_FUNCTION_FAILED MAKE_UTF_ERROR(10)

/** Macro to be used to trigger the error code above. Should specify the game/engine function that indicated failure. */
#define RETURN_FUNCFAILED(function) { Output.SetNamedStringValue(L"FailedFunction", L#function); return UTF_E_FUNCTION_FAILED; }

/** Error to be used when a console command cannot be executed. */
#define UTF_E_CONSOLE_COMMAND_NOT_EXECUTED MAKE_UTF_ERROR(11)

/** Error to be used when trying to get a player objects gets a null pointer. */
#define UTF_E_WORLD_WAS_NOT_FOUND MAKE_UTF_ERROR(12)
