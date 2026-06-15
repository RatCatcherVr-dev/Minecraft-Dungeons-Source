//------------------------------------------------------------------------------
// <copyright file="RpcBinaryDataRt.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#ifdef ENABLE_GAMESTEST_RPC
#ifdef _WINRT_DLL

#include "RpcBinaryToTextEncoding.h"
#include "RpcCompressionScheme.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server
{
	class RpcBinaryDataImpl;

	/// <summary> 
	/// This class is the data container used to send and receive binary data to and 
    /// from an RpcServer.
	/// </summary>
	public ref class RpcBinaryData sealed
	{
	public:
		/// <summary> 
		/// Initializes a new instance of the RpcBinaryData class.
		/// </summary>
		RpcBinaryData();

		/// <summary> 
		/// Initializes a new instance of the RpcBinaryData class.
		/// </summary>
		/// <param name="data">The array of data to use.</param>
		[Windows::Foundation::Metadata::DefaultOverloadAttribute]
		RpcBinaryData(Windows::Storage::Streams::IBuffer^ data);

		/// <summary> 
		/// Initializes a new instance of the RpcBinaryData class.  This will create a deep copy
		/// of the RpcBinaryData given in the <paramref name="other"/> parameter.
		/// </summary>
		/// <param name="other">The existing RpcBinaryData from which a deep copy shall be made.</param>
		RpcBinaryData(RpcBinaryData^ other);

	public:
		/// <summary>
		/// Gets or sets the binary data.
		/// </summary>
		property Windows::Storage::Streams::IBuffer^ Data
		{
			Windows::Storage::Streams::IBuffer^ get()
			{
				return GetData();
			}

			void set(Windows::Storage::Streams::IBuffer^ value)
			{
				SetData(value);
			}
		}

	private:
		/// <summary>
		/// Gets or sets the encoding for the binary data.
		/// </summary>
		property RpcBinaryToTextEncoding Encoding
		{
			RpcBinaryToTextEncoding get()
			{
				return GetEncoding();
			}

			void set(RpcBinaryToTextEncoding value)
			{
				SetEncoding(value);
			}
		}

		/// <summary>
		/// Gets or sets the compression for the binary data.
		/// </summary>
		property RpcCompressionScheme Compression
		{
			RpcCompressionScheme get()
			{
				return GetCompression();
			}

			void set(RpcCompressionScheme value)
			{
				SetCompression(value);
			}
		}

	private:
		/// <summary>
		/// Gets the data that was passed in.
		/// </summary>
		Windows::Storage::Streams::IBuffer^ GetData();

		/// <summary>
		/// Sets the data that was passed in.
		/// </summary>
		void SetData(Windows::Storage::Streams::IBuffer^ data);

		/// <summary>
		/// Gets the encoding for the binary data.
		/// </summary>
		RpcBinaryToTextEncoding GetEncoding();

		/// <summary>
		/// Sets the encoding for the binary data.
		/// </summary>
		void SetEncoding(RpcBinaryToTextEncoding encoding);
		
		/// <summary>
		/// Gets the compression for the binary data.
		/// </summary>
		RpcCompressionScheme GetCompression();
		
		/// <summary>
		/// Sets the compression for the binary data.
		/// </summary>
		void SetCompression(RpcCompressionScheme compression);

		
		/// <summary>
		/// Gets the native pointer for a buffer.
		/// </summary>
		unsigned __int8* GetDataPointer(Windows::Storage::Streams::IBuffer^ buffer);

	private:
		/// <summary>
		/// Internal implementation being wrapped.
		/// </summary>
		RpcBinaryDataImpl* m_impl;
		Windows::Storage::Streams::IBuffer^ m_data;

	private:
		/// <summary>
		/// Initializes an instance of the RpcBinaryData class using an existing implementation object.
		/// </summary>
		RpcBinaryData(const RpcBinaryDataImpl& implementation);

		/// <summary>
		/// Destructor used to free internal implementation object.
		/// </summary>
		~RpcBinaryData();

		/// <summary>
		/// Gets the internal implementation object of an external RpcBinaryData interface.
		/// </summary>
		/// <param name="data">The data from which the internal implementation object shall be retrieved.</param>
		RpcBinaryDataImpl& GetImplementation();

		/// <summary> 
		/// Creates a deep copy of the RpcBinaryData given by the <paramref name="other"/> parameter.
		/// </summary>
		/// <param name="other">The data from which a deep copy shall be created.</param>
		RpcBinaryData^ operator=(RpcBinaryData^ other);

		/// <summary>
		/// Moves an existing RpcBinaryData to an existing RpcBinaryData.
		/// </summary>
		/// <param name="other">The existing RpcBinaryData to be moved.</param>
		RpcBinaryData^ operator=(RpcBinaryData^& other);

		/// <summary>
		/// Allocates a new internal implementation.
		/// </summary>
		void SetImplementation();

		/// <summary>
		/// Frees the internal implementation object.
		/// </summary>
		void FreeImplementation();

	private:
		friend ref class RpcArchive;
	};

}}}}}

#endif // _WINRT_DLL
#endif // ENABLE_GAMESTEST_RPC