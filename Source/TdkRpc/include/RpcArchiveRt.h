//------------------------------------------------------------------------------
// <copyright file="RpcArchiveRt.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#ifdef ENABLE_GAMESTEST_RPC
#ifdef _WINRT_DLL

#include <RpcMemory.h>
#include <RpcBinaryDataRt.h>

namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server
{
	/// <summary>
	/// This class is the data repository for the TDK RPC library.  All data that users
	/// retrieve or send through the TDK RPC library will pass through this class.
	/// </summary>
	public ref class RpcArchive sealed
	{
	public:
		/// <summary> 
		/// Initializes a new instance of the RpcArchive class.
		/// </summary>
		RpcArchive();

		/// <summary> 
		/// Initializes a new instance of the RpcArchive class.  This will create a deep copy
		/// of the RpcArchive given in the <paramref name="other"/> parameter.
		/// </summary>
		/// <param name="other">The existing archive from which a deep copy shall be made.</param>
		RpcArchive(RpcArchive^ other);

	public:
		/// <summary> 
		/// Indicates whether or not a field with the given name exists within this archive.
		/// </summary>
		/// <param name="fieldName">The name of the field to check for existence.</param>
		/// <returns>True if a field with the given names exists in the archive, otherwise false.</returns>
		bool HasNamedValue(Platform::String^ fieldName);

		/// <summary>
		/// Retrieves an RpcArchive value from the field with the given name.
		/// </summary>
		/// <param name="fieldName>The name of the field to retrieve the value from.</param>
		/// <returns>The value retrieved from the archive.</returns>
		RpcArchive^ GetNamedArchiveValue(Platform::String^ fieldName);

		/// <summary> 
		/// Sets a field with the given name to the given RpcArchive value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">The value to store in the RpcArchive.</param>
		void SetNamedArchiveValue(Platform::String^ fieldName, RpcArchive^ fieldValue);

		/// <summary> 
		/// Retrieves a boolean value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>The value retrieved from the archive.</returns>
		bool GetNamedBoolValue(Platform::String^ fieldName);

		/// <summary> 
		/// Sets a field with the given name to the given boolean value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">The value to store in the RpcArchive.</param>
		void SetNamedBoolValue(Platform::String^ fieldName, bool fieldValue);

		/// <summary> 
		/// Retrieves an unsigned 8 bit int value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>The value retrieved from the archive.</returns>
		uint8 GetNamedByteValue(Platform::String^ fieldName);

		/// <summary> 
		/// Sets a field with the given name to the given unsigned 8 bit int value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">The value to store in the RpcArchive.</param>
		void SetNamedByteValue(Platform::String^ fieldName, uint8 fieldValue);

		/// <summary> 
		/// Retrieves an unsigned 16 bit int value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>The value retrieved from the archive.</returns>
		uint16 GetNamedWordValue(Platform::String^ fieldName);

		/// <summary> 
		/// Sets a field with the given name to the given unsigned 16 bit int value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">The value to store in the RpcArchive.</param>
		void SetNamedWordValue(Platform::String^ fieldName, uint16 fieldValue);

		/// <summary> 
		/// Retrieves an 16 bit int value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>The value retrieved from the archive.</returns>
		int16 GetNamedShortValue(Platform::String^ fieldName);

		/// <summary> 
		/// Sets a field with the given name to the given 16 bit int value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">The value to store in the RpcArchive.</param>
		void SetNamedShortValue(Platform::String^ fieldName, int16 fieldValue);

		/// <summary> 
		/// Retrieves a 32 bit int value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>The value retrieved from the archive.</returns>
		int32 GetNamedIntValue(Platform::String^ fieldName);

		/// <summary> 
		/// Sets a field with the given name to the given 32 bit int value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">The value to store in the RpcArchive.</param>
		void SetNamedIntValue(Platform::String^ fieldName, int32 fieldValue);

		/// <summary> 
		/// Retrieves an unsigned 32 bit int value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>The value retrieved from the archive.</returns>
		uint32 GetNamedDwordValue(Platform::String^ fieldName);

		/// <summary> 
		/// Sets a field with the given name to the given unsigned 32 bit int value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">The value to store in the RpcArchive.</param>
		void SetNamedDwordValue(Platform::String^ fieldName, uint32 fieldValue);

		/// <summary> 
		/// Retrieves an 64 bit int value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>The value retrieved from the archive.</returns>
		int64 GetNamedInt64Value(Platform::String^ fieldName);

		/// <summary> 
		/// Sets a field with the given name to the given 64 bit int value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">The value to store in the RpcArchive.</param>
		void SetNamedInt64Value(Platform::String^ fieldName, int64 fieldValue);

		/// <summary> 
		/// Retrieves an unsigned 64 bit int value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>The value retrieved from the archive.</returns>
		uint64 GetNamedUInt64Value(Platform::String^ fieldName);

		/// <summary>
		/// Sets a field with the given name to the given unsigned 64 bit int value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">The value to store in the RpcArchive.</param>
		void SetNamedUInt64Value(Platform::String^ fieldName, uint64 fieldValue);

		/// <summary> 
		/// Retrieves a float value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>The value retrieved from the archive.</returns>
		float GetNamedFloatValue(Platform::String^ fieldName);

		/// <summary> 
		/// Sets a field with the given name to the given float value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">The value to store in the RpcArchive.</param>
		void SetNamedFloatValue(Platform::String^ fieldName, float fieldValue);

		/// <summary> 
		/// Retrieves a double value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>The value retrieved from the archive.</returns>
		double GetNamedDoubleValue(Platform::String^ fieldName);

		/// <summary> 
		/// Sets a field with the given name to the given double value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">The value to store in the RpcArchive.</param>
		void SetNamedDoubleValue(Platform::String^ fieldName, double fieldValue);

		/// <summary>
		/// Retrieves a string value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>A string read from the archive, or an exception thrown if there was an error.</returns>
		Platform::String^ GetNamedStringValue(Platform::String^ fieldName);

		/// <summary> 
		/// Sets a field with the given name to the given string value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">A string to store in the RpcArchive.</param>
		void SetNamedStringValue(Platform::String^ fieldName, Platform::String^ fieldValue);

		/// <summary>
		/// Retrieves a large binary value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>A blob read from the archive, or an exception thrown if there was an error.</returns>
		RpcBinaryData^ GetNamedBlobValue(Platform::String^ fieldName);

		/// <summary> 
		/// Sets a field with the given name to the given blob value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">A blob to store in the RpcArchive.</param>
		void SetNamedBlobValue(Platform::String^ fieldName, RpcBinaryData^ fieldValue);

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <returns>An array of archives that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</returns>
		Platform::Array<RpcArchive^>^ GetNamedArchiveCollection(Platform::String^ fieldName);

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">An array of bool's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		Platform::Array<bool>^ GetNamedBoolCollection(Platform::String^ fieldName);

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">An array of bytes that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		Platform::Array<uint8>^ GetNamedByteCollection(Platform::String^ fieldName);
		
		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">An array of short's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		Platform::Array<int16>^ GetNamedShortCollection(Platform::String^ fieldName);

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">An array of word's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		Platform::Array<uint16>^ GetNamedWordCollection(Platform::String^ fieldName);

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">An array of int's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		Platform::Array<int32>^ GetNamedIntCollection(Platform::String^ fieldName);

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">An array of dword's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		Platform::Array<uint32>^ GetNamedDwordCollection(Platform::String^ fieldName);

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">An array of int64's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		Platform::Array<int64>^ GetNamedInt64Collection(Platform::String^ fieldName);

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">An array of unsigned int64's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		Platform::Array<uint64>^ GetNamedUInt64Collection(Platform::String^ fieldName);

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <returns>An array of float's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</returns>
		Platform::Array<float>^ GetNamedFloatCollection(Platform::String^ fieldName);

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <returns>An array of double's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</returns>
		Platform::Array<double>^ GetNamedDoubleCollection(Platform::String^ fieldName);

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the string collection whose content is to be retrieved.</param>
		/// <returns>An array of strings where the collection referred to by the <paramref name="fieldName"/> parameter will be stored.</returns>
		Platform::Array<Platform::String^>^ GetNamedStringCollection(Platform::String^ fieldName);

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the string collection whose content is to be retrieved.</param>
		/// <returns>An array of strings where the collection referred to by the <paramref name="fieldName"/> parameter will be stored.</returns>
		Platform::Array<RpcBinaryData^>^ GetNamedBlobCollection(Platform::String^ fieldName);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">An array of RpcArchive's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		void SetNamedArchiveCollection(Platform::String^ fieldName, const Platform::Array<RpcArchive^>^ collectionBuffer);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of bool's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		void SetNamedBoolCollection(Platform::String^ fieldName, const Platform::Array<bool>^ collectionBuffer);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">An array of unsigned __int8's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		void SetNamedByteCollection(Platform::String^ fieldName, const Platform::Array<uint8>^ collectionBuffer);
		
		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">An array of short's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		void SetNamedShortCollection(Platform::String^ fieldName, const Platform::Array<int16>^ collectionBuffer);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">An array of word's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>		
		void SetNamedWordCollection(Platform::String^ fieldName, const Platform::Array<uint16>^ collectionBuffer);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">An array of int's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		void SetNamedIntCollection(Platform::String^ fieldName, const Platform::Array<int32>^ collectionBuffer);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">An array of dword's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		void SetNamedDwordCollection(Platform::String^ fieldName, const Platform::Array<uint32>^ collectionBuffer);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">An array of int64's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		void SetNamedInt64Collection(Platform::String^ fieldName, const Platform::Array<int64>^ collectionBuffer);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">An array of unsigned int64's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		void SetNamedUInt64Collection(Platform::String^ fieldName, const Platform::Array<uint64>^ collectionBuffer);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">An array of floats that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		void SetNamedFloatCollection(Platform::String^ fieldName, const Platform::Array<float>^ collectionBuffer);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">An array of doubles that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		void SetNamedDoubleCollection(Platform::String^ fieldName, const Platform::Array<double>^ collectionBuffer);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to be set.</param>
		/// <param name="strings">An array of strings that holds the starting address of each string in the collection referred to by the <paramref name="fieldName"/> parameter.</param>
		void SetNamedStringCollection(Platform::String^ fieldName, const Platform::Array<Platform::String^>^ strings);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to be set.</param>
		/// <param name="strings">An array of strings that holds the starting address of each string in the collection referred to by the <paramref name="fieldName"/> parameter.</param>
		void SetNamedBlobCollection(Platform::String^ fieldName, const Platform::Array<RpcBinaryData^>^ collectionBuffer);

	private:
		/// <summary>
		/// Friend class(es).
		/// </summary>
		friend ref class RpcServer;
		friend class RpcRequest;		
		friend class RpcServerImpl;

	private:
		/// <summary>
		/// Internal implementation being wrapped.
		/// </summary>
		RpcArchiveImplPtr m_impl;

	private:
		/// <summary> 
		/// Initializes a new instance of the RpcArchive class.  This will create an instance of the RpcArchive
		/// class based on the given existing implementation object.
		/// </summary>
		/// <param name="implementation">The existing implementation object to be used by the new RpcArchive.</param>
		RpcArchive(const RpcArchiveImpl& implementation);

		/// <summary>
		/// Gets the internal implementation object of an external archive interface.
		/// </summary>
		static RpcArchiveImpl& GetImplementation(const RpcArchive^ archive);

		/// <summary> 
		/// Creates a deep copy of the archive given by the <paramref name="other"/> parameter.
		/// </summary>
		/// <param name="other">The archive from which a deep copy shall be created.</param>
		RpcArchive^ operator=(RpcArchive^ other);

		/// <summary>
		/// Moves an existing RpcArchive to an existing RpcArchive.
		/// </summary>
		/// <param name="other">The existing archive to be moved.</param>
		RpcArchive^ operator=(RpcArchive^& other);

		/// <summary>
		/// Allocates a new internal implementation or reuses a previously allocated one from a pool.
		/// </summary>
		void SetImplementation();

		/// <summary>
		/// Used with std::move semantics to transfer archive data to another object.
		/// </summary>
		RpcArchiveImplPtr TransferImplementation();
	};

}}}}}

#endif // _WINRT_DLL
#endif // ENABLE_GAMESTEST_RPC