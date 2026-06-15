#include "Dungeons.h"
#include "CloudFileUtil.h"

#if CLOUDSAVE_ENABLED

#if !CONSOLE_SAVE_SYSTEM 


class FCloudFileUtilWindows : public FCloudFileUtil
{
protected:
	virtual void WriteToLocalFileImpl(FStringArg _FileName, StringArg json)
	{
		auto FileName = GetFullPathForFile(_FileName);
		UCharacterSaveData::EnsurePathExists(FileName);
		auto Encryptedjson = binarysavedata::safer::encryptString(json);
		binarysavedata::safer::writeJsonToBinaryFileSafer(Encryptedjson, FileName);
	}

	virtual bool AreLocalFileOperationsCompletedImpl() const
	{
		return true;
	}

	virtual TArray<FString> GetLocalFileListImpl()
	{
		TArray<FString> LocalFileList;
		auto savePath = GetPath();
		IFileManager& FileManager = IFileManager::Get();
		FileManager.FindFiles(LocalFileList, *(savePath / "*" + globalsavedata::BinaryFileExtension), true, false);
		return LocalFileList;
	}

	virtual void DeleteLocalFileImpl(FStringArg f)
	{
		auto Path = GetFullPathForFile(f);
		IFileManager& FileManager = IFileManager::Get();
		FileManager.Delete(*Path);
	}

	virtual SerializedDataPtr LoadLocalFileImpl(FStringArg f)
	{
		auto FileName = GetFullPathForFile(f);
		auto json = binarysavedata::safer::loadJsonStringAndDecryptSafer(FileName);
		if (json.Num() == 0)
			return nullptr;
		return MakeSerializedDataPtr(json);
	}

	virtual bool IsLocalFileNameImpl(FString& FileName)
	{
		auto Index = FileName.Find(globalsavedata::BinaryFileExtension);
		if (!FileName.IsValidIndex(Index))
			return false;
		check(Index > 0);
		FileName = FileName.Mid(0, Index);
		FileName = ExtractFileNameFromPath(FileName);
		return true;
	}

	FString GetFullPathForFile(FStringArg File)
	{
		return GetPath() / File;
	}

	FString ExtractFileNameFromPath(FStringArg _FileName)
	{
		int32 Index = INDEX_NONE;
		FString FileName = _FileName;
		if (!FileName.FindLastChar('/', Index))
			FileName.FindLastChar('\\', Index);
		if (Index != INDEX_NONE)
			FileName = FileName.Mid(Index + 1);
		return FileName;
	}


};

std::unique_ptr<FCloudFileUtil> FCloudFileUtil::CreateInstance()
{
	return  std::make_unique<FCloudFileUtilWindows>();
}

#endif

#endif