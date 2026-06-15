//------------------------------------------------------------------------------
// <copyright file="UTFHResult.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"

/**
 * The facility in an HRESULT code represents its origin.
 * Custom facility codes can have values: 257-4095.
 * We are using a facility code of 500 for use with UTF.
 */
#define UTF_FACILITY 500

/**
 * This macro is provided to create custom error return codes. It helps
 * ensure other HRESULTs aren't overridden.
 * The provided code will be OR'd with a severity indicating failure
 * and a custom facility number in order to create an HRESULT.
 */
#define MAKE_UTF_ERROR(code)	MAKE_HRESULT(SEVERITY_ERROR,UTF_FACILITY,code)

/**
 * Example of error code creation using the maximum allowed code. Min is 0.
 * See also: UTFReturnCodes.h
 * #define MY_CUSTOM_ERROR_EXAMPLE MAKE_UTF_ERROR(65535)
 */

/**
 * Similar to the above, this macro creates return codes with the
 * difference of them indicating success.
 */
#define MAKE_UTF_SUCCESS(code)  MAKE_HRESULT(SEVERITY_SUCCESS,UTF_FACILITY,code)
