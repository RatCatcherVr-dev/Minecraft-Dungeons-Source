// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#if !defined(__cplusplus)
   #error C++11 required
#endif

#pragma once

extern "C"
{

/// <summary>
/// Creates an XblContextHandle used to access Xbox Live services.
/// </summary>
/// <param name="user">The user associated with this Xbox Live context.</param>
/// <param name="context">The returned Xbox Live context handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblContextCreateHandle(
    _In_ XblUserHandle user,
    _Out_ XblContextHandle* context
) XBL_NOEXCEPT;

/// <summary>
/// Duplicates the XblContextHandle.
/// </summary>
/// <param name="xboxLiveContextHandle">The Xbox Live context handle.</param>
/// <param name="duplicatedHandle">The duplicated handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// Use this method rather than creating a new context with the same user 
/// if the XblContextHandle is needed by multiple threads.
/// </remarks>
STDAPI XblContextDuplicateHandle(
    _In_ XblContextHandle xboxLiveContextHandle,
    _Out_ XblContextHandle* duplicatedHandle
) XBL_NOEXCEPT;

/// <summary>
/// Closes the XblContextHandle.
/// </summary>
/// <param name="xboxLiveContextHandle">The Xbox Live context handle.</param>
/// <returns></returns>
/// <remarks>
/// When all outstanding handles have been closed, XblContextCloseHandle() 
/// will free the memory associated with the handle.
/// </remarks>
STDAPI_(void) XblContextCloseHandle(
    _In_ XblContextHandle xboxLiveContextHandle
) XBL_NOEXCEPT;

/// <summary>
/// Get the XblContextHandle associated with this context.
/// </summary>
/// <param name="context">The Xbox Live context handle.</param>
/// <param name="user">The returned Xbox Live user handle.</param>
/// <returns>HRESULT return code for this API operation.</returns>
/// <remarks>
/// XSAPI will call XalUserDuplicateHandle before returning the handle, 
/// so it the responsibility of the caller to later call XalUserCloseHandle.
/// </remarks>
STDAPI XblContextGetUser(
    _In_ XblContextHandle context,
    _Out_ XblUserHandle* user
) XBL_NOEXCEPT;

/// <summary>
/// Get the Xbox user ID of the user associated with the context.
/// </summary>
/// <param name="context">The Xbox Live context handle.</param>
/// <param name="xboxUserId">The returned xbox user ID.</param>
/// <returns>HRESULT return code for this API operation.</returns>
STDAPI XblContextGetXboxUserId(
    _In_ XblContextHandle context,
    _Out_ uint64_t* xboxUserId
) XBL_NOEXCEPT;

}