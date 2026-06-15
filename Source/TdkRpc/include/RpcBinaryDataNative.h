//------------------------------------------------------------------------------
// <copyright file="RpcBinaryDataNative.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#ifdef ENABLE_GAMESTEST_RPC
#ifndef _WINRT_DLL

#include <memory>

#include "RpcBinaryToTextEncoding.h"
#include "RpcCompressionScheme.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server
{
	class RpcBinaryDataImpl;

	/// <summary> 
	/// This class is the data container used to send and receive binary data to and 
    /// from an RpcServer.
	/// </summary>
	class RpcBinaryData
	{
	public:
		typedef unsigned __int8* iterator;
		typedef const unsigned __int8* const_iterator;

	public:
		/// <summary> 
		/// Initializes a new instance of the RpcBinaryData class.
		/// </summary>
		RpcBinaryData();

		/// <summary> 
		/// Initializes a new instance of the RpcBinaryData class.
		/// </summary>
		/// <param name="data">The pointer to the an array of binary data.</param>
		/// <param name="length">The length of the array specified by <paramref name="data" /> parameter.</param>
		RpcBinaryData(std::shared_ptr<unsigned __int8> data, unsigned int length);

		/// <summary> 
		/// Initializes a new instance of the RpcBinaryData class.  This will create a deep copy
		/// of the RpcBinaryData given in the <paramref name="other"/> parameter.
		/// </summary>
		/// <param name="other">The existing binary data to make a deep copy of.</param>
		RpcBinaryData(const RpcBinaryData& other);

		/// <summary>
		/// Initializes a new instance of the RpcBinaryData class.  This will move an existing RpcBinaryData
		/// into a newly created RpcBinaryData.
		/// </summary>
		/// <param name="other">The existing binary data to be moved.</param>
		RpcBinaryData(RpcBinaryData&& other);

		/// <summary> 
		/// Creates a deep copy of the binary data given by the <paramref name="other"/> parameter.
		/// </summary>
		/// <param name="other">The binary data from which a deep copy shall be created.</param>
		RpcBinaryData& operator=(const RpcBinaryData& other);

		/// <summary>
		/// Moves an existing RpcBinaryData to an existing RpcBinaryData.
		/// </summary>
		/// <param name="other">The existing binary data to be moved.</param>
		RpcBinaryData& operator=(RpcBinaryData&& other);
		
		/// <summary>
		/// Returns an iterator to the beginning of the binary data.
		/// </summary>
		iterator begin();

		/// <summary>
		/// Returns an const_iterator to the beginning of the binary data.
		/// </summary>
		const_iterator begin() const;

		/// <summary>
		/// Returns an iterator to the end of the binary data.
		/// </summary>
		iterator end();		

		/// <summary>
		/// Returns an const_iterator to the end of the binary data.
		/// </summary>
		const_iterator end() const;

		/// <summary>
		/// Returns a pointer to the binary data array.
		/// </summary>
		unsigned __int8* Data();
		
		/// <summary>
		/// Returns a pointer to the binary data array.
		/// </summary>
		const unsigned __int8* Data() const;
		
		/// <summary>
		/// Returns the length of the binary data.
		/// </summary>
		unsigned int GetLength() const;

		/// <summary>
		/// Gets the length of the binary data.
		/// </summary>
		__declspec(property(get=GetLength)) unsigned int Length;

		virtual ~RpcBinaryData();

	private:		
		/// <summary>
		/// Returns the encoding used to encode the binary data.
		/// </summary>
		RpcBinaryToTextEncoding GetEncoding() const;

		/// <summary>
		/// Sets the encoding used to encode the binary data.
		/// </summary>
		void SetEncoding(RpcBinaryToTextEncoding encoding);

		/// <summary>
		/// Gets and sets the encoding used to encode the binary data.
		/// </summary>
		__declspec(property(get=GetEncoding, put=SetEncoding)) RpcBinaryToTextEncoding Encoding;
		
		/// <summary>
		/// Returns the compression scheme used to compress the binary data.
		/// </summary>
		RpcCompressionScheme GetCompression() const;
		
		/// <summary>
		/// Sets the compression scheme used to compress the binary data.
		/// </summary>
		void SetCompression(RpcCompressionScheme Compression);
		
		/// <summary>
		/// Gets and sets the compression scheme used to compress the binary data.
		/// </summary>
		__declspec(property(get=GetCompression, put=SetCompression)) RpcCompressionScheme Compression;


	private:
		friend class RpcArchive;

	private:
		RpcBinaryDataImpl* m_impl;

		static RpcBinaryDataImpl& GetImplementation(_In_ const RpcBinaryData* archive);
		void FreeImplementation();
	};
}}}}}

#endif // _WINRT_DLL
#endif // ENABLE_GAMESTEST_RPC