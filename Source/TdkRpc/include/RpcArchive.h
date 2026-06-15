//------------------------------------------------------------------------------
// <copyright file="RpcArchive.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#ifdef ENABLE_GAMESTEST_RPC

	#ifdef _WINRT_DLL

		// Using WinRT Component DLL configuration
		#include "RpcArchiveRt.h"

	#else

		// Using Native Static LIB configuration
		#include "RpcArchiveNative.h"

#endif

#endif // ENABLE_GAMESTEST_RPC