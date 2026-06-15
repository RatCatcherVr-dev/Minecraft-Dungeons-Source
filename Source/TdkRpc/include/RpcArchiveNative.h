//------------------------------------------------------------------------------
// <copyright file="RpcArchiveNative.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#ifdef ENABLE_GAMESTEST_RPC
#ifndef _WINRT_DLL

#include "RpcAnnotations.h"
#include "RpcMemory.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server { namespace Ut {
	class RpcArchiveTests;
}}}}}}

namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server
{
	class RpcArchiveImpl;
	class RpcBinaryData;

	/// <summary> 
	/// This class is the data repository for the TDK RPC library.  All data that users
	/// retrieve or send through the TDK RPC library will pass through this class.
	/// </summary>
	class RpcArchive
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
		RpcArchive(const RpcArchive& other);

		/// <summary>
		/// Initializes a new instance of the RpcArchive class.  This will move an existing RpcArchive
		/// into a newly created RpcArchive.
		/// </summary>
		/// <param name="other">The existing archive to be moved.</param>
		RpcArchive(RpcArchive&& other);

		/// <summary> 
		/// Initializes a new instance of the RpcArchive class.  This will create an instance of the RpcArchive
		/// class based on the given existing implementation object.
		/// </summary>
		/// <param name="implementation">The existing implementation object to be used by the new RpcArchive.</param>
		RpcArchive(const RpcArchiveImpl& implementation);

		/// <summary> 
		/// Creates a deep copy of the archive given by the <paramref name="other"/> parameter.
		/// </summary>
		/// <param name="other">The archive from which a deep copy shall be created.</param>
		RpcArchive& operator=(const RpcArchive& other);

		/// <summary>
		/// Moves an existing RpcArchive to an existing RpcArchive.
		/// </summary>
		/// <param name="other">The existing archive to be moved.</param>
		RpcArchive& operator=(RpcArchive&& other);

		/// <summary> 
		/// Indicates whether or not a field with the given name exists within this archive.
		/// </summary>
		/// <param name="fieldName">The name of the field to check for existence.</param>
		/// <returns>True if a field with the given names exists in the archive, otherwise false.</returns>
		bool HasNamedValue(_In_z_ const wchar_t* fieldName) const;

		/// <summary>
		/// Retrieves an RpcArchive value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <param name="fieldValue">A pointer to an RpcArchive to hold the value retrieved by this method.</param>
		/// <returns>S_OK if the field was able to be successfully converted to an RpcArchive, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ RpcArchive* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given RpcArchive value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">The value of the field to be set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, const RpcArchive& fieldValue);

		/// <summary> 
		/// Retrieves a boolean value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to a boolean, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ bool* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given boolean value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, bool fieldValue);

		/// <summary> 
		/// Retrieves an unsigned char value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to an unsigned char, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ unsigned char* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given unsigned char value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, unsigned char fieldValue);

		/// <summary> 
		/// Retrieves a char value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to an char, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ char* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given char value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, char fieldValue);

		/// <summary> 
		/// Retrieves a signed char value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to an char, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ signed char* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given signed char value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, signed char fieldValue);

		/// <summary> 
		/// Retrieves an unsigned short value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to an unsigned short, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ unsigned short* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given unsigned short value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, unsigned short fieldValue);

		/// <summary> 
		/// Retrieves an short value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to an short, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ short* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given short value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, short fieldValue);

		/// <summary> 
		/// Retrieves an int value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to an int, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ int* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given int value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, int fieldValue);

		/// <summary> 
		/// Retrieves an unsigned int value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to an unsigned int, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ unsigned int* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given unsigned int value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, unsigned int fieldValue);

		/// <summary> 
		/// Retrieves an unsigned long value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to an unsigned long, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ long* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given unsigned long value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, long fieldValue);

		/// <summary> 
		/// Retrieves an unsigned long value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to an unsigned long, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ unsigned long* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given unsigned long value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, unsigned long fieldValue);

		/// <summary> 
		/// Retrieves an long long value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to an long long, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ long long* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given long long value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, long long fieldValue);

		/// <summary> 
		/// Retrieves an unsigned long long value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to an unsigned long long, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ unsigned long long* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given unsigned long long value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, unsigned long long fieldValue);

		/// <summary> 
		/// Retrieves a float value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to a float, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ float* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given float value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, float fieldValue);

		/// <summary> 
		/// Retrieves a double value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <returns>S_OK if the field was able to be successfully converted to a double, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ double* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given double value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, double fieldValue);

		/// <summary>
		/// Retrieves an RpcBinaryData value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <param name="fieldValue">A pointer to an RpcBinaryData to hold the value retrieved by this method.</param>
		/// <returns>S_OK if the field was able to be successfully converted to an RpcArchive, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedValue(_In_z_ const wchar_t* fieldName, _Out_ RpcBinaryData* fieldValue) const;

		/// <summary> 
		/// Sets a field with the given name to the given RpcBinaryData value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">The value of the field to be set.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedValue(_In_z_ const wchar_t* fieldName, const RpcBinaryData& fieldValue);

		/// <summary>
		/// Gets the length of the string field with the given name.  The length includes the null termination character.
		/// </summary>
		/// <param name="fieldName">The name of the field that contains the string to be measured.</param>
		/// <param name="stringLength">A pointer to a unsigned long that will contain the number of characters in the string indicated by the <paramref name="fieldName"/> parameter.</param>
		/// <returns>S_OK if the field was located and its length was able to be measured, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT MeasureNamedStringValue(_In_z_ const wchar_t* fieldName, _Out_ unsigned long* stringLength) const;

		/// <summary>
		/// Retrieves a string value from the field with the given name.
		/// </summary>
		/// <param name="fieldName">The name of the field to retrieve the value from.</param>
		/// <param name="buffer">A pointer to a user-allocated array of wchar_t where the string value will be written.</param>
		/// <param name="bufferLength">The length of the array given in the <paramref name="buffer"/> parameter.</param>
		/// <param name="charactersWritten">Optional.  Output variable that will contain the number of wchar_t characters written to the buffer pointed to by the <paramref name="buffer"/> parameter,
		/// including the null-termination character.</param>
		/// <returns>S_OK if the string value was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT GetNamedStringValue(_In_z_ const wchar_t* fieldName, _Out_writes_z_(bufferLength) wchar_t* buffer, _In_  unsigned long bufferLength, _Out_opt_ unsigned long* charactersWritten) const;

		/// <summary> 
		/// Sets a field with the given name to the given string value.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="fieldValue">A pointer to a null-terminated string to store in the RpcArchive.</param>
		/// <returns>S_OK if the field was able to be set successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD
			HRESULT SetNamedStringValue(_In_z_ const wchar_t* fieldName, _In_z_ const wchar_t* fieldValue);

		/// <summary>
		/// Gets the number of elements in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose length is to be retrieved.</param>
		/// <param name="numElements">A pointer to a unsigned long that will contain the number of elements in the collection indicated by the <paramref name="fieldName"/> parameter.</param>
		/// <returns>S_OK if the number of elements in the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT MeasureNamedCollection(_In_z_ const wchar_t* fieldName, _Out_ unsigned long* numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of RpcArchive's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) RpcArchive* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of bool's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) bool* collectionBuffer, _In_ unsigned long numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of signed char's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) signed char* collectionBuffer, _In_ unsigned long numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of char's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) char* collectionBuffer, _In_ unsigned long numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of unsigned char's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) unsigned char* collectionBuffer, _In_ unsigned long numElements) const;
		
		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of short's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) short* collectionBuffer, _In_ unsigned long numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of unsigned short's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) unsigned short* collectionBuffer, _In_ unsigned long numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of int's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) int* collectionBuffer, _In_ unsigned long numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of unsigned int's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) unsigned int* collectionBuffer, _In_ unsigned long numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of long's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) long* collectionBuffer, _In_ unsigned long numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of unsigned long's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) unsigned long* collectionBuffer, _In_ unsigned long numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of long long's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) long long* collectionBuffer, _In_ unsigned long numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of unsigned long long's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) unsigned long long* collectionBuffer, _In_ unsigned long numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of floats that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) float* collectionBuffer, _In_ unsigned long numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of doubles that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) double* collectionBuffer, _In_ unsigned long numElements) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the collection whose content is to be retrieved.</param>
		/// <param name="collectionBuffer">A pointer to an array of RpcBinaryData's that will be used to hold the content of collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numElements) RpcBinaryData* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Gets both the number of strings in the field with given name as well as the total number of characters contained within the collection.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the string collection whose length is to be retrieved.</param>
		/// <param name="numStrings">A pointer to a unsigned long that will contain the number of strings in the collection indicated by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="totalNumStringCharacters">A pointer to a unsigned long that will contain the total number of all characters in the entire string collection.  This includes space for null-termination characters.</param>
		/// <returns>S_OK if the number of strings in the collection and the total number characters in the collection were able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT MeasureNamedStringCollection(_In_z_ const wchar_t* fieldName, _Out_ unsigned long* numStrings, _Out_ unsigned long* totalNumStringCharacters) const;

		/// <summary>
		/// Gets the contents of the collection in the field with given name.
		/// </summary>
		/// <param name="fieldName">The name of the field that represents the string collection whose content is to be retrieved.</param>
		/// <param name="stringPointers">A pointer to an array of wchar_t pointers that will be used to hold the starting address of each string in the collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numStringPointers">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="stringPointers"/> parameter.</param>
		/// <param name="stringPointers">A pointer to an array of wide characters that will be used to hold the entire content of every string in the collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numStringCharacters">A unsigned long that indicates the number of wide characters in the array pointed to by the <paramref name="stringContents"/> parameter.</param>
		/// <returns>S_OK if the collection was able to retrieved successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT GetNamedStringCollection(_In_z_ const wchar_t* fieldName, _Out_writes_(numStringPointers) wchar_t** stringPointers, _In_ unsigned long numStringPointers, _Out_writes_(numStringCharacters) wchar_t* stringContents, _In_ unsigned long numStringCharacters) const;

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of RpcArchive's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) RpcArchive* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of bool's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) bool* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of signed char's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) signed char* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of char's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) char* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of unsigned char's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) unsigned char* collectionBuffer, _In_ unsigned long numElements);
		
		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of short's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) short* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of unsigned short's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) unsigned short* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of int's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) int* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of unsigned int's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) unsigned int* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of long's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) long* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of unsigned long's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) unsigned long* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of long long's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) long long* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of unsigned long long's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) unsigned long long* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of floats that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) float* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of doubles that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) double* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to set.</param>
		/// <param name="collectionBuffer">A pointer to an array of RpcBinaryData's that will be added to the archive as a collection with the name referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numElements">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="collectionBuffer"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numElements) RpcBinaryData* collectionBuffer, _In_ unsigned long numElements);

		/// <summary>
		/// Sets a field with the given name to the given collection.
		/// </summary>
		/// <param name="fieldName">The name of the field to be set.</param>
		/// <param name="stringPointers">A pointer to an array of wchar_t pointers that holds the starting address of each string in the collection referred to by the <paramref name="fieldName"/> parameter.</param>
		/// <param name="numStrings">A unsigned long that indicates the total number of elements in the array pointed to by the <paramref name="stringPointers"/> parameter.</param>
		/// <returns>S_OK if the collection was able to be added to the archive successfully, otherwise an HRESULT representing a failure.</returns>
		GAMESTEST_ANNOTATE_HRESULT_METHOD HRESULT SetNamedStringCollection(_In_z_ const wchar_t* fieldName, _In_reads_(numStrings) const wchar_t* const* stringPointers, _In_ unsigned long numStrings);

	private:
		friend class RpcServer;

		/// <summary> 
		/// Helps unit-test the internal archive functionality.
		/// </summary>
		friend class Microsoft::Internal::GamesTest::Rpc::Server::Ut::RpcArchiveTests;

	private:
		RpcArchiveImplPtr m_impl;

		static RpcArchiveImpl& GetImplementation(_In_ const RpcArchive* archive);
		void SetImplementation();
		RpcArchiveImplPtr TransferImplementation();
	};
}}}}}

#endif // _WINRT_DLL
#endif // ENABLE_GAMESTEST_RPC