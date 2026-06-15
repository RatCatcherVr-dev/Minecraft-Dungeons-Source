//------------------------------------------------------------------------------
// <copyright file="RpcArchiveExtensions.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcArchiveExtensions.h"
#include "RpcArchive.h"
#include "RpcServer.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

using namespace GamesTest::Rpc::Server;

RpcHookCode GetStringValue(const wchar_t* FieldName, const RpcArchive& Archive, FString* OutString)
{
    HRESULT Result = S_OK;

    const int16 DefaultBufferSize = 128;
    wchar_t Buffer[DefaultBufferSize];
    wchar_t* BufferPtr = Buffer;

    ::DWORD StringLength = 0;
	::DWORD StringBufferLength = DefaultBufferSize;
    HRCHK(Archive.MeasureNamedStringValue(FieldName, &StringLength))

    // If the buffer won't be able to hold the value, allocate one that will.
    if (StringLength > DefaultBufferSize)
    {
        BufferPtr = new (std::nothrow) wchar_t[StringLength];
        if (!BufferPtr)
        {
            return UTF_E_CUSTOM_OUT_OF_MEMORY;
        }

        StringBufferLength = StringLength;
    }

    HRCHKCLEANUP(Archive.GetNamedStringValue(FieldName, BufferPtr, StringBufferLength, nullptr))
    
    // FString overrides its assignment operator
    // to do a deep copy. Does memcpy of chars.
    *OutString = BufferPtr;

Cleanup:
    if (BufferPtr != Buffer)
    {
        delete[] BufferPtr;
    }

    return Result;
}

template<typename StringType, typename AllocatorType>
RpcHookCode GetStringCollection(const wchar_t* FieldName, const Rpc::Server::RpcArchive& Archive, TArray<StringType, AllocatorType>& OutStrings)
{
	::DWORD numStrings;
	::DWORD numStringCharacters;
	Archive.MeasureNamedStringCollection(FieldName, &numStrings, &numStringCharacters);
	if (numStrings == 0)
	{
		return S_FALSE;
	}

	wchar_t** stringBuffer = new wchar_t*[numStrings]; // Pointers to content
	wchar_t* stringContents = new wchar_t[numStringCharacters]; // Combined string
	Archive.GetNamedStringCollection(FieldName, stringBuffer, numStrings, stringContents, numStringCharacters);
	OutStrings.Reserve(numStrings);
	for (int i = 0; i < (int)numStrings; i++)
	{
		wchar_t* rawString = stringBuffer[i];
		OutStrings.Add(rawString);
	}

	delete[] stringBuffer;
	delete[] stringContents;

	return S_OK;
}

// Template instantiation for functions to retrieve a string collection from an RpcArchive
template RpcHookCode GetStringCollection(const wchar_t* fieldName, const Rpc::Server::RpcArchive& archive, TArray<FString, FDefaultAllocator>& stringList);
template RpcHookCode GetStringCollection(const wchar_t* fieldName, const Rpc::Server::RpcArchive& archive, TArray<FString, TInlineAllocator<1>>& stringList);
template RpcHookCode GetStringCollection(const wchar_t* fieldName, const Rpc::Server::RpcArchive& archive, TArray<FName, FDefaultAllocator>& stringList);
template RpcHookCode GetStringCollection(const wchar_t* fieldName, const Rpc::Server::RpcArchive& archive, TArray<FName, TInlineAllocator<1>>& stringList);

template<typename AllocatorType>
RpcHookCode SetStringCollection(const wchar_t* FieldName, Rpc::Server::RpcArchive& Archive, const TArray<FString, AllocatorType>& StringCollection)
{
	RpcHookCode hr = S_OK;
	::DWORD listSize = StringCollection.Num();
	wchar_t** stringPointers = NULL;
	unsigned long i; // loop index

	try
	{
		// If stringList has no entries, add a dummy one to keep the call to SetNamedStringCollection from crashing.
		if (listSize < 1)
		{
			listSize = 1;
			stringPointers = new wchar_t*[listSize];
			*stringPointers = NULL;
			*stringPointers = new wchar_t[1];
			**stringPointers = 0;

			hr = S_FALSE;
		}
		else
		{
			stringPointers = new wchar_t*[listSize];

			// Set the stringPointers elements to NULL.
			for (i = 0; i < listSize; i++)
			{
				stringPointers[i] = NULL;
			}

			TArray<FString, AllocatorType>::TConstIterator it = StringCollection.CreateConstIterator();
			for (i = 0; it; it++, i++)
			{
				int len = (*it).Len();
				wchar_t* nameEntry = new wchar_t[len + 1];
				FCString::Strncpy(nameEntry, *(*it), len + 1);
				stringPointers[i] = nameEntry;
			}
		}
	}
	catch (std::bad_alloc&)
	{
		hr = E_OUTOFMEMORY;
	}

	if (SUCCEEDED(hr))
	{
		hr = Archive.SetNamedStringCollection(FieldName, stringPointers, listSize);
	}

	if (stringPointers)
	{
		for (i = 0; i < listSize; i++)
		{
			delete[] stringPointers[i];
		}

		delete[] stringPointers;
	}

	return hr;
}

// Template instantiation for functions to set a string collection from an RpcArchive
template RpcHookCode SetStringCollection(const wchar_t* fieldName, Rpc::Server::RpcArchive& archive, const TArray<FString, FDefaultAllocator>& StringCollection);

}}}}

#endif // ENABLE_GAMETEST_RPC
