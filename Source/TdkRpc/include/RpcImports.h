//------------------------------------------------------------------------------
// <copyright file="RpcImports.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

//
// Instructs linker to use socket import libraries, but only for users of this static library, not the library itself.
// This fixes LNK4006 and LNK4221 warnings resulting from static library directly using import libraries.
//

#ifdef ENABLE_GAMESTEST_RPC
#ifndef _WINRT_DLL

	#ifndef RPCLIB

		#ifdef _DURANGO
			// For Durango platform, link to Winsock
			#pragma comment (lib, "Ws2_32.lib")
		#else
			#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
				// For Win32 platform, link to Winsock
				#pragma comment (lib, "Ws2_32.lib")
			#else
				// For WinRT platform, link to WinRT sockets
				#pragma comment (lib, "Rpcrt4.lib")
			#endif
		#endif

	#endif // RPCLIB

#endif // _WINRT_DLL
#endif // ENABLE_GAMESTEST_RPC