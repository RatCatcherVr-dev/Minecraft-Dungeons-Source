//------------------------------------------------------------------------------
// <copyright file="RpcServerNative.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#ifndef _WINRT_DLL

#include <rpc.h>

#include "RpcImports.h"
#include "RpcAnnotations.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server
{
	class RpcArchive;
	class RpcServerImpl;

	/// <summary> 
	/// Used to enforce the use of our custom macro for hook return codes.
	/// Macro can be found under RpcHookCode.h
	/// </summary> 
	typedef HRESULT RpcHookCode;

	/// <summary> 
	/// Provides the definition of a hook in the TDK RPC system.
	/// </summary>
	typedef RpcHookCode (*RpcHook)(RpcArchive& input, RpcArchive& output);

	/// <summary> 
	/// Entry point into the server-side of the TDK RPC library.
	/// The RpcServer class is used to start and stop servers and register
	/// hook methods that are exposed to remote clients.
	/// </summary>
	class RpcServer
	{
	public:
		/// <summary> 
		/// Initializes a new instance of the RpcServer class.
		/// </summary>
		RpcServer();

		/// <summary> 
		/// Initializes a new instance of the RpcServer class with specific port number.
		/// <param name="port">A specific port to listen on.</param>
		/// </summary>
		RpcServer(int port);

		/// <summary> 
		/// Initializes a new instance of the RpcServer class.
		/// <param name="serverId">A unique identifier for this server instance.
		/// The same identifier should be used on the client to connect to this server.</param>
		/// </summary>
		RpcServer(const GUID& serverId);

		/// <summary> 
		/// Initializes a new instance of the RpcServer class with specific port number.
		/// <param name="serverId">A unique identifier for this server instance.
		/// The same identifier should be used on the client to connect to this server.</param>
		/// <param name="port">A specific port to listen on.</param>
		/// </summary>
		RpcServer(const GUID& serverId, int port);

		/// <summary> 
		/// Initializes a new instance of the RpcServerImpl class.
		/// </summary>
		/// <param name="maxQueueSize">Maximum queue size for the server to start with.</param>
		RpcServer(unsigned int maxQueueSize);

		/// <summary>
		/// Initializes a new instance of the RpcServerImpl class with a specific port number.
		/// <param name="port">A port number to listen on.</param>
		/// <param name="maxQueueSize">Maximum queue size for the server to start with.</param>
		/// </summary>
		RpcServer(int port, unsigned int maxQueueSize);

		/// <summary> 
		/// Initializes a new instance of the RpcServerImpl class.
		/// <param name="serverId">A unique identifier for this server instance.
		/// <param name="maxQueueSize">Maximum queue size for the server to start with.</param>
		/// The same identifier should be used on the client to connect to this server.</param>
		/// </summary>
		RpcServer(const GUID& serverId, unsigned int maxQueueSize);

		/// <summary> 
		/// Initializes a new instance of the RpcServerImpl class with a specific port number.
		/// <param name="serverId">A unique identifier for this server instance.
		/// The same identifier should be used on the client to connect to this server.</param>
		/// <param name="port">A port number to listen on.</param>
		/// <param name="maxQueueSize">Maximum queue size for the server to start with.</param>
		/// </summary>
		RpcServer(const GUID& serverId, int port, unsigned int maxQueueSize);

		/// <summary> 
		/// Start this sever and begin listening for incoming messages.
		/// </summary>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
		HRESULT Start();

		/// <summary>
		/// Stops this server so that it no longer listens for messages.
		/// </summary>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
		HRESULT Stop();

		/// <summary>
		/// Synchronously processes requests received since the last call and raises queued events.
		/// </summary>
		/// <remarks>
		/// If this method is not called repeatedly to process and clear the request queue,
		/// hooks will not be called and remote method calls on respective clients will time out.
		/// Clients will also stop receiving event notifications for raised events.
		/// The request queue may also become full due to continuously arriving requests that
		/// aren't dequeued and processed. GAMESTEST_RPC_E_PENDING_REQUESTS_QUEUE_FULL error
		/// responses will be sent to all clients until the queue is processed.
		/// Attempting to raise events at that time will return the same error code.
		/// </remarks>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
		HRESULT ProcessPendingRequests();

		/// <summary>
		/// Raise an event to all subscribing clients.
		/// </summary>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
		HRESULT RaiseEvent(_In_z_ const wchar_t* eventName, _In_ const RpcArchive& archive);

		/// <summary>
		/// Raise an event to all subscribing clients.
		/// </summary>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
		HRESULT RaiseEvent(_In_z_ const wchar_t* eventName, _In_ RpcArchive&& archive);

		/// <summary>
		/// Gets a value indicating whether or not this server has been started.
		/// </summary>
		bool GetIsStarted();

		/// <summary>
		/// Gets a value indicating whether or not this server has been started.
		/// </summary>
		__declspec(property(get=GetIsStarted)) bool IsStarted;

		/// <summary>
		/// Gets the id of this RpcServer.
		/// </summary>
		const GUID& GetServerId();

		/// <summary>
		/// Gets the port number this server is listening on.
		/// </summary>
		int GetPort();

		/// <summary>
		/// Gets the default listening port.
		/// </summary>
		static int GetDefaultPort();

		/// <summary>
		/// Gets the default max. queue size for RpcServer.
		/// </summary>
		static unsigned int GetDefaultMaxQueueSize();

		/// <summary>
		/// Gets the id of this RpcServer.
		/// </summary>
		__declspec(property(get=GetServerId)) const GUID& ServerId;

		/// <summary>
		/// Gets the port number of this RpcServer.
		/// </summary>
		__declspec(property(get=GetPort)) int Port;

		/// <summary>
		/// Gets the default port number of this RpcServer.
		/// </summary>
		__declspec(property(get=GetDefaultPort)) int DefaultPort;

		/// <summary>
		/// Gets the default max. queue size of this RpcServer.
		/// </summary>
		__declspec(property(get=GetDefaultMaxQueueSize)) unsigned int DefaultMaxQueueSize;
		
		/// <summary>
		/// Gets a value indicating whether or not RpcServer methods have been enabled.
		/// </summary>
		static bool GetIsEnabled();
		
		/// <summary>
		/// Gets the whether or not this RpcServer's methods have been enabled.
		/// </summary>
		__declspec(property(get=GetIsEnabled)) bool IsEnabled;

		/// <summary> 
		/// Register a method with this server so that it can be invoked by remote clients.
		/// </summary>
		/// <param name="hookName">The name of the method on the remote interface.  
		/// Clients will use this name to invoke the hook regardless of the name of the
		/// method passed in the <paramref name="hook"/> parameter.</param>
		/// <param name="hook">A method to be made available to remote clients.</param>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
		HRESULT RegisterHookMethod(_In_z_ const wchar_t* hookName, _In_ RpcHook hook);

		/// <summary> 
		/// Register a method with this server so that it can be invoked by remote clients.
		/// </summary>
		/// <param name="hookName">The name of the method on the remote interface.  
		/// Clients will use this name to invoke the hook regardless of the name of the
		/// method passed in the <paramref name="hook"/> parameter.</param>
		/// <param name="hook">A method to be made available to remote clients.</param>
		/// <param name="immediate">Whether the hook will be processed immediately in a server thread instead of waiting until ProcessPendingRequests.</param>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
		HRESULT RegisterHookMethod(_In_z_ const wchar_t* hookName, _In_ RpcHook hook, bool immediate);

		/// <summary> 
		/// Virtual destructor.
		/// </summary>
		virtual ~RpcServer();

	private:
		/// <summary> 
		/// Private copy constructor to prevent the compiler from generating a public one.
		/// </summary>
		RpcServer(const RpcServer&);

		/// <summary> 
		/// Private assignment operator to prevent the compiler from generating a public one.
		/// </summary>
		RpcServer& operator=(const RpcServer&);

	private:
		friend class RpcServerImpl;

	private:
		RpcServerImpl* m_impl;
	};

}}}}}

#endif // _WINRT_DLL