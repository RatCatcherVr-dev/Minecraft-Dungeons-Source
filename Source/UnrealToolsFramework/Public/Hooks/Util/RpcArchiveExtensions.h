//------------------------------------------------------------------------------
// <copyright file="RpcArchiveExtensions.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcServer.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

/**
 * Gets a string out of an archive.
 * @param FieldName - Name of the field holding the string.
 * @param Archive - Archive holding the string.
 * @param OutString - String that was obtained.
 * @return A code indicating either success or failure.
 */
	UNREALTOOLSFRAMEWORK_API Rpc::Server::RpcHookCode GetStringValue(const wchar_t* FieldName, const Rpc::Server::RpcArchive& Archive, FString* OutString);

/**
 * Gets a string collection out of an archive.
 * @typeparam StringType - The output array string type.
 * @typeparam AllocatorType - The output array allocator.
 * @param FieldName - Name of the field holding the string.
 * @param Archive - Archive holding the string collection.
 * @param OutStrings - String collection that was obtained
 * @return A code indicating either success or failure.
 */
	template<typename StringType, typename AllocatorType>
	UNREALTOOLSFRAMEWORK_API Rpc::Server::RpcHookCode GetStringCollection(const wchar_t* FieldName, const Rpc::Server::RpcArchive& Archive, TArray<StringType, AllocatorType>& OutStrings);

/**
 * Sets a string collection in an archive.
 * @typeparam AllocatorType - The input array allocator.
 * @param FieldName - Name of the field holding the string.
 * @param Archive - Archive to store the collection.
 * @param StringCollection - String collection that will be stored in the archive
 * @return A code indicating either success or failure.
 */
	template<typename AllocatorType>
	UNREALTOOLSFRAMEWORK_API Rpc::Server::RpcHookCode SetStringCollection(const wchar_t* FieldName, Rpc::Server::RpcArchive& Archive, const TArray<FString, AllocatorType>& StringCollection);
}}}}

#endif // ENABLE_GAMETEST_RPC
