//------------------------------------------------------------------------------
// <copyright file="RpcErrorRt.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#ifdef ENABLE_GAMESTEST_RPC
#ifdef _WINRT_DLL

namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server
{
	/// <summary> 
	/// RPC error codes.
	/// </summary>
	public ref class RpcError sealed
	{
	public:
		/// <summary> 
		/// E_FAIL
		/// </summary> 		
		static property int UnknownFailure
		{
			int get() { return E_FAIL; }
		}

		/// <summary> 
		/// E_INVALIDARG
		/// </summary> 
		static property int InvalidArg
		{
			int get() { return E_INVALIDARG; }
		}
			
		/// <summary> 
		/// GAMESTEST_RPC_E_METHOD_NOT_FOUND
		/// </summary> 
		static property int MethodNotFound
		{
			int get() { return GAMESTEST_RPC_E_METHOD_NOT_FOUND; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_S_NOT_STARTED
		/// </summary> 
		static property int ServerNotStarted
		{
			int get() { return GAMESTEST_RPC_S_NOT_STARTED; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_E_SERVER_ID_ALREADY_IN_USE
		/// </summary> 
		static property int ServerIdAlreadyInUse
		{
			int get() { return GAMESTEST_RPC_E_SERVER_ID_ALREADY_IN_USE; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_S_ALREADY_LISTENING
		/// </summary> 
		static property int ServerAlreadyListening
		{
			int get() { return GAMESTEST_RPC_S_ALREADY_LISTENING; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_E_UNKNOWN_SERVER_ID
		/// </summary> 
		static property int UnknownServerId
		{
			int get() { return GAMESTEST_RPC_E_UNKNOWN_SERVER_ID; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_W_FAILED_TO_SERIALIZE_USER_DATA
		/// </summary> 
		static property int FailedToSerializeUserData
		{
			int get() { return GAMESTEST_RPC_W_FAILED_TO_SERIALIZE_USER_DATA; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_E_CLIENT_DISCONNECTED
		/// </summary> 
		static property int ClientDisconnected
		{
			int get() { return GAMESTEST_RPC_E_CLIENT_DISCONNECTED; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_E_FAILED_TO_PARSE_RPC_SOCKET_REQUEST
		/// </summary> 
		static property int FailedToParseRpcSocketRequest
		{
			int get() { return GAMESTEST_RPC_E_FAILED_TO_PARSE_RPC_SOCKET_REQUEST; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_E_DESTINATION_BUFFER_TOO_SMALL
		/// </summary> 
		static property int DestinationBufferTooSmall
		{
			int get() { return GAMESTEST_RPC_E_DESTINATION_BUFFER_TOO_SMALL; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_E_ARCHIVE_VALUE_IS_NULL
		/// </summary> 
		static property int ArchiveValueIsNull
		{
			int get() { return GAMESTEST_RPC_E_ARCHIVE_VALUE_IS_NULL; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_E_MEMBER_NOT_FOUND_IN_ARCHIVE
		/// </summary> 
		static property int ArchiveMemberNotFound
		{
			int get() { return GAMESTEST_RPC_E_MEMBER_NOT_FOUND_IN_ARCHIVE; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_E_PENDING_REQUESTS_QUEUE_FULL
		/// </summary> 
		static property int PendingRequestsQueueFull
		{
			int get() { return GAMESTEST_RPC_E_PENDING_REQUESTS_QUEUE_FULL; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_I_PROCESS_PENDING_REQUEST_PARTIAL_FAILURE
		/// </summary> 
		static property int ProcessPendingRequestPartialFailure
		{
			int get() { return GAMESTEST_RPC_I_PROCESS_PENDING_REQUEST_PARTIAL_FAILURE; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_E_PROCESS_PENDING_REQUEST_FAILURE
		/// </summary> 
		static property int ProcessPendingRequestFailure
		{
			int get() { return GAMESTEST_RPC_E_PROCESS_PENDING_REQUEST_FAILURE; }
		}

		/// <summary> 
		/// GAMESTEST_RPC_E_VERSION_MISMATCH
		/// </summary> 
		static property int VersionMismatch
		{
			int get() { return GAMESTEST_RPC_E_VERSION_MISMATCH; }
		}

		/// <summary>
		/// Get friendly error description from HRESULT code.
		/// </summary>
		/// <param name="errorCode">Error code from Platform:COMException thrown by RpcArchive.</param>
		static Platform::String^ GetDescription(int errorCode);

		/// <summary>
		/// Get friendly error description from HRESULT code.
		/// </summary>
		/// <param name="userDefinedErrorCode">User defined error code to turn into HRESULT with "RPC Hook" facility.</param>
		static int MakeHookError(int userDefinedErrorCode);
	};

}}}}}

#endif // _WINRT_DLL
#endif // ENABLE_GAMESTEST_RPC