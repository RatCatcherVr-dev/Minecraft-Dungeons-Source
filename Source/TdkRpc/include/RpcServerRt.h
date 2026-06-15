//------------------------------------------------------------------------------
// <copyright file="RpcServerRt.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server
{
	/// <summary> 
	/// Forward declarations
	/// </summary>
	ref class RpcArchive;
	class RpcServerImpl;

	/// <summary> 
	/// Used to enforce the use of our custom macro for hook return codes.
	/// Macro can be found under RpcHookCode.h
	/// </summary> 
	typedef int RpcHookCode;

	/// <summary> 
	/// Provides the definition of a hook in the TDK RPC system.
	/// </summary>
	public delegate RpcHookCode RpcHook(RpcArchive^ input, RpcArchive^ output);
	
	/// <summary> 
	/// Entry point into the server-side of the TDK RPC library for Windows Store apps.
	/// The RpcServer class is used to start and stop servers and register
	/// hook methods that are exposed to remote clients.
	/// </summary>
    public ref class RpcServer sealed
    {
    public:
        /// <summary> 
		/// Initializes a new instance of the RpcServer class.
		/// </summary>
		RpcServer();

		/// <summary> 
		/// Initializes a new instance of the RpcServer class.
		/// <param name="serverId">A unique identifier for this server instance.
		///		The same identifier should be used on the client to connect to this server.</param>
		/// </summary>
		RpcServer(Platform::Guid serverId);

		/// <summary> 
		/// Initializes a new instance of the RpcServer class with specific port number.
		/// <param name="serverId">A unique identifier for this server instance.
		///		The same identifier should be used on the client to connect to this server.</param>
		/// <param name="port">A specific port to listen on.</param>
		/// </summary>
		RpcServer(Platform::Guid serverId, int port);

		/// <summary> 
		/// Initializes a new instance of the RpcServerImpl class with a specific port number and max queue size.
		/// <param name="serverId">A unique identifier for this server instance.
		///		The same identifier should be used on the client to connect to this server.</param>
		/// <param name="port">A specific port to listen on.</param>
		/// <param name="maxQueueSize">Maximum queue size for the server to start with.</param>
		/// </summary>
		RpcServer(Platform::Guid serverId, int port, unsigned int maxQueueSize);

	public:
		/// <summary> 
		/// Start this sever and begin listening for incoming messages.
		/// </summary>
		void Start();

		/// <summary>
		/// Stops this server so that it no longer listens for messages.
		/// </summary>
		void Stop();

		/// <summary>
		/// Asynchronously processes all pending requests and raises queued events.
		/// </summary>
		void ProcessPendingRequestsAsync();

		/// <summary>
		/// Synchronously processes all pending requests and raises queued events.
		/// </summary>
		void ProcessPendingRequests();

		/// <summary>
		/// Raise an event to all subscribing clients.
		/// </summary>
		void RaiseEvent(Platform::String^ eventName, RpcArchive^ archive);

		/// <summary> 
		/// Register a method with this server so that it can be invoked by remote clients.
		/// The hook code executes on a different thread spawned every time a hook is called.
		/// </summary>
		/// <param name="hookName">The name of the method on the remote interface.  
		/// Clients will use this name to invoke the hook regardless of the name of the
		/// method passed in the <paramref name="hook"> parameter.</param>
		/// <param name="hook">A method to be made available to remote clients, specified as a delegate.</param>
		void RegisterHookMethodAsync(Platform::String^ hookName, RpcHook^ hook);

		/// <summary> 
		/// Register a method with this server so that it can be invoked by remote clients.
		/// The hook code executes on a different thread spawned every time a hook is called.
		/// </summary>
		/// <param name="hookName">The name of the method on the remote interface.  
		/// Clients will use this name to invoke the hook regardless of the name of the
		/// method passed in the <paramref name="hook"> parameter.</param>
		/// <param name="hook">A method to be made available to remote clients, specified as a delegate.</param>
		/// <param name="immediate">Whether the hook will be processed immediately in a server thread instead of waiting until ProcessPendingRequests.</param>
		void RegisterHookMethodAsync(Platform::String^ hookName, RpcHook^ hook, bool immediate);

		/// <summary> 
		/// Register a method with this server so that it can be invoked by remote clients.
		/// The hook code executes on the same thread as the code that called ProcessPendingRequests().
		/// </summary>
		/// <param name="hookName">The name of the method on the remote interface.  
		/// Clients will use this name to invoke the hook regardless of the name of the
		/// method passed in the <paramref name="hook"> parameter.</param>
		/// <param name="hook">A method to be made available to remote clients, specified as a delegate.</param>
		void RegisterHookMethod(Platform::String^ hookName, RpcHook^ hook);

		/// <summary> 
		/// Register a method with this server so that it can be invoked by remote clients.
		/// The hook code executes on the server receiving thread instead of waiting until ProcessPendingRequests().
		/// </summary>
		/// <param name="hookName">The name of the method on the remote interface.  
		/// Clients will use this name to invoke the hook regardless of the name of the
		/// method passed in the <paramref name="hook"> parameter.</param>
		/// <param name="hook">A method to be made available to remote clients, specified as a delegate.</param>
		/// <param name="immediate">Whether the hook will be processed immediately in a server thread instead of waiting until ProcessPendingRequests.</param>
		void RegisterHookMethod(Platform::String^ hookName, RpcHook^ hook, bool immediate);

	public:
		/// <summary>
		/// Gets the default listening port.
		/// </summary>
		static property int DefaultPort
		{
			int get()
			{
				return GetDefaultPort();
			}
		}

		/// <summary>
		/// Gets the port number this server is listening on.
		/// </summary>
		property int Port
		{
			int get()
			{
				return GetPort();
			}
		}

		/// <summary>
		/// Gets a value indicating whether or not this server has been started.
		/// </summary>
		property bool IsStarted
		{
			bool get()
			{
				return GetIsStarted();
			}
		}

		/// <summary>
		/// Gets the id of this RpcServer.
		/// </summary>
		property Platform::Guid ServerId
		{
			Platform::Guid get()
			{
				return GetServerId();
			}
		}

		/// <summary>
		/// Gets the default id of this RpcServer.
		/// </summary>
		static property Platform::Guid DefaultServerId
		{
			Platform::Guid get()
			{
				return GetDefaultServerId();
			}
		}

		/// <summary>
		/// Gets the default max. queue size for RpcServer.
		/// </summary>
		static property unsigned int DefaultMaxQueueSize
		{
			unsigned int get()
			{
				return GetDefaultMaxQueueSize();
			}
		}

	private:
		/// <summary>
		/// Destructor used to free internal implementation object.
		/// (can't use smart pointers because this requires RpcServerImpl to be defined,
		/// which we can't do because that creates circular header dependency).
		/// </summary>
		~RpcServer();

		/// <summary>
		/// Gets the default listening port.
		/// </summary>
		static int GetDefaultPort();

		/// <summary>
		/// Gets the port number this server is listening on.
		/// </summary>
		int GetPort();

		/// <summary>
		/// Gets a value indicating whether or not this server has been started.
		/// </summary>
		bool GetIsStarted();

		/// <summary>
		/// Gets the id of this RpcServer.
		/// </summary>
		Platform::Guid GetServerId();

		/// <summary>
		/// Gets the default id of this RpcServer.
		/// </summary>
		static Platform::Guid GetDefaultServerId();

		/// <summary>
		/// Gets the default max. queue size for RpcServer.
		/// </summary>
		static unsigned int GetDefaultMaxQueueSize();

	private:
		/// <summary>
		/// Allow RpcServerImpl access to internal members for unit tests.
		/// </summary>
		friend class RpcServerImpl;

	private:
		/// <summary>
		/// The internal server implementation.
		/// </summary>
		RpcServerImpl* m_impl;
    };

}}}}}
