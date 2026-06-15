#pragma once

#include "DungeonsCloudLoadSave.h"

#include "..\DungeonsConsoleSave.h"
#include "..\BinarySaveData.h"
#include "..\GlobalSaveData.h"
#include "SaveGameSystem.h"

#if CLOUDSAVE_ENABLED

class FCloudFileUtil
{
public:
	FCloudFileUtil();
	virtual ~FCloudFileUtil();
	static std::unique_ptr<FCloudFileUtil> CreateInstance();
	static void PostProccessFileList(TArray<FString>& saveFiles);
	void SetFilePath(SystemId Id, FStringArg Path);
	void WriteToLocalFile(FStringArg _FileName, SerializedDataPtr pContents);
	TArray<FString> GetLocalFileList();
	void DeleteLocalFile(FStringArg f);
	SerializedDataPtr LoadLocalFile(FStringArg f);
	static SerializedDataPtr MakeSerializedDataPtr(SerializedDataInput pBegin, int Size);
	static SerializedDataPtr MakeSerializedDataPtr(int Size);
	static SerializedDataPtr MakeSerializedDataPtr(const TArray<uint8>& Contents);
	static SerializedDataPtr MakeSerializedDataPtr();
	static SerializedDataPtr MakeSerializedDataPtr(const std::string& Data);
	bool AreLocalFileOperationsCompleted() const;
protected:
	bool IsLocalFileName(FString FileName);
	static bool IsHexString(FStringArg FileName);
	FString GetPath();
	virtual bool AreLocalFileOperationsCompletedImpl() const = 0;
	virtual void WriteToLocalFileImpl(FStringArg _FileName, StringArg Contents) = 0;
	virtual TArray<FString> GetLocalFileListImpl() = 0;
	virtual void DeleteLocalFileImpl(FStringArg f) = 0;
	virtual SerializedDataPtr LoadLocalFileImpl(FStringArg f) = 0;
	virtual bool IsLocalFileNameImpl(FString& FileName) = 0;
	SystemId GetSystemId() const;
	SystemId mCurrentSystemId = -1;
	FString mCurrentPath;
};

#endif