#pragma once

#include <string>
#include <Xal/xal_platform.h>

namespace XalExtra
{

namespace Win32
{

/// <summary>
/// Helper providing a simple file based implementation of the Xal storage event
/// handlers.
/// </summary>
class FileStorage
{
public:
    /// <summary>
    /// Registers the storage callbacks with Xal.
    /// </summary>
    /// <param name="queue">The async queue to run the event handlers on.
    /// </param>
    /// <param name="pathPrefix">Optional path prefix, see remarks</param>
    /// <remarks>
    /// This function should be called once before XalInit is called.
    ///
    /// The handlers will create one file per key in the following form:
    /// {pathPrefix}{key}.json
    /// </remarks>
    static void Init(XTaskQueueHandle queue, _In_opt_z_ char const* pathPrefix);

private:
    static void OnWrite(
        _In_opt_ void* context,
        _In_opt_ void* userContext,
        _In_ XalPlatformOperation operation,
        _In_z_ char const* key,
        _In_ size_t dataSize,
        _In_reads_bytes_(dataSize) void const* data
    );
    static void OnRead(
        _In_opt_ void* context,
        _In_opt_ void* userContext,
        _In_ XalPlatformOperation operation,
        _In_z_ char const* key
    );
    static void OnClear(
        _In_opt_ void* context,
        _In_opt_ void* userContext,
        _In_ XalPlatformOperation operation,
        _In_z_ char const* key
    );
};

namespace Detail
{

struct FileHandleCloser
{
    void operator()(HANDLE f)
    {
        CloseHandle(f);
    }
};

using FileHandle = std::unique_ptr<std::remove_pointer<HANDLE>::type, FileHandleCloser>;

std::string& PathPrefix()
{
    static std::string s_pathPrefix;
    return s_pathPrefix;
}

std::string MakeName(char const* key)
{
    return PathPrefix() + key + ".json";
}

}

inline
/*static*/ void FileStorage::Init(XTaskQueueHandle queue, _In_opt_z_ char const* pathPrefix)
{
    if (pathPrefix)
    {
        Detail::PathPrefix() = pathPrefix;
    }

    XalPlatformStorageEventHandlers handlers = {};
    handlers.write = &OnWrite;
    handlers.read = &OnRead;
    handlers.clear = &OnClear;
    handlers.context = nullptr;

    HRESULT hr = XalPlatformStorageSetEventHandlers(queue, &handlers);
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);
}

inline
/*static*/ void FileStorage::OnWrite(
    _In_opt_ void* /*context*/,
    _In_opt_ void* /*userContext*/,
    _In_ XalPlatformOperation operation,
    _In_z_ char const* key,
    _In_ size_t dataSize,
    _In_reads_bytes_(dataSize) void const* data
)
{
    std::string name = Detail::MakeName(key);

    Detail::FileHandle f{ CreateFileA(
        name.c_str(),
        GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    ) };

    if (f.get() == INVALID_HANDLE_VALUE)
    {
        XalPlatformStorageWriteComplete(operation, XalPlatformOperationResult_Failure);
        return;
    }

    DWORD written = 0;
    if (!WriteFile(f.get(), data, static_cast<DWORD>(dataSize), &written, nullptr) || written != dataSize)
    {
        XalPlatformStorageWriteComplete(operation, XalPlatformOperationResult_Failure);
        return;
    }

    XalPlatformStorageWriteComplete(operation, XalPlatformOperationResult_Success);
}

inline
/*static*/ void FileStorage::OnRead(
    _In_opt_ void* /*context*/,
    _In_opt_ void* /*userContext*/,
    _In_ XalPlatformOperation operation,
    _In_z_ char const* key
)
{
    std::string name = Detail::MakeName(key);

    Detail::FileHandle f{ CreateFileA(
        name.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    ) };

    if (f.get() == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();

        if (error == ERROR_FILE_NOT_FOUND)
        {
            XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Success, 0, nullptr);
            return;
        }
        else
        {
            XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Failure, 0, nullptr);
            return;
        }
    }

    LARGE_INTEGER fileSize = {0};
    if( !GetFileSizeEx(f.get(), &fileSize) )
     {
        XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Failure, 0, nullptr);
        return;
     }
     
    DWORD read = 0;
    std::vector<uint8_t> data(fileSize.LowPart);
    if (!ReadFile(f.get(), data.data(), static_cast<DWORD>(data.size()), &read, nullptr) || read != data.size())
    {
        XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Failure, 0, nullptr);
        return;
    }

    XalPlatformStorageReadComplete(operation, XalPlatformOperationResult_Success, data.size(), data.data());
}

inline
/*static*/ void FileStorage::OnClear(
    _In_opt_ void* /*context*/,
    _In_opt_ void* /*userContext*/,
    _In_ XalPlatformOperation operation,
    _In_z_ char const* key
)
{
    std::string name = Detail::MakeName(key);

    if (!DeleteFileA(name.c_str()))
    {
        DWORD error = GetLastError();

        if (error == ERROR_FILE_NOT_FOUND)
        {
            XalPlatformStorageClearComplete(operation, XalPlatformOperationResult_Success);
            return;
        }
        else
        {
            std::cout << "    Failed to delete file with error " << GetLastError() << std::endl;
            XalPlatformStorageClearComplete(operation, XalPlatformOperationResult_Failure);
            return;
        }
    }

    XalPlatformStorageClearComplete(operation, XalPlatformOperationResult_Success);
}

}

}
